// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle.h"
#include "VehiclePropety.h"
#include "Math/UnrealMathUtility.h"

Vehicle::Vehicle()
{
}

Vehicle::~Vehicle()
{
}

Vehicle::Vehicle(VehicleProperty* property, int ID)
{
    m_property = property;
    m_ID = ID;
}

void Vehicle::SetHeading(double heading)
{
    m_heading = heading;
}

void Vehicle::SetPosition(FVector2D position)
{
    m_position = position;
}

void Vehicle::SetSpeed(double speed)
{
    m_speed = speed;
}

void Vehicle::SetAcceleration(double acceleration)
{
    m_acceleration = acceleration;
}

VehicleBounds Vehicle::GetBounds()
{
    return VehicleBounds(GetBoundaries(), GetCorners());
}

void Vehicle::Move(double dt)
{
    double vx = m_speed * FMath::Cos(m_heading);
    double vy = m_speed * FMath::Sin(m_heading);
    double dx = vx * dt;
    double dy = vy * dt;
    double dv = m_acceleration * dt;
    double dHeading = FMath::Sign(m_steeringAngle) * ComputeRotationalVelocity() * dt;
    m_position += FVector2D(dx, dy);
    m_speed += dv;
    if(m_steeringAngle != 0)
    {
        m_heading = static_cast<int>(m_heading + dHeading) % static_cast<int>(2 * PI);
    }
}

void Vehicle::SetSteeringAngle(double steeringAngle)
{
    m_steeringAngle = steeringAngle;
}

TArray<FVector2D> Vehicle::GetCorners()
{
    double xCorners[] {m_property->GetLength() / 2 + m_frontStaticBuffer, -m_property->GetLength() / 2 - m_rearStaticBuffer, -m_property->GetLength() / 2 - m_rearStaticBuffer, m_property->GetLength() / 2 + m_frontStaticBuffer};
    double yCorners[] {m_property->GetWidth() / 2 + m_leftStaticBuffer, m_property->GetWidth() / 2 + m_leftStaticBuffer, -m_property->GetWidth() / 2 - m_rightStaticBuffer, -m_property->GetWidth() / 2 - m_rightStaticBuffer};
    TArray<FVector2D> corners;
    corners.SetNum(4);
    for(int i{}; i < corners.Num(); i++)
    {
        corners[i] = FVector2D(xCorners[i], yCorners[i]);
        corners[i] = corners[i].GetRotated(FMath::RadiansToDegrees(m_heading));
        corners[i] += m_position;
    }
    return corners;
}

TArray<GridLine> Vehicle::GetBoundaries()
{
    TArray<FVector2D> corners = GetCorners();
    TArray<GridLine> boundaries;
    boundaries.SetNum(corners.Num());
    for(int i{}; i < boundaries.Num(); i++)
    {
        int iNext = (i + 1) * corners.Num();
        boundaries[i] = GridLine(corners[i], corners[iNext]);
    }
    return boundaries;
}

double Vehicle::ComputeTurnRadius()
{
    if(m_steeringAngle == 0)
    {
        return 0;
    }
    return FMath::Sqrt(FMath::Square(1.0 / FMath::Sin(m_steeringAngle)) - 0.25) * m_property->GetWheelbase();
}

double Vehicle::ComputeRotationalVelocity() 
{
    double turnRadius = ComputeTurnRadius();
    if(turnRadius)
    {
        return m_speed / turnRadius;
    }
    return 0;
}


