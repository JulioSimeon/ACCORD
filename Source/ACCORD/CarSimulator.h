#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ACCORD_API CarSimulator
{
public:
	CarSimulator();
	~CarSimulator();

    void SetScheduler(class CommMessageScheduler* commSched);
    void SetTrack(class Track* track);
    void AddCar(class SimulatedCar* car, Track* route);
    void AddCar(SimulatedCar* car);
    void Simulate();
    void Start();

private:
    CommMessageScheduler* m_commSched;
    Track* m_track;
    TArray<SimulatedCar*> m_carList;
    TArray<Track*> m_routeTracks;
    bool m_start{false};
    double m_startTime{};
    void DoSteering(SimulatedCar* car, class CarTracker* tracker);
    void DoThrottling(SimulatedCar* car, CarTracker* tracker);
    double ComputeNextOrientation(SimulatedCar* car, CarTracker* tracker);
    Track* GetCarRoute(SimulatedCar* car);
    int GetCarListIndex(SimulatedCar* car);
    TArray<double> ComputeSteerCompensateTime(SimulatedCar* car);
    TArray<double> ComputeSteerCompensateTime(int distCLine, double followOrient, double speed);
    int minDistanceToCorrect{100};
    int minAngleToCorrect{10};
    double orientCorrection{15};
};