// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrackSegment.h"
#include "IntersectionSlot.h"
#include "ReservationManager/ReservationManager.h"
#include "Tiles/Intersection.h"

/**
 * 
 */
class ACCORD_API IntersectionSegment : public TrackSegment
{
public:
	IntersectionSegment();

    IntersectionSegment(int size_mm);

	~IntersectionSegment();

    void ConnectEntranceExits(TArray<TrackSegment> entrances, TArray<TrackSegment> exits);

    TrackSegment* GetExitTo(TrackSegment* enterFrom, int direction);

    bool IsReserved(class SimulatedCar* car);   
    bool ReleaseReservation(SimulatedCar* car);
    bool Reserve(SimulatedCar* car, TrackSegment* entrance, int turn);
    TrackSegment* GetCurrentInternalSegment(SimulatedCar* car);

private:
    int m_DimensionSize{};
    TArray<TrackSegment> IntersectSegs[4][3];

    TArray<TrackSegment> m_entrances;
    TArray<TrackSegment> m_exits;

    TArray<IntersectionSlot> m_slots;

    int GetHeadingSegIndex(int heading);
    int GetEntranceHeading(TrackSegment* enterFrom);

    IntersectionSlot* FindSlot(SimulatedCar* car);
    ReservationManager resMan;
    double m_start = FPlatformTime::Seconds();
    double GetTimeToEntrance(SimulatedCar* car, TrackSegment* entrance);
    void SetInternalSegmentLocations(int x, int y);
    
    int m_absoluteXLoc{};
    int m_absoluteYLoc{};
    Intersection intersection;
    int m_resolution{4};
    
    bool CheckFront(SimulatedCar* car);
    void AddSlot(SimulatedCar* car, int heading, int direction);
};
