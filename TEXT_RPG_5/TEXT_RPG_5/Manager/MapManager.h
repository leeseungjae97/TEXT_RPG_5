#pragma once
#include "../Singleton.h"
#include "../pch.h"
#include "../Struct/Coordinate.h"

struct Vector;
class Player;
class AObject;

class MapManager : public Singleton<MapManager>
{
public:
	MapManager();
	~MapManager();

public:
	void BeginPlay();
	void Tick(float DeltaTime);

private:
	void UpdateMap();
	
public:
	bool IsMapInitSize();
	bool IsTypeExist(int Y, int X, MapObjectType Type);
	bool IsTypeExist(Vector Pos, MapObjectType Type);
	int GetID(int Y, int X);
	int GetID(Vector Pos);
	void SetMapObjectCoordinate(int Y, int X, Coordinate InCoordinate);
	void SetMapObjectType(int Y, int X, MapObjectType Type);
	void SetMapObjectType(Vector Pos, MapObjectType Type);
	void SetMapObjectID(int Y, int X, int InID);
	MapObjectType GetType(Vector Pos);
	MapObjectType GetType(int Y, int X);
	AObject* GetMapObject(Vector Pos, MapObjectType Type);
	AObject* GetMapObject(int Y, int X, MapObjectType Type);

private:
	vector<vector<Coordinate>> Map;
};
