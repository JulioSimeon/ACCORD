// Fill out your copyright notice in the Description page of Project Settings.


#include "Track.h"
#include "TrackSegment.h"
#include "IntersectionSegment.h"
#include "SimulatedCar.h"
#include <limits.h>

Track::Track()
{
    
}

Track::Track(int laneWidth, int startUpLength, int straightWayLength, int margin)
{
    TArray<TrackSegment> startUps;
    TArray<TrackSegment> straights;
    TArray<TrackSegment> exits;

    startUps.SetNum(4);
    straights.SetNum(4);
    exits.SetNum(4);

    IntersectionSegment intersection(laneWidth * 2);
    intersection.SetSegmentID("Intersection");

    double direction{};
    for(int i = 0; i < 4; i++)
    {
        startUps[i].CreateLineSegment(startUpLength, laneWidth, direction);
        startUps[i].SetSegmentID("StartUp " + FString::SanitizeFloat(direction));

        straights[i].CreateLineSegment(straightWayLength, laneWidth, direction);
        straights[i].SetSegmentID("Straight " + FString::SanitizeFloat(direction));

        exits[i].CreateLineSegment(startUpLength + straightWayLength, laneWidth, direction);
        exits[i].SetSegmentID("Exit " + FString::SanitizeFloat(direction));

        direction += 90;
    }

    startUps[0].SetAbsoluteLocation(0, startUpLength + straightWayLength + (laneWidth/2));
    straights[0].SetAbsoluteLocation(startUpLength, startUpLength + straightWayLength + (laneWidth/2));
    exits[0].SetAbsoluteLocation(startUpLength + straightWayLength + laneWidth * 2, startUpLength + straightWayLength + (laneWidth/2));

    startUps[1].SetAbsoluteLocation(startUpLength + straightWayLength + static_cast<int>(laneWidth * 1.5), 0);
    straights[1].SetAbsoluteLocation(startUpLength + straightWayLength + static_cast<int>(laneWidth * 1.5), startUpLength);
    exits[1].SetAbsoluteLocation(startUpLength + straightWayLength + static_cast<int>(laneWidth * 1.5), startUpLength + straightWayLength + laneWidth * 2);

    startUps[2].SetAbsoluteLocation(startUpLength * 2 + straightWayLength * 2 + laneWidth * 2, startUpLength + straightWayLength + static_cast<int>(laneWidth * 1.5));
    straights[2].SetAbsoluteLocation(startUpLength + straightWayLength * 2 + laneWidth * 2, startUpLength + straightWayLength + static_cast<int>(laneWidth * 1.5));
    exits[2].SetAbsoluteLocation(startUpLength + straightWayLength, startUpLength + straightWayLength + static_cast<int>(laneWidth * 1.5));

    startUps[3].SetAbsoluteLocation(startUpLength + straightWayLength + laneWidth/2, startUpLength * 2 + straightWayLength * 2 + laneWidth * 2);
    straights[3].SetAbsoluteLocation(startUpLength + straightWayLength + laneWidth/2, startUpLength + straightWayLength * 2 + laneWidth * 2);
    exits[3].SetAbsoluteLocation(startUpLength + straightWayLength + laneWidth/2, startUpLength + straightWayLength);

    intersection.SetAbsoluteLocation(startUpLength + straightWayLength, startUpLength + straightWayLength);

    // for(int i = 0; i < 4; i++)
    // {
    //     startUps[i].ConnectSegments(nullptr, &straights[i]);
    //     straights[i].ConnectSegments(&startUps[i], &intersection);
    //     exits[i].ConnectSegments(&intersection, nullptr);
    // }

    intersection.ConnectEntranceExits(straights, exits);

    // m_segments.Append(startUps);
    // m_segments.Append(straights);
    // m_segments.Append(exits);
    for(int i{}; i < startUps.Num(); i++)
    {
        m_segments.Add(startUps[i]);
        m_segments.Add(straights[i]);
        m_segments.Add(exits[i]);
        
    }
    m_segments.Emplace(intersection);
    for (int i{}; i < m_segments.Num() - 3; i += 3)
    {
        m_segments[i].ConnectSegments(nullptr, &m_segments[i + 1]);
        m_segments[i + 1].ConnectSegments(&m_segments[i], &m_segments[m_segments.Num() - 1]);
        m_segments[i + 2].ConnectSegments(&m_segments[m_segments.Num() - 1], nullptr);
    }
    SetTrackMargin(margin);
}

Track::~Track()
{
}

void Track::PrintAllSegments()
{
    for(auto& segment : m_segments)
    {
        UE_LOG(LogTemp, Display, TEXT("Segment ID: %s \t XLoc: %d \tYLoc: %d"), *segment.GetSegmentID(), segment.GetXLocation(), segment.GetYLocation());
        if(segment.GetNextSegment() != nullptr)
        {
            FString temp = segment.GetNextSegment()->GetSegmentID();
            UE_LOG(LogTemp, Display, TEXT("Next Segment ID: %s \t XLoc: %d \tYLoc: %d"), *temp, segment.GetNextSegment()->GetXLocation(), segment.GetNextSegment()->GetYLocation());
        }
    }
}

void Track::PrintAllSegmentsPointers()
{
    for(TrackSegment* segment : m_segmentpointers)
    {
        UE_LOG(LogTemp, Display, TEXT("Segment ID:  \t XLoc: %d \tYLoc: %d"), segment->GetXLocation(), segment->GetYLocation());
    }
}

