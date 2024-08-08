#pragma once

#include "CoreMinimal.h"
#include "TrackSegment.h"
/**
 * 
 */
class ACCORD_API IntersectionSlot
{
public:
	IntersectionSlot();
	~IntersectionSlot();
    
    class SimulatedCar* car = nullptr;
    int heading{};
    int direction{};
    TrackSegment* enterFrom = nullptr;
    TrackSegment* exitTo = nullptr;
    TArray<TrackSegment*> route;
    bool isReserved = false;

private:
    
};




