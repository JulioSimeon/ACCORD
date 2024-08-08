// Fill out your copyright notice in the Description page of Project Settings.


#include "VehiclePropety.h"
#include "Kismet/KismetMathLibrary.h"

VehicleProperty::VehicleProperty(double length, double width, double wheelbase, double maxSpeed, double maxAcceleration, double maxSteeringAngle)
: m_length{length}, m_width{width}, m_wheelbase{wheelbase}, m_maxSpeed{maxSpeed}, m_maxAcceleration{maxAcceleration}, m_maxSteeringAngle{maxSteeringAngle}
{
	
}

VehicleProperty::VehicleProperty()
{
	
}

VehicleProperty::~VehicleProperty()
{
}

double VehicleProperty::GetMinimumTurnRadius() const
{
	if(m_maxSteeringAngle == 0)
	{
		return 0;
	}
	return UKismetMathLibrary::Sqrt(FMath::Pow(1.f / UKismetMathLibrary::Sin(m_maxSteeringAngle), 2) - 0.25) * m_wheelbase;
}
