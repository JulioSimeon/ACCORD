#pragma once

#include "CoreMinimal.h"
#include "../Geometry/GridLine.h"
#include "VehicleBounds.h"

/**
 * 
 */

class ACCORD_API Vehicle
{
public:
	Vehicle();
	~Vehicle();
    Vehicle(class VehicleProperty* property, int ID);
    void SetHeading(double heading);
    void SetPosition(FVector2D position);
    void SetSpeed(double speed);
    void SetAcceleration(double acceleration);
    VehicleProperty* GetProperty() const { return m_property; }
    FVector2D GetPosition() const { return m_position; }
    VehicleBounds GetBounds();
    double GetSpeed() const { return m_speed; }
    void Move(double dt);
    void SetSteeringAngle(double steeringAngle);
    double GetHeading() const { return m_heading; }
    double GetSteeringAngle() const { return m_steeringAngle; }
    
private:
    VehicleProperty* m_property;
    int m_ID{};
    double m_speed{};
    double m_acceleration{};
    FVector2D m_position;
    double m_steeringAngle{};
    double m_heading{};
    double m_frontStaticBuffer{};
    double m_rearStaticBuffer{};
    double m_leftStaticBuffer{};
    double m_rightStaticBuffer{};
	TArray<FVector2D> GetCorners();
    TArray<GridLine> GetBoundaries();
    double ComputeRotationalVelocity();
    double ComputeTurnRadius();
};