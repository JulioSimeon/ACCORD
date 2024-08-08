#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ACCORD_API CarTracker
{
public:
	CarTracker();
	~CarTracker();

    class SimulatedCar* trackedCar = nullptr;
    int x{};
    int y{};
    class TrackSegment* currentSeg = nullptr;
    TrackSegment* nextSeg = nullptr;
    bool isOutOfBounds = true;
    bool hasReservation = false;
    int distanceFromDrivingLine{};
    double angleDeviation{};
    double idealAngle{};
    
    CarTracker(SimulatedCar* car);

private:

};




