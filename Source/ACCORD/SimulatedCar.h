// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "vehicle/VehiclePropety.h"
#include "Kismet/KismetMathLibrary.h"
#include "SimulationConstants.h"

/**
 * 
 */

struct CarDetails
{
	int carId{};
	int xloc{};
	int yloc{};
	double orientation{};
	int xdimen{};
	int ydimen{};
	double speed{};
	TArray<int> xLocHistory;
	TArray<int> yLocHistory;
	TArray<double> orientHistory;
	TArray<double> timeStampHist;
	TArray<int> routeDirections;
	int routeCount{};

	bool isValidated{false};
	TArray<bool> isValidData; 
};

class ACCORD_API SimulatedCar
{
public:
	SimulatedCar();

	void SetAttributes(int id, int x, int y, double orientation, int xdim, int ydim, double speed);

	double GetSpeedEquivalent(int throttle) const;

	int getID() const;

	static const int DEFAULT_XDim = 300;
    static const int DEFAULT_YDim = 300;

	static const int DEFAULT_THROTTLE_CEILING = 127;
    static const int DEFAULT_THROTTLE_FLOOR = 0;

	static const int SPEED_LIMIT = 70;
	static const int SPEED_FLOOR = 50;

	static const int STEERING_WINDOW = 2;
	static const int THROTTLE_STEP = (SPEED_LIMIT - SPEED_FLOOR) / 5;

	void AddRouteDirection(int direction);
	
	void InitThrottleToSpeedTable(TArray<int> ThrottleValues, TArray<double> SpeedValues);

	bool AdjustThrottle(int throttle);

	int const GetXLocation() const { return m_x; }
	int const GetYLocation() const { return m_y; }

	double GetOrientation() const { return m_orientation; }
	double GetSpeed() const { return m_speed; }

	void SetOutOfBounds(bool OutOfBounds);
	int GetNextRouteDirection();
	bool UpdateLocation();
	bool AlignXAxis();
	VehicleProperty* GetVehicleProperty() { return &m_vehicleProperty; }
	bool throttleDecrement();
	bool throttleIncrement();
	bool MaintainOrientation(double orientation, bool overwrite);
	bool MaintainOrientationTimed(double orientation, double time);
	int GetThrottlePower() const { return m_throttlePower; }

private:
	void InitThrottleToSpeedTable();
	void InitCarDetails(int id, int timeHistorySize, int xLocationHistorySize, int yLocationHistorySize, int orientHistorySize, int routeDirectionsSize);
	
	TMap<int, double> ThrottleToSpeedTable;

	int m_id{};
	int m_x{};
	int m_y{};
	double m_orientation{};
	int m_xdim{};
	int m_ydim{};
	double m_speed{};
	int m_throttlePower{};
	bool m_outOfBounds{};
	double m_lastTime{};
	double m_maintainOrient{};
	bool m_updated{false};
	TArray<int> m_routeQueue;
	TArray<int> m_xLocationHistory;
	TArray<int> m_yLocationHistory;
	TArray<double> m_orientationHistory;
	CarDetails m_carDetails{};
	VehicleProperty m_vehicleProperty{DEFAULT_XDim, DEFAULT_YDim, DEFAULT_XDim * 0.9, 100, 10, UKismetMathLibrary::DegreesToRadians(15)};
	double GetTurnRadius() const;
	void AddXLocationHistory(int xloc);
	void AddYLocationHistory(int yloc);
	void AddOrientationHistory(double orientation);
	double m_xAxisCalib{};
	double m_tempOrient{};
};
