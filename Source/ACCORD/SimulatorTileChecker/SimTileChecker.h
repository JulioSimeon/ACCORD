#pragma once

#include "CoreMinimal.h"
#include "../Tiles/Tile.h"
/**
 * 
 */

class ACCORD_API SimTileChecker
{
public:
	SimTileChecker();
	~SimTileChecker();
    int static CheckForReservation(const TileTimes& tileTimes, const ReservationsMap& reservations);

private:

	
};