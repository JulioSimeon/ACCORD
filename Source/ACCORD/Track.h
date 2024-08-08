// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TrackSegment.h"
#include "CarTracker.h"

/**
 * 
 */

class ACCORD_API Track
{

public:
	Track();
	Track(int laneWidth, int startUpLength, int straightWayLength, int margin);
	~Track();

	void PrintAllSegments();
	void PrintAllSegmentsPointers();
	Track GetRouteTrack(class SimulatedCar* car, TrackSegment* start);
	void AddTrackSegment(TrackSegment* segment, bool autoConnect);
	bool UpdateCarTrackerAngles(CarTracker& carTracker);
	CarTracker UpdateCarTracker(SimulatedCar* car);
	int GetDistanceFromCenterLine(SimulatedCar* car);
	TrackSegment* GetCurrentSegment(int xLoc, int yLoc);
	double GetIdealDirection(int xLoc, int yLoc);

	
private:
	TArray<TrackSegment> m_segments;
	TArray<TrackSegment*> m_segmentpointers;
	TArray<CarTracker> m_trackers;

	void SetTrackMargin(int margin);
	TrackSegment* SearchCurrentSegment(SimulatedCar* car);
};
