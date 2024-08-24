// Fill out your copyright notice in the Description page of Project Settings.


#include "ReservationManager.h"
#include "../Tiles/Intersection.h"
#include "../SimulatorTileRetrieval/SimTileRetriever.h"
#include "../vehicle/VehiclePropety.h"
#include "../SimulatorTileChecker/SimTileChecker.h"

ReservationManager::ReservationManager()
{
}

ReservationManager::~ReservationManager()
{
}

bool ReservationManager::Remove(int vehicleID)
{
    if(m_reservations.Contains(vehicleID))
    {
        m_reservations.Remove(vehicleID);
        return true;
    }
    return false;
}

bool ReservationManager::Reserve(Intersection* intersection, double arrivalTime, VehicleProperty* vechicleProperty, double speed, double acceleration, int heading, int direction, int vehicleID, int timeBase)
{
    SimTileRetriever tileRetriever(arrivalTime, speed, acceleration, vechicleProperty, intersection, direction, heading, vehicleID, timeBase);
    TileTimes path = tileRetriever.Simulate();
    double LastTime{};
    for(auto entry : path)
    {
        if(entry.Key > LastTime)
        {
            LastTime = entry.Key;
        }
        // UE_LOG(LogTemp, Warning, TEXT("Car: %d Time: %f Speed: %f"), vehicleID, entry.Key, speed);
        // for(auto value : entry.Value)
        // {
        //     UE_LOG(LogTemp, Warning, TEXT("Tile x:%d y:%d"), value.GetID().X, value.GetID().Y);
        // }
    }
    UE_LOG(LogTemp, Warning, TEXT("Car %d projected to exit intersection at time: %f"), vehicleID, LastTime);
    bool notReserved{SimTileChecker::CheckForReservation(path, m_reservations) == -1};
    if(notReserved)
    {
        m_reservations.Add(vehicleID, path);
    }
    return notReserved;
}

