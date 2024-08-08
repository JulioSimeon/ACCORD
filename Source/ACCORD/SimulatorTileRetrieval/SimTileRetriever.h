#pragma once

#include "CoreMinimal.h"
#include "../Tiles/Tile.h"
/**
 * 
 */
class ACCORD_API SimTileRetriever
{
public:
	SimTileRetriever() = default;
	~SimTileRetriever() = default;
    SimTileRetriever(double arrivalTime, double speed, double acceleration, class VehicleProperty* vehicleProperty, class Intersection* intersection, int direction, int heading, int vehicleID, int timeBase);
    TileTimes Simulate();

private:
    void CheckDirection(int direction);
    void CheckHeading(int heading);
    double m_arrivalTime{};
    double m_speed{};
    double m_acceleration{};
    VehicleProperty* m_vehicleProperty = nullptr;
    Intersection* m_intersection = nullptr;
    int m_direction{};
    int m_heading{};
    int m_vehicleID{};
    int m_timeBase{};
    void InitializeValues(Intersection* intersection);
    TArray<FVector2D> m_startPoints;
    TArray<double> m_vehicleHeadings;
    TileTimes SimulateStraight();
    TileTimes SimulateRightTurn();
    TileTimes SimulateLeftTurn();

};