// Fill out your copyright notice in the Description page of Project Settings.


#include "SimTileChecker.h"

SimTileChecker::SimTileChecker()
{
}

SimTileChecker::~SimTileChecker()
{
}

int SimTileChecker::CheckForReservation(TileTimes& tileTimes, const ReservationsMap& reservations)
{
    for(auto reservationEntry : reservations)
    {
        TileTimes reservedTileTimes = reservationEntry.Value;
        for(auto tileTimesEntry : tileTimes)
        {
            double time = tileTimesEntry.Key;
            if(reservedTileTimes.Contains(time))
            {
                TileSet reservedTiles = tileTimes[time];
                TileSet resultTiles = reservedTiles.Intersect(reservedTileTimes[time]);
                if(!resultTiles.IsEmpty())
                {
                    return reservationEntry.Key;
                }
            }
        }
    }
    return -1;
}
