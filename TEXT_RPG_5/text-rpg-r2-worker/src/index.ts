export interface Env {
  LEADERBOARD_BUCKET: R2Bucket;
  RATE_LIMIT_KV: KVNamespace;
  API_TOKEN: string;
}

const LEADERBOARD_KEY = "text-rpg-leaderboard.txt";

// 같은 IP 기준 5분에 3회 POST 허용
const RATE_LIMIT_WINDOW_SECONDS = 300;
const RATE_LIMIT_MAX_REQUESTS = 3;

// 비정상 기록 방지용
const MIN_CLEAR_TIME_MS = 10_000;              // 10초 미만 클리어 기록 거부
const MAX_CLEAR_TIME_MS = 24 * 60 * 60 * 1000; // 24시간 초과 기록 거부
const MIN_LEVEL = 1;
const MAX_LEVEL = 9999;
const MAX_NAME_LENGTH = 20;
const MAX_LEADERBOARD_COUNT = 100;

type LeaderboardEntry = {
  time: string; // "00:00:00.000"
  level: number;
  name: string;
};

type RateLimitResult = {
  allowed: boolean;
  ip: string;
  remaining: number;
};

function jsonResponse(
  data: unknown,
  status = 200,
  extraHeaders?: HeadersInit
): Response {
  return new Response(JSON.stringify(data, null, 2), {
    status,
    headers: {
      "Content-Type": "application/json; charset=utf-8",
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
      "Access-Control-Allow-Headers": "Content-Type, Authorization",
      ...extraHeaders,
    },
  });
}

function textResponse(text: string, status = 200): Response {
  return new Response(text, {
    status,
    headers: {
      "Content-Type": "text/plain; charset=utf-8",
      "Access-Control-Allow-Origin": "*",
      "Access-Control-Allow-Methods": "GET, POST, OPTIONS",
      "Access-Control-Allow-Headers": "Content-Type, Authorization",
    },
  });
}

function checkAuth(request: Request, env: Env): boolean {
  const auth = request.headers.get("Authorization");

  if (!auth) {
    return false;
  }

  return auth === `Bearer ${env.API_TOKEN}`;
}

function getClientIp(request: Request): string {
  const cfIp = request.headers.get("CF-Connecting-IP");

  if (cfIp && cfIp.trim().length > 0) {
    return cfIp.trim();
  }

  const forwardedFor = request.headers.get("X-Forwarded-For");

  if (forwardedFor && forwardedFor.trim().length > 0) {
    return forwardedFor.split(",")[0].trim();
  }

  return "unknown";
}

async function checkPostRateLimit(
  request: Request,
  env: Env,
  actionName: string
): Promise<RateLimitResult> {
  const ip = getClientIp(request);

  if (ip === "unknown") {
    return {
      allowed: false,
      ip,
      remaining: 0,
    };
  }

  const key = `rate-limit:${actionName}:${ip}`;
  const currentValue = await env.RATE_LIMIT_KV.get(key);
  const currentCount = currentValue ? Number(currentValue) : 0;

  if (Number.isFinite(currentCount) && currentCount >= RATE_LIMIT_MAX_REQUESTS) {
    return {
      allowed: false,
      ip,
      remaining: 0,
    };
  }

  const nextCount = Number.isFinite(currentCount) ? currentCount + 1 : 1;

  await env.RATE_LIMIT_KV.put(key, String(nextCount), {
    expirationTtl: RATE_LIMIT_WINDOW_SECONDS,
  });

  return {
    allowed: true,
    ip,
    remaining: Math.max(0, RATE_LIMIT_MAX_REQUESTS - nextCount),
  };
}

function isValidTimeFormat(time: string): boolean {
  return /^(\d{2}):(\d{2}):(\d{2})\.(\d{3})$/.test(time);
}

function parseTimeToMs(timeText: string): number {
  const match = timeText.match(/^(\d{2}):(\d{2}):(\d{2})\.(\d{3})$/);

  if (!match) {
    return Number.MAX_SAFE_INTEGER;
  }

  const hours = Number(match[1]);
  const minutes = Number(match[2]);
  const seconds = Number(match[3]);
  const milliseconds = Number(match[4]);

  if (
    !Number.isFinite(hours) ||
    !Number.isFinite(minutes) ||
    !Number.isFinite(seconds) ||
    !Number.isFinite(milliseconds)
  ) {
    return Number.MAX_SAFE_INTEGER;
  }

  if (
    hours < 0 ||
    minutes < 0 ||
    minutes >= 60 ||
    seconds < 0 ||
    seconds >= 60 ||
    milliseconds < 0 ||
    milliseconds >= 1000
  ) {
    return Number.MAX_SAFE_INTEGER;
  }

  return (
    hours * 60 * 60 * 1000 +
    minutes * 60 * 1000 +
    seconds * 1000 +
    milliseconds
  );
}

function sanitizeName(name: string): string {
  return name
    .replaceAll(",", "_")
    .replaceAll("\n", "")
    .replaceAll("\r", "")
    .replaceAll("\t", " ")
    .trim()
    .slice(0, MAX_NAME_LENGTH);
}

async function readLeaderboardText(env: Env): Promise<string> {
  console.log("Read key:", LEADERBOARD_KEY);

  const object = await env.LEADERBOARD_BUCKET.get(LEADERBOARD_KEY);

  if (object === null) {
    console.log("R2 object not found. Creating empty leaderboard.");

    const defaultText = "";

    await env.LEADERBOARD_BUCKET.put(LEADERBOARD_KEY, defaultText, {
      httpMetadata: {
        contentType: "text/plain; charset=utf-8",
      },
    });

    return defaultText;
  }

  console.log("R2 object found.");

  return await object.text();
}

async function writeLeaderboardText(env: Env, text: string): Promise<void> {
  await env.LEADERBOARD_BUCKET.put(LEADERBOARD_KEY, text, {
    httpMetadata: {
      contentType: "text/plain; charset=utf-8",
    },
  });
}

function parseLeaderboard(text: string): LeaderboardEntry[] {
  const lines = text
    .split(/\r?\n/)
    .map((line) => line.trim())
    .filter((line) => line.length > 0)
    .filter((line) => !line.startsWith("#"));

  const result: LeaderboardEntry[] = [];

  for (const line of lines) {
    // 현재 포맷:
    // 1. 00:03:25.142, 12, Knight
    {
      const match = line.match(/^\d+\.\s*([^,]+),\s*(\d+),\s*(.+)$/);

      if (match) {
        const time = match[1].trim();
        const level = Number(match[2]);
        const name = match[3].trim();

        if (
          isValidTimeFormat(time) &&
          parseTimeToMs(time) !== Number.MAX_SAFE_INTEGER &&
          Number.isFinite(level) &&
          level >= MIN_LEVEL &&
          level <= MAX_LEVEL &&
          name.length > 0
        ) {
          result.push({
            time,
            level: Math.floor(level),
            name: sanitizeName(name),
          });
        }

        continue;
      }
    }

    // 구 포맷 호환:
    // Rank,Name,Score
    // 1,Knight,1200
    // 이 경우 time 정보가 없으므로 뒤쪽으로 보내기 위해 99:59:59.999 처리
    {
      const match = line.match(/^(\d+),\s*([^,]+),\s*(\d+)$/);

      if (match) {
        const name = match[2].trim();
        const score = Number(match[3]);

        if (name.length > 0 && Number.isFinite(score)) {
          result.push({
            time: "99:59:59.999",
            level: Math.floor(score),
            name: sanitizeName(name),
          });
        }

        continue;
      }
    }

    if (line.toLowerCase() === "rank,name,score") {
      continue;
    }
  }

  return result;
}

function buildLeaderboardText(entries: LeaderboardEntry[]): string {
  const sorted = entries
    .slice()
    .sort((a, b) => {
      const timeA = parseTimeToMs(a.time);
      const timeB = parseTimeToMs(b.time);

      // 1순위: 클리어 시간이 짧을수록 상위
      if (timeA !== timeB) {
        return timeA - timeB;
      }

      // 2순위: 시간이 같으면 level 높은 순
      if (a.level !== b.level) {
        return b.level - a.level;
      }

      // 3순위: 이름 사전순
      return a.name.localeCompare(b.name);
    })
    .slice(0, MAX_LEADERBOARD_COUNT);

  const lines: string[] = [];

  for (let i = 0; i < sorted.length; ++i) {
    const index = i + 1;
    const entry = sorted[i];

    lines.push(`${index}. ${entry.time}, ${entry.level}, ${sanitizeName(entry.name)}`);
  }

  if (lines.length === 0) {
    return "";
  }

  return lines.join("\n") + "\n";
}

async function handleGetLeaderboard(env: Env): Promise<Response> {
  const text = await readLeaderboardText(env);
  return textResponse(text);
}

async function handleOverwriteLeaderboard(
  request: Request,
  env: Env
): Promise<Response> {
  if (!checkAuth(request, env)) {
    return jsonResponse({ error: "Unauthorized" }, 401);
  }

  const rateLimit = await checkPostRateLimit(
    request,
    env,
    "overwrite-leaderboard"
  );

  if (!rateLimit.allowed) {
    return jsonResponse(
      {
        error: "Too Many Requests",
        message: "Too many leaderboard overwrite requests. Please try again later.",
      },
      429,
      {
        "Retry-After": String(RATE_LIMIT_WINDOW_SECONDS),
      }
    );
  }

  const bodyText = await request.text();

  // 덮어쓰기 API는 텍스트 전체를 받지만,
  // 저장 전 한 번 파싱/재빌드해서 순위 포맷을 정리합니다.
  const entries = parseLeaderboard(bodyText);
  const normalizedText = buildLeaderboardText(entries);

  await writeLeaderboardText(env, normalizedText);

  return jsonResponse({
    ok: true,
    key: LEADERBOARD_KEY,
    remaining: rateLimit.remaining,
  });
}

async function handlePostRecord(
  request: Request,
  env: Env
): Promise<Response> {
  if (!checkAuth(request, env)) {
    return jsonResponse({ error: "Unauthorized" }, 401);
  }

  const rateLimit = await checkPostRateLimit(request, env, "post-record");

  if (!rateLimit.allowed) {
    return jsonResponse(
      {
        error: "Too Many Requests",
        message: "Too many rank submissions. Please try again later.",
      },
      429,
      {
        "Retry-After": String(RATE_LIMIT_WINDOW_SECONDS),
      }
    );
  }

  let body: unknown;

  try {
    body = await request.json();
  } catch {
    return jsonResponse({ error: "Invalid JSON" }, 400);
  }

  const data = body as Partial<LeaderboardEntry>;

  if (!data.time || typeof data.time !== "string") {
    return jsonResponse({ error: "Invalid time" }, 400);
  }

  if (!isValidTimeFormat(data.time)) {
    return jsonResponse(
      {
        error: "Invalid time format. Expected 00:00:00.000",
      },
      400
    );
  }

  const clearTimeMs = parseTimeToMs(data.time);

  if (
    clearTimeMs === Number.MAX_SAFE_INTEGER ||
    clearTimeMs < MIN_CLEAR_TIME_MS ||
    clearTimeMs > MAX_CLEAR_TIME_MS
  ) {
    return jsonResponse(
      {
        error: "Invalid clear time",
      },
      400
    );
  }

  if (typeof data.level !== "number" || !Number.isFinite(data.level)) {
    return jsonResponse({ error: "Invalid level" }, 400);
  }

  if (data.level < MIN_LEVEL || data.level > MAX_LEVEL) {
    return jsonResponse({ error: "Level out of range" }, 400);
  }

  if (!data.name || typeof data.name !== "string") {
    return jsonResponse({ error: "Invalid name" }, 400);
  }

  const safeName = sanitizeName(data.name);

  if (!safeName) {
    return jsonResponse({ error: "Invalid name" }, 400);
  }

  const oldText = await readLeaderboardText(env);
  const entries = parseLeaderboard(oldText);

  entries.push({
    time: data.time,
    level: Math.floor(data.level),
    name: safeName,
  });

  const newText = buildLeaderboardText(entries);

  await writeLeaderboardText(env, newText);

  return jsonResponse({
    ok: true,
    key: LEADERBOARD_KEY,
    remaining: rateLimit.remaining,
    leaderboard: newText,
  });
}

export default {
  async fetch(request: Request, env: Env): Promise<Response> {
    const url = new URL(request.url);

    if (request.method === "OPTIONS") {
      return jsonResponse({ ok: true });
    }

    if (url.pathname === "/leaderboard" && request.method === "GET") {
      return await handleGetLeaderboard(env);
    }

    // 리더보드 파일 전체 덮어쓰기
    // Body plain text:
    // 1. 00:03:25.142, 12, Knight
    // 2. 00:04:01.530, 10, Mage
    if (url.pathname === "/leaderboard" && request.method === "POST") {
      return await handleOverwriteLeaderboard(request, env);
    }

    // 기록 1개 추가
    // JSON:
    // {
    //   "time": "00:03:25.142",
    //   "level": 12,
    //   "name": "Knight"
    // }
    if (url.pathname === "/record" && request.method === "POST") {
      return await handlePostRecord(request, env);
    }

    return jsonResponse(
      {
        error: "Not Found",
        routes: [
          "GET /leaderboard",
          "POST /leaderboard",
          "POST /record",
        ],
        format: "index. 00:00:00.000, level, name",
        example: "1. 00:03:25.142, 12, Knight",
      },
      404
    );
  },
};