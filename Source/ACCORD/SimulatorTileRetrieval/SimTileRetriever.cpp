// Fill out your copyright notice in the Description page of Project Settings.


#include "SimTileRetriever.h"
#include "../Tiles/Intersection.h"
#include "../SimulationConstants.h"
#include "../vehicle/Vehicle.h"
#include "../ReservationTiles/TileRetriever.h"
#include "../vehicle/VehiclePropety.h"



SimTileRetriever::SimTileRetriever(double arrivalTime, double speed, double acceleration, VehicleProperty* vehicleProperty, Intersection* intersection, int direction, int heading, int vehicleID, int timeBase)
{
    CheckDirection(direction);
    CheckHeading(heading);
    m_arrivalTime = arrivalTime;
    m_speed = speed;
    m_acceleration = acceleration;
    m_vehicleProperty = vehicleProperty;
    m_direction = direction;
    m_heading = heading;
    m_vehicleID = vehicleID;
    m_timeBase = timeBase;
    InitializeValues(intersection);
}

void SimTileRetriever::CheckDirection(int direction)
{
    if(!(direction == STRAIGHT || direction == RIGHT_TURN || direction == LEFT_TURN))
    {
        throw "Invalid direction";
    }
}

void SimTileRetriever::CheckHeading(int heading)
{
    if(!(heading == NORTH || heading == SOUTH || heading == WEST || heading == EAST))
    {
        throw "Invalid heading";
    }
}

void SimTileRetriever::InitializeValues(Intersection* intersection)
{
    m_intersection = intersection;
    double length = intersection->GetLength();
    m_startPoints.SetNum(4);
    m_vehicleHeadings.SetNum(4);
    
    m_startPoints[0] = FVector2D(0.75 * length, length + (m_vehicleProperty->GetLength() / 2));
    m_startPoints[1] = FVector2D(-m_vehicleProperty->GetLength() / 2, 0.75 * length);
    m_startPoints[2] = FVector2D(length + (m_vehicleProperty->GetLength() / 2), length / 4);
    m_startPoints[3] = FVector2D(length / 4, -m_vehicleProperty->GetLength() / 2);

    m_vehicleHeadings[0] = FMath::DegreesToRadians(270);
    m_vehicleHeadings[1] = FMath::DegreesToRadians(0);
    m_vehicleHeadings[2] = FMath::DegreesToRadians(180);
    m_vehicleHeadings[3] = FMath::DegreesToRadians(90);
}

TileTimes SimTileRetriever::Simulate()
{
    if(m_direction == LEFT_TURN)
    {
        return SimulateLeftTurn();
    }
    else if(m_direction == RIGHT_TURN)
    {
        return SimulateRightTurn();
    }
    return SimulateStraight();
}

TileTimes SimTileRetriever::SimulateStraight()
{
    Vehicle vehicle(m_vehicleProperty, m_vehicleID);
    vehicle.SetHeading(m_vehicleHeadings[m_heading]);
    vehicle.SetPosition(m_startPoints[m_heading]);
    vehicle.SetSpeed(m_speed);
    vehicle.SetAcceleration(m_acceleration);
    double dt = m_timeBase / FMath::Pow(10.f, 9.f);
    //UE_LOG(LogTemp, Warning, TEXT("Simulate Straight Speed: %f"), vehicle.GetSpeed());
    TileRetriever tileRetriever(m_intersection);
    TileTimes tileTimes;
    double currentTime = m_arrivalTime;
    double targetSpeed = vehicle.GetProperty()->GetMaxSpeed();

    //while(m_intersection->IsInside(vehicle.GetPosition()))
    TArray<FVector2D> Corners = vehicle.GetCorners();
    while(m_intersection->IsInside(Corners[0]) || m_intersection->IsInside(Corners[1]) || m_intersection->IsInside(Corners[2]) || m_intersection->IsInside(Corners[3]))
    {
        TileSet occupiedTiles = tileRetriever.GetOccupiedTiles(&vehicle, m_intersection->GetTileLength() / 47); // sample interval = 940 / 47 = 20
        int CurrentTime = currentTime * 100;
        tileTimes.Add(static_cast<double>(CurrentTime) / 100, occupiedTiles); //tileTimes.Add(currentTime, occupiedTiles);
        // UE_LOG(LogTemp, Warning, TEXT("Time: %f Speed: %f Position: %s"), currentTime, vehicle.GetSpeed(), *vehicle.GetPosition().ToString());
        // for(auto value : occupiedTiles)
        // {
        //     UE_LOG(LogTemp, Warning, TEXT("Tile x:%d y:%d"), value.GetID().X, value.GetID().Y);
        // }
        currentTime += dt;
        if(vehicle.GetSpeed() >= targetSpeed)
        {
            vehicle.SetSpeed(targetSpeed);
            vehicle.SetAcceleration(0);
        }
        vehicle.Move(dt);
        Corners = vehicle.GetCorners();
    }
    return tileTimes;
}

