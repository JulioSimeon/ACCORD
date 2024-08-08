// Fill out your copyright notice in the Description page of Project Settings.


#include "SimulatedCar.h"
#include "CommMessageScheduler.h"

SimulatedCar::SimulatedCar()
{
    InitThrottleToSpeedTable();
    InitCarDetails(m_id, 10, 10, 10, 10, 16);
}

void SimulatedCar::SetAttributes(int id, int x, int y, double orientation, int xdim, int ydim, double speed)
{
    m_id = id;
    m_x = x;
    m_y = y;
    m_orientation = orientation;
    m_xdim = xdim;
    m_ydim = ydim;
    m_speed = speed;
}

double SimulatedCar::GetSpeedEquivalent(int throttle) const 
{
    if(ThrottleToSpeedTable.Contains(throttle))
    {
        return ThrottleToSpeedTable[throttle];
    }
    double equivalentSpeed{};
    TArray<int> ThrottleValues;
    ThrottleToSpeedTable.GetKeys(ThrottleValues);
    ThrottleValues.Sort();

    for(int i = 0; i < ThrottleValues.Num() - 1; i++)
    {
        if(throttle >= ThrottleValues[i] && throttle <= ThrottleValues[i + 1])
        {
            return ThrottleToSpeedTable[ThrottleValues[i]] + (ThrottleToSpeedTable[ThrottleValues[i + 1]] - ThrottleToSpeedTable[ThrottleValues[i]]) / (ThrottleValues[i + 1] - ThrottleValues[i]) * (throttle - ThrottleValues[i]);
        }
    }
    return equivalentSpeed;
}

int SimulatedCar::getID() const
{
    return m_id;
}

void SimulatedCar::AddRouteDirection(int direction)
{
    if(m_routeQueue.Num() >= m_carDetails.routeDirections.Num())
    {
        return;
    }
    m_routeQueue.Push(direction);
    m_carDetails.routeCount = m_routeQueue.Num();
    for(int i = 0; i < m_routeQueue.Num(); i++)
    {
        if(i >= m_carDetails.routeDirections.Num())
        {
            break;
        }
        m_carDetails.routeDirections[i] = m_routeQueue[i];
    }
}

void SimulatedCar::InitThrottleToSpeedTable(TArray<int> ThrottleValues, TArray<double> SpeedValues)
{
    ThrottleToSpeedTable.Empty();

    for(int i = 0; i < ThrottleValues.Num(); i++)
    {
        ThrottleToSpeedTable.Add(ThrottleValues[i], SpeedValues[i]);
    }
}

void SimulatedCar::InitThrottleToSpeedTable()
{
    TArray<int> ThrottleValues {0, 32, 64, 96, 127};
    TArray<int> SpeedValues {0, 0, 830, 1480, 1900};

    ThrottleToSpeedTable.Empty();
    
    for(int i = 0; i < ThrottleValues.Num(); i++)
    {
        ThrottleToSpeedTable.Add(ThrottleValues[i], SpeedValues[i]);
    }
}

bool SimulatedCar::AdjustThrottle(int throttle)
{
    if(throttle > SPEED_LIMIT)
    {
        m_throttlePower = SPEED_LIMIT;
    }
    else if (throttle > 0 && throttle <= SPEED_FLOOR)
    {
        m_throttlePower = SPEED_FLOOR;
    }
    else
    {
        m_throttlePower = FMath::Max(0, throttle);
    }
    m_speed = GetSpeedEquivalent(m_throttlePower);
    if(m_commSched)
    {
        m_commSched->CMDSetThrotte(m_id, m_throttlePower);
    }
    return true; 
}

void SimulatedCar::SetOutOfBounds(bool OutOfBounds)
{
    m_outOfBounds = OutOfBounds;
}

int SimulatedCar::GetNextRouteDirection()
{
    if(m_routeQueue.IsEmpty())
    {
        return STRAIGHT;
    }
    return m_routeQueue[0];
}

bool SimulatedCar::UpdateLocation()
{
    if(m_lastTime <= 0)
    {
        m_lastTime = FPlatformTime::Seconds();
        return false;
    }
    m_speed = GetSpeedEquivalent(m_throttlePower);
    double dt = FPlatformTime::Seconds() - m_lastTime;
    m_lastTime = FPlatformTime::Seconds();
    double dx = m_speed * UKismetMathLibrary::Cos(FMath::DegreesToRadians(m_carDetails.orientation)) * dt;
    double dy = m_speed * UKismetMathLibrary::Sin(FMath::DegreesToRadians(m_carDetails.orientation)) * dt;
    m_carDetails.xloc += dx;
    m_carDetails.yloc += dy;
    double dheading = m_speed / GetTurnRadius() * dt;
    dheading = FMath::RadiansToDegrees(dheading);
    double orientDev = m_maintainOrient - m_carDetails.orientation;
    if(FMath::Abs(orientDev) > 180)
    {
        if(orientDev >= 0)
        {
            orientDev = -(360 - orientDev);
        }
        else
        {
            orientDev = orientDev + 360;
        }
    }
    orientDev = static_cast<int>(orientDev) % 360;

    if(FMath::Abs(orientDev) > STEERING_WINDOW)
    {
        if(orientDev < 0)
        {
            dheading = -dheading;
        }
    }
    else
    {
        dheading = 0;
    }

    if(FMath::Abs(dheading) > FMath::Abs(orientDev))
    {
        m_carDetails.orientation = m_maintainOrient;
    }
    else
    {
        m_carDetails.orientation += dheading;
    }
    m_carDetails.orientation = static_cast<int>(m_carDetails.orientation) % 360;
    m_updated = true;

    AddOrientationHistory(m_carDetails.orientation);
    AddXLocationHistory(m_carDetails.xloc);
    AddYLocationHistory(m_carDetails.yloc);
    

    return m_updated;
}

void SimulatedCar::InitCarDetails(int id, int timeHistorySize, int xLocationHistorySize, int yLocationHistorySize, int orientHistorySize, int routeDirectionsSize)
{
    m_carDetails.carId = id;
    m_carDetails.orientHistory.SetNum(timeHistorySize);
    m_carDetails.timeStampHist.SetNum(timeHistorySize);
    m_carDetails.xLocHistory.SetNum(xLocationHistorySize);
    m_carDetails.yLocHistory.SetNum(yLocationHistorySize);
    m_carDetails.routeDirections.SetNum(routeDirectionsSize);
    m_carDetails.xdimen = DEFAULT_XDim;
    m_carDetails.ydimen = DEFAULT_YDim;
}

double SimulatedCar::GetTurnRadius() const
{
    return 300;
}

void SimulatedCar::AddXLocationHistory(int xloc)
{
    if(m_xLocationHistory.Num() >= m_carDetails.xLocHistory.Num())
    {
        m_xLocationHistory.Pop();
    }
    m_xLocationHistory.Insert(xloc, 0);
    m_carDetails.xLocHistory = m_xLocationHistory;
}

void SimulatedCar::AddYLocationHistory(int yloc)
{
    if(m_yLocationHistory.Num() >= m_carDetails.yLocHistory.Num())
    {
        m_yLocationHistory.Pop();
    }
    m_yLocationHistory.Insert(yloc, 0);
    m_carDetails.yLocHistory = m_yLocationHistory;
}

void SimulatedCar::AddOrientationHistory(double orientation)
{
    if(m_orientationHistory.Num() >= m_carDetails.orientHistory.Num())
    {
        m_orientationHistory.Pop();
    }
    m_orientationHistory.Insert(orientation, 0);
    m_carDetails.orientHistory = m_orientationHistory;
}

bool SimulatedCar::AlignXAxis()
{
    UpdateLocation();
    m_xAxisCalib = 0;
    return true;
}

bool SimulatedCar::throttleDecrement()
{
    if(m_throttlePower <= SPEED_FLOOR)
    {
        return AdjustThrottle(0);
    }
    return AdjustThrottle(m_throttlePower - THROTTLE_STEP);
}

bool SimulatedCar::throttleIncrement()
{
    if(m_throttlePower <= 0)
    {
        return AdjustThrottle(SPEED_FLOOR);
    }
    return AdjustThrottle(m_throttlePower + THROTTLE_STEP);
}

bool SimulatedCar::MaintainOrientation(double orientation, bool overwrite)
{
    m_maintainOrient = orientation;
    if(overwrite)
    {
        m_tempOrient = orientation;
    }
    return true;
}

bool SimulatedCar::MaintainOrientationTimed(double orientation, double time)
{
    m_tempOrient = orientation;
    return true;
}