Track Track::GetRouteTrack(SimulatedCar* car, TrackSegment* start)
{
    TrackSegment* current = start;
    TrackSegment* next = nullptr;
    TrackSegment* prev = nullptr;
    Track routeTrack;
    if(!car || !start)
    {
        return routeTrack;
    }
    do
    {
        routeTrack.AddTrackSegment(current, false);
        //UE_LOG(LogTemp, Warning, TEXT("I'm a %s"), *current->GetSegmentID());
        // if(current && current->IsIntersection())
        // {
        //     UE_LOG(LogTemp, Display, TEXT("I'm an intersection"));
        //     if(IntersectionSegment* intersection = dynamic_cast<IntersectionSegment*>(current))
        //     {
        //         next = intersection->GetExitTo(prev, car->GetNextRouteDirection());
        //     }
        // }
        // else
        // {
        //     next = current->GetNextSegment();
        // // }
        // prev = current;
        // current = next;
        TrackSegment temp = *current;
        UE_LOG(LogTemp, Display, TEXT("I was called1 %s"), *temp.GetNextSegment()->GetSegmentID());
        
        current = nullptr;
    }
    while(current != nullptr);
    
    return routeTrack;
}

void Track::AddTrackSegment(TrackSegment* segment, bool autoConnect)
{
    if(autoConnect)
    {
        m_segmentpointers[m_segmentpointers.Num() - 1]->ConnectNextSegment(segment);
    }
    m_segments.Add(*segment);
    UE_LOG(LogTemp, Display, TEXT("I was called2"));
    //m_segmentpointers.Add(segment);
}

bool Track::UpdateCarTrackerAngles(CarTracker& carTracker)
{
    if(!carTracker.currentSeg)
    {
        return false;
    }
    carTracker.idealAngle = carTracker.currentSeg->GetIdealDirection(carTracker.trackedCar);
    carTracker.angleDeviation = FMath::Abs(carTracker.trackedCar->GetOrientation() - carTracker.idealAngle);
    if(carTracker.angleDeviation > 180)
    {
        carTracker.angleDeviation = carTracker.angleDeviation - 360;
    }
    return true;
}

void Track::SetTrackMargin(int margin)
{
    for(auto& segment : m_segments)
    {
        segment.SetBoundaryMargin(margin);
    }
}

CarTracker Track::UpdateCarTracker(SimulatedCar* car)
{
    CarTracker tempCarTracker;
    for(const auto& tracker : m_trackers)
    {
        if(tracker.trackedCar == car)
        {
            tempCarTracker = tracker;
        }
    }

    if(!tempCarTracker.trackedCar)
    {
        tempCarTracker.trackedCar = car;
        m_trackers.Emplace(tempCarTracker);
    }

    tempCarTracker.x = car->GetXLocation();
    tempCarTracker.y = car->GetYLocation();
    tempCarTracker.currentSeg = SearchCurrentSegment(car);
    if(tempCarTracker.currentSeg)
    {   
        UpdateCarTrackerAngles(tempCarTracker);
        tempCarTracker.nextSeg = tempCarTracker.currentSeg->GetNextSegment();
    }
    else
    {
        tempCarTracker.angleDeviation = 0;
        tempCarTracker.distanceFromDrivingLine = 0;
        tempCarTracker.idealAngle = car->GetOrientation();
        tempCarTracker.hasReservation = false;
    }

    if(tempCarTracker.nextSeg && tempCarTracker.nextSeg->IsIntersection())
    {
        
        if(IntersectionSegment* intersection = static_cast<IntersectionSegment*>(tempCarTracker.nextSeg))
        {
            tempCarTracker.hasReservation = intersection->IsReserved(car);
        }
    }
    else if(tempCarTracker.currentSeg && tempCarTracker.currentSeg->IsIntersection())
    {
        if(IntersectionSegment* intersection = static_cast<IntersectionSegment*>(tempCarTracker.currentSeg))
        {
            tempCarTracker.hasReservation = intersection->IsReserved(car);
        }
    }
    else
    {
        tempCarTracker.hasReservation = false;
    }

    tempCarTracker.isOutOfBounds = (!tempCarTracker.currentSeg);
    tempCarTracker.trackedCar->SetOutOfBounds(tempCarTracker.isOutOfBounds);

    if(tempCarTracker.currentSeg && tempCarTracker.currentSeg->GetPreviousSegment() && tempCarTracker.currentSeg->GetPreviousSegment()->IsIntersection())
    {
        if(IntersectionSegment* intersection = static_cast<IntersectionSegment*>(tempCarTracker.currentSeg->GetPreviousSegment()))
        {
            tempCarTracker.hasReservation = intersection->ReleaseReservation(car);
        }
    }
    
    return tempCarTracker;
}

int Track::GetDistanceFromCenterLine(SimulatedCar* car)
{
    int xLoc = car->GetXLocation();
    int yLoc = car->GetYLocation();
    TrackSegment* segment = GetCurrentSegment(xLoc, yLoc);
    if(segment)
    {
        return segment->GetDistanceFromCenterLine(xLoc, yLoc);
    }
    else
    {
        return INT_MAX;
    }

}

TrackSegment* Track::GetCurrentSegment(int xLoc, int yLoc)
{
    for(auto& segment : m_segments)
    {
        if(segment.IsWithInBounds(xLoc, yLoc))
        {
            return &segment;
        }
    }
    return nullptr;
}

double Track::GetIdealDirection(int xLoc, int yLoc)
{
    TrackSegment* segment = GetCurrentSegment(xLoc, yLoc);
    if(segment)
    {
        return segment->GetIdealDirection(xLoc, yLoc);
    }
    else
    {
        return INT_MAX;
    }
}

TrackSegment* Track::SearchCurrentSegment(SimulatedCar* car)
{
    for(auto& segment : m_segments)
    {
        if(segment.IsWithInBounds(car))
        {
            return &segment;
        }
    }
    return nullptr;
}