TileTimes SimTileRetriever::SimulateRightTurn()
{
    Vehicle vehicle(m_vehicleProperty, m_vehicleID);
    vehicle.SetHeading(m_vehicleHeadings[m_heading]);
    vehicle.SetPosition(m_startPoints[m_heading]);
    vehicle.SetSteeringAngle(vehicle.GetProperty()->GetMaxSteeringAngle());
    vehicle.SetSpeed(m_speed);
    vehicle.SetAcceleration(m_acceleration);
    double dt = m_timeBase / FMath::Pow(10.f, 9.f);
    // UE_LOG(LogTemp, Warning, TEXT("Simulate Right Turn DT: %f"), dt);
    // UE_LOG(LogTemp, Warning, TEXT("Starting Position: %s"), *vehicle.GetPosition().ToString());
    // UE_LOG(LogTemp, Warning, TEXT("Heading: %d"), m_heading);
    // UE_LOG(LogTemp, Warning, TEXT("Speed: %f"), vehicle.GetSpeed());
    // UE_LOG(LogTemp, Warning, TEXT("Steering Angle: %f"), vehicle.GetSteeringAngle());
    TileRetriever tileRetriever(m_intersection);
    TileTimes tileTimes;
    double currentTime = m_arrivalTime;
    double targetSpeed = vehicle.GetProperty()->GetMaxSpeed();
    double initialHeading = vehicle.GetHeading();
    double finalHeading = fmod((initialHeading + 90), 360);
    
    TArray<FVector2D> Corners = vehicle.GetCorners();
    while(m_intersection->IsInside(Corners[0]) || m_intersection->IsInside(Corners[1]) || m_intersection->IsInside(Corners[2]) || m_intersection->IsInside(Corners[3]))
    //while(m_intersection->IsInside(vehicle.GetPosition()))
    {
        TileSet occupiedTiles = tileRetriever.GetOccupiedTiles(&vehicle, m_intersection->GetTileLength() / 47); // sample interval = 940 / 47 = 20
        int CurrentTime = currentTime * 100;
        tileTimes.Add(static_cast<double>(CurrentTime) / 100, occupiedTiles); //tileTimes.Add(currentTime, occupiedTiles);
        // UE_LOG(LogTemp, Warning, TEXT("Time: %f Speed: %f Position: %s Steering: %f Heading: %f"), currentTime, vehicle.GetSpeed(), *vehicle.GetPosition().ToString(), vehicle.GetSteeringAngle(), vehicle.GetHeading());
        // for(auto value : occupiedTiles)
        // {
        //     UE_LOG(LogTemp, Warning, TEXT("Tile x:%d y:%d"), value.GetID().X, value.GetID().Y);
        // }
        currentTime += dt;
        if(vehicle.GetSpeed() >= targetSpeed)
        {
            vehicle.SetSpeed(targetSpeed);
            vehicle.SetAcceleration(0);
        }
        double relativeHeading = fmod((vehicle.GetHeading() - initialHeading), 360);
        if(relativeHeading >= 90 && vehicle.GetSteeringAngle() != 0)
        {
            vehicle.SetSteeringAngle(0);
            vehicle.SetHeading(finalHeading);
        }
        vehicle.Move(dt);
        Corners = vehicle.GetCorners();
    }
    return tileTimes;
}

TileTimes SimTileRetriever::SimulateLeftTurn()
{
    Vehicle vehicle(m_vehicleProperty, m_vehicleID);
    vehicle.SetHeading(m_vehicleHeadings[m_heading]);
    vehicle.SetPosition(m_startPoints[m_heading]);
    vehicle.SetSpeed(m_speed);
    vehicle.SetAcceleration(m_acceleration);
    double dt = m_timeBase / FMath::Pow(10.f, 9.f);
    TileRetriever tileRetriever(m_intersection);
    TileTimes tileTimes;
    double currentTime = m_arrivalTime;
    double targetSpeed = vehicle.GetProperty()->GetMaxSpeed();
    
    FVector2D startPoint(vehicle.GetPosition().X, vehicle.GetPosition().Y);
    double initialHeading{fmod((2 * PI - m_heading), (2 * PI))};
    double finalHeading{fmod(fmod((2 * PI - initialHeading), (2 * PI)) + 90, 360)};
    double steeringAngle{};

    TArray<FVector2D> Corners = vehicle.GetCorners();
    while(m_intersection->IsInside(Corners[0]) || m_intersection->IsInside(Corners[1]) || m_intersection->IsInside(Corners[2]) || m_intersection->IsInside(Corners[3]))
    {
        TileSet occupiedTiles = tileRetriever.GetOccupiedTiles(&vehicle, m_intersection->GetTileLength() / 47); // sample interval = 940 / 47 = 20
        int CurrentTime = currentTime * 100;
        tileTimes.Add(static_cast<double>(CurrentTime) / 100, occupiedTiles); //tileTimes.Add(currentTime, occupiedTiles);
        // UE_LOG(LogTemp, Warning, TEXT("Time: %f Speed: %f Position: %s"), currentTime, vehicle.GetSpeed(), *vehicle.GetPosition().ToString());
        // for(auto value : occupiedTiles)
        // {
        //     UE_LOG(LogTemp, Warning, TEXT("Tile x:%d y:%d"), value.GetID().X, value.GetID().Y);
        // }
        currentTime += dt;
        if(vehicle.GetSpeed() >= targetSpeed)
        {
            vehicle.SetSpeed(targetSpeed);
            vehicle.SetAcceleration(0);
        }
        FVector2D position = vehicle.GetPosition();
        if(FVector2D::Distance(position, startPoint) >= m_intersection->GetLength() / 3)
        {
            steeringAngle = -vehicle.GetProperty()->GetMaxSteeringAngle();
        }
        double relativeHeading = fmod(fmod(2 * PI - vehicle.GetHeading(), (2 * PI)) - initialHeading, 360);
        if(relativeHeading >= 90)
        {
            steeringAngle = 0;
            vehicle.SetHeading(finalHeading);
        }
        vehicle.SetSteeringAngle(steeringAngle);
        vehicle.Move(dt);
        Corners = vehicle.GetCorners();
    }
    return tileTimes;
}
