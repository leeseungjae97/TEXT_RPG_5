#pragma once
#include "Singleton.h"
#include "pch.h"

class MapManager : public Singleton<MapManager>
{
public:
	MapManager();
	~MapManager();

public:
	void Init();
	void Tick(float DeltaTime);

	void UpdateMap();
	vector<vector<int>>& GetMap() { return Map; }

private:
	vector<vector<int>> Map;
	//vector<vector<int>> MapOrigin;
};
