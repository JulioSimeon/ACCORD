#pragma once

#include "CoreMinimal.h"
#include "../Tiles/Tile.h"
/**
 * 
 */
class ACCORD_API ReservationManager
{
public:
	ReservationManager();
	~ReservationManager();

    bool Remove(int vehicleID);
    bool Reserve(class Intersection* intersection, double arrivalTime, class VehicleProperty* vechicleProperty, double speed, double acceleration, int heading, int direction, int vehicleID, int timeBase);

private:
    ReservationsMap m_reservations;
};