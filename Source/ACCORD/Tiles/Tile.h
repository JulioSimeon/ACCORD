#pragma once

#include "CoreMinimal.h"
/**
 * 
 */

class ACCORD_API Tile
{
public:
	Tile();
	~Tile();
	Tile(FIntPoint ID, int type);

	friend bool operator== (const Tile& T1, const Tile& T2)
	{
    	return T1.m_id == T2.m_id;
	}

	FIntPoint GetID() const { return m_id; }

	static const int EDGE_TILE{-2};
	static const int INTERNAL_TILE{-3};
	static const int UNRESERVED_STATUS{-1};
    
	

private:
	FIntPoint m_id;
	int m_type{};
	int m_status{};
	

	
};

FORCEINLINE uint32 GetTypeHash(const Tile& tile)
{
	return FCrc::MemCrc32(&tile, sizeof(Tile));
	//return HashCombine(GetTypeHash(tile.m_type), tile.m_id)
}

using TileSet = TSet<Tile>;
using TileTimes = TMap<double, TileSet>;
using ReservationsMap = TMap<int, TileTimes>;