#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ACCORD_API VehicleProperty
{
public:
	VehicleProperty();
	VehicleProperty(double length, double width, double wheelbase, double maxSpeed, double maxAcceleration, double maxSteeringAngle);
	~VehicleProperty();

	double GetLength() const { return m_length; }
	double GetWidth() const { return m_width; }
	double GetWheelbase() const { return m_wheelbase; }
	double GetMaxSpeed() const { return m_maxSpeed; }
	double GetMaxAcceleration() const { return m_maxAcceleration; }
	double GetMaxSteeringAngle() const { return m_maxSteeringAngle; }
	double GetMinimumTurnRadius() const;

private:
	const double m_length{};
	const double m_width{};
	const double m_wheelbase{};
	const double m_maxSpeed{};
	const double m_maxAcceleration{};
	const double m_maxSteeringAngle{};
};