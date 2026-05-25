#pragma once
#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/Coordinate.h"

class Player;
class MapManager : public Singleton<MapManager>
{
public:
	MapManager();
	~MapManager();

public:
	void BeginPlay();
	void Tick(float DeltaTime);

	void UpdateMap();
	vector<vector<Coordinate>>& GetMap() { return Map; }

private:
	vector<vector<Coordinate>> Map;
	//vector<vector<int>> MapOrigin;
};
