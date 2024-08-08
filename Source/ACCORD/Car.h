// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "Car.generated.h"

/**
 * 
 */
UCLASS()
class ACCORD_API ACar : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:
	virtual void SetAttributes(int id, int x, int y, double orientation, int xdim, int ydim, double speed);
	int getID() const;

	static const int DEFAULT_XDim = 300;
    static const int DEFAULT_YDim = 300;

	static const int DEFAULT_THROTTLE_CEILING = 127;
    static const int DEFAULT_THROTTLE_FLOOR = 0;

	virtual double GetSpeedEquivalent(int throttle) const;
	virtual void AddRouteDirection(int direction);

protected:
	int m_id{};
	int m_x{};
	int m_y{};
	double m_orientation{};
	int m_xdim{};
	int m_ydim{};
	double m_speed{};
};
