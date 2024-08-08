// Fill out your copyright notice in the Description page of Project Settings.


#include "TileRetriever.h"
#include "../vehicle/Vehicle.h"
#include "../vehicle/VehicleBounds.h"
#include "../Geometry/GridLine.h"
#include "../Tiles/Intersection.h"

TileRetriever::TileRetriever(class Intersection* intersection)
{
    m_intersection = intersection;
}

TileSet TileRetriever::GetOccupiedTiles(Vehicle* vehicle, double sampleInterval)
{
    TileSet occupiedTiles;
    VehicleBounds bounds = vehicle->GetBounds();
    TMap<int, TileSet> tileMap;
    for(auto& line : bounds.GetBounds())
    {
        TSet<FVector2D> pointSet;
        //addFromLine
        if(line.m_x1 == line.m_x2)
        {
            double yStart = FMath::Min(line.m_y1, line.m_y2);
            double yEnd = FMath::Max(line.m_y1, line.m_y2);
            for(double y{yStart}; true; y = FMath::Min(y + sampleInterval, yEnd))
            {
                pointSet.Add(FVector2D(line.m_x1, y));
                if(y >= yEnd)
                {
                    break;
                }
            }
        }
        else
        {
            double xStart = FMath::Min(line.m_x1, line.m_x2);
            double xEnd = FMath::Max(line.m_x1, line.m_x2);
            double m = line.GetSlope();
            double xInterval = sampleInterval / FMath::Sqrt(FMath::Square(m) + 1);
            for(double x{xStart}; true; x = FMath::Min(x + xInterval, xEnd))
            {
                double y = line.m_y1 + m * (x - line.m_x1);
                pointSet.Add(FVector2D(x, y));
                if( x >= xEnd)
                {
                    break;
                }
            }
        }
        // addFromLine end
        for(FVector2D point : pointSet)
        {   
            if(!IsInBounds(point))
            {
                continue;
            }
            int remX = static_cast<int>(point.X) % static_cast<int>(m_intersection->GetLength());
            int remY = static_cast<int>(point.Y) % static_cast<int>(m_intersection->GetLength());
            if(remX == 0 || remY == 0)
            {
                continue;
            }
            int tileX = static_cast<int>(point.X / m_intersection->GetLength());
            int tileY = static_cast<int>(point.Y / m_intersection->GetLength());
            FIntPoint tileID(tileX, tileY);
            Tile tile(tileID, GetTileType(tileID));
            if(tileMap.Contains(tileX))
            {
                TileSet set = tileMap[tileX];
                set.Add(tile);
                tileMap[tileX] = set;
            }
            else
            {
                TileSet set;
                set.Add(tile);
                tileMap.Add(tileX, set);
            }
        }
    }
    TArray<int> keys;
    tileMap.GetKeys(keys);
    for(int key : keys)
    {
        TileSet set = tileMap[key];
        Tile* minTile = GetMinTile(set);
        Tile* maxTile = GetMaxTile(set);
        TileSet newSet;
        for(int i{minTile->GetID().Y}; i <= maxTile->GetID().Y; i++)
        {
            FIntPoint ID(key, i);
            Tile newTile(ID, GetTileType(ID));
            newSet.Add(newTile);
        }
        tileMap.Add(key, newSet);   
    }
    TArray<TileSet> values;
    tileMap.GenerateValueArray(values);
    for(TileSet set : values)
    {
        occupiedTiles.Append(set);
    }
    return occupiedTiles;

}

void TileRetriever::addFromLine(GridLine* line, double interval)
{
    
}

bool TileRetriever::IsInBounds(FVector2D pt) const
{
    return pt.X >= 0 && pt.X <= m_intersection->GetLength() && pt.Y >= 0 && pt.Y <= m_intersection->GetLength();
}

int TileRetriever::GetTileType(FIntPoint tileID) const
{
    return (tileID.X == 0 || tileID.X == m_intersection->GetResolution() - 1 || tileID.Y == 0 || tileID.Y == m_intersection->GetResolution() - 1) ? Tile::EDGE_TILE : Tile::INTERNAL_TILE;
}

Tile* TileRetriever::GetMinTile(TileSet& set)
{
    Tile* minTile = nullptr;
    if(!set.IsEmpty())
    {
        for(Tile& tile : set)
        {
            if(minTile)
            {
                if(tile.GetID().Y >= minTile->GetID().Y)
                {
                    continue;
                }
                minTile = &tile;
            }
        }
    }
    return minTile;
}

Tile* TileRetriever::GetMaxTile(TileSet& set)
{
    Tile* maxTile = nullptr;
    if(!set.IsEmpty())
    {
        for(Tile& tile : set)
        {
            if(maxTile)
            {
                if(tile.GetID().Y <= maxTile->GetID().Y)
                {
                    continue;
                }
                maxTile = &tile;
            }
        }
    }
    return maxTile;
}
