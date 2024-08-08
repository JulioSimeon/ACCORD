#pragma once

#include "CoreMinimal.h"
#include "../Tiles/Tile.h"

/**
 * 
 */
class ACCORD_API TileRetriever
{
public:
	TileRetriever() = default;
	~TileRetriever() = default;
	TileRetriever(class Intersection* intersection);
	TileSet GetOccupiedTiles(class Vehicle* vehicle, double sampleInterval);
	

private:
	Intersection* m_intersection;
	void addFromLine(class GridLine* line, double interval);
	bool IsInBounds(FVector2D pt) const;
	int GetTileType(FIntPoint tileID) const;
	Tile* GetMinTile(TileSet& set);
	Tile* GetMaxTile(TileSet& set);
};