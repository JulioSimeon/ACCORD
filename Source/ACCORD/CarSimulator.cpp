// Fill out your copyright notice in the Description page of Project Settings.


#include "CarSimulator.h"
#include "CommMessageScheduler.h"
#include "Track.h"
#include "SimulatedCar.h"
#include "IntersectionSegment.h"
#include <limits.h>

CarSimulator::CarSimulator()
{
}

CarSimulator::~CarSimulator()
{
}

void CarSimulator::SetScheduler(CommMessageScheduler* commSched)
{
    m_commSched = commSched;
}

void CarSimulator::SetTrack(Track* track)
{
    m_track = track;
}

void CarSimulator::AddCar(SimulatedCar* car, Track* route)
{
    AddCar(car);
    m_routeTracks.Add(route);
}

void CarSimulator::AddCar(SimulatedCar* car)
{
    m_carList.Add(car);
    if(!car->UpdateLocation())
    {
        UE_LOG(LogTemp, Display, TEXT("CarSimulator: Car 0x %s failed to initial update"), *(FString::FromInt(car->getID())));
    }
}

void CarSimulator::Simulate()
{
    if(!m_start)
    {
        Start();
    }
    Track* subtrack;
    if(m_track != nullptr && m_carList.Num() > 0)
    {
        for(int i{}; i < m_carList.Num(); i++)
        {
            SimulatedCar* car {m_carList[i]};
            if(i >= m_routeTracks.Num())
            {
                subtrack = m_track;
            }
            else
            {
                subtrack = m_routeTracks[i];
            }
            if(car->UpdateLocation())
            {
                CarTracker carTracker = subtrack->UpdateCarTracker(car);

                if(carTracker.nextSeg && carTracker.nextSeg->IsIntersection())
                {
                    IntersectionSegment* intersection = static_cast<IntersectionSegment*>(carTracker.nextSeg);
                    if(intersection->IsReserved(car))
                    {

                    }
                    else
                    {
                        if(car->GetSpeed() > 0)
                        {
                            if(intersection->Reserve(car, carTracker.currentSeg, car->GetNextRouteDirection()))
                            {

                            }
                            else
                            {
                                car->throttleDecrement();
                            }
                        }
                        else
                        {
                            car->AdjustThrottle(SimulatedCar::SPEED_FLOOR);
                            if(!intersection->Reserve(car, carTracker.currentSeg, car->GetNextRouteDirection()))
                            {
                                car->AdjustThrottle(0);
                            }
                        }
                        carTracker = m_track->UpdateCarTracker(car);
                    }
                }
                if(carTracker.currentSeg && carTracker.currentSeg->IsIntersection())
                {
                    CarTracker tempCarTracker(car);
                    tempCarTracker.x = carTracker.x;
                    tempCarTracker.y = carTracker.y;
                    tempCarTracker.angleDeviation = carTracker.angleDeviation;
                    tempCarTracker.distanceFromDrivingLine = carTracker.distanceFromDrivingLine;
                    tempCarTracker.hasReservation = carTracker.hasReservation;
                    tempCarTracker.idealAngle = carTracker.idealAngle;
                    tempCarTracker.isOutOfBounds = carTracker.isOutOfBounds;
                    tempCarTracker.nextSeg = carTracker.nextSeg;
                    tempCarTracker.currentSeg = static_cast<IntersectionSegment*>(carTracker.currentSeg)->GetCurrentInternalSegment(car);
                    if(!tempCarTracker.currentSeg)
                    {
                        tempCarTracker.isOutOfBounds = true;
                    }
                    carTracker = tempCarTracker;
                }
                car->SetOutOfBounds(carTracker.isOutOfBounds);
                DoSteering(car, &carTracker);
                DoThrottling(car, &carTracker);
            }
        }
    }
}

void CarSimulator::Start()
{
    m_startTime = FPlatformTime::Seconds();
    m_start = true;
}

void CarSimulator::DoSteering(SimulatedCar* car, CarTracker* tracker)
{
    if(!tracker->isOutOfBounds)
    {
        if(tracker->currentSeg->GetSegmentShape() == SEGSHAPE::LINEAR)
        {
            car->MaintainOrientation(ComputeNextOrientation(car, tracker), false);
            TArray<double> orientationTimed = ComputeSteerCompensateTime(car);
            car->MaintainOrientationTimed(orientationTimed[0], orientationTimed[1]);
        }
        else if(tracker->currentSeg->GetSegmentShape() == SEGSHAPE::NINETYDEGTURN)
        {
            double exitDirection{tracker->currentSeg->GetIdealDirection(tracker->currentSeg->GetExitXLocation(), tracker->currentSeg->GetExitYLocation())};
            double deviation{FMath::Abs(car->GetOrientation() - exitDirection)};
            if(deviation > 180)
            {
                deviation = -(360 - deviation);
            }
            if(FMath::Abs(deviation) > minAngleToCorrect)
            {
                car->MaintainOrientation(exitDirection, true);
            }
            else
            {
                TArray<double> orientationTimed = ComputeSteerCompensateTime(car);
                car->MaintainOrientationTimed(orientationTimed[0], orientationTimed[1]);
            }
        }
    }
}

void CarSimulator::DoThrottling(SimulatedCar* car, CarTracker* tracker)
{
    if(tracker->isOutOfBounds)
    {
        car->throttleDecrement();
    }
    else if(tracker->currentSeg && tracker->currentSeg->IsIntersection())
    {
        car->AdjustThrottle(car->GetThrottlePower());
    }
    else if(tracker->hasReservation)
    {
        car->AdjustThrottle(car->GetThrottlePower());
    }
    else if(tracker->nextSeg && tracker->nextSeg->IsIntersection())
    {
        if(tracker->hasReservation)
        {
            car->AdjustThrottle(car->GetThrottlePower());
        }
        else
        {

        }
    }
    else
    {
        car->throttleIncrement();
    }

}

double CarSimulator::ComputeNextOrientation(SimulatedCar* car, CarTracker* tracker)
{
    Track* subtrack = GetCarRoute(car);
    int distCLine = tracker->distanceFromDrivingLine;
    double followOrient = tracker->idealAngle;
    if(distCLine == INT_MAX)
    {
        car->SetOutOfBounds(true);
        return car->GetOrientation();
    }
    else
    {
        car->SetOutOfBounds(false);
    }
    return followOrient;
}

Track* CarSimulator::GetCarRoute(SimulatedCar* car)
{
    int carIndex = GetCarListIndex(car);
    if(carIndex < 0)
    {
        return nullptr;
    }
    if(carIndex >= m_routeTracks.Num())
    {
        return m_track;
    }
    return m_routeTracks[carIndex];
}

int CarSimulator::GetCarListIndex(SimulatedCar* car)
{
    for(int i{}; i < m_carList.Num(); i++)
    {
        if(m_carList[i] == car)
        {
            return i;
        }
    }
    return -1;
}

TArray<double> CarSimulator::ComputeSteerCompensateTime(SimulatedCar* car)
{
    Track* subtrack = GetCarRoute(car);
    int distCLine = subtrack->GetDistanceFromCenterLine(car);
    double followOrient {subtrack->GetIdealDirection(car->GetXLocation(), car->GetYLocation())};
    return ComputeSteerCompensateTime(distCLine, followOrient, car->GetSpeed());
}

TArray<double> CarSimulator::ComputeSteerCompensateTime(int distCLine, double followOrient, double speed)
{
    TArray<double> data;
    data.SetNum(2);
    double time{};
    if(speed != 0)
    {
        time = (distCLine * 1000) / speed;
    }
    if(FMath::Abs(distCLine) > minDistanceToCorrect)
    {
        if(distCLine > 0)
        {
            followOrient += orientCorrection;
        }
        else
        {
            followOrient -= orientCorrection;
        }
    }
    data[0] = followOrient;
    data[1] = time;
    return data;
}
