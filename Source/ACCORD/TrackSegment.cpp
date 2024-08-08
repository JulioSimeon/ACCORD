// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackSegment.h"
#include "Kismet/KismetMathLibrary.h"
#include "SimulatedCar.h"

TrackSegment::TrackSegment()
{
    
}

TrackSegment::~TrackSegment()
{
}

void TrackSegment::CreateLineSegment(int length, int width, double direction)
{
    m_length = length;
    m_width = width;
    m_direction = (static_cast<int>(direction) % 360);
    SegmentShape = SEGSHAPE::LINEAR;
    m_lineM = UKismetMathLibrary::Tan(UKismetMathLibrary::DegreesToRadians(direction));
    ComputerOtherParameters();
}

void TrackSegment::Create90DegreeTurn(int radius, bool leftTurn, int width, double entryDirection)
{
    SegmentShape = SEGSHAPE::NINETYDEGTURN;
    m_radius = radius;
    m_turn90 = leftTurn ? 1 : -1;
    m_width = width;
    m_direction = entryDirection;
    ComputerOtherParameters();
}

void TrackSegment::SetIsIntersection(bool bIsIntersection)
{
    m_bIsIntersection = bIsIntersection;
}

void TrackSegment::SetSegmentID(FString segmentID)
{
    m_segmentID = segmentID;
}

void TrackSegment::SetAbsoluteLocation(int x, int y)
{
    m_absoluteXLoc = x;
    m_absoluteYLoc = y;
    ComputerOtherParameters();
}

void TrackSegment::ConnectSegments(TrackSegment* previousSegment, TrackSegment* nextSegment)
{
    m_previousSegment = previousSegment;
    m_nextSegment = nextSegment;
}

void TrackSegment::ConnectNextSegment(TrackSegment* nextSegment)
{
    if(nextSegment)
    {
        m_nextSegment = nextSegment;
        m_nextSegment->SetAbsoluteLocation(m_absoluteExitXLoc, m_absoluteExitYLoc);
        m_nextSegment->ConnectPreviousSegment(this);
    }
}

void TrackSegment::ConnectPreviousSegment(TrackSegment* previousSegment)
{
    if(previousSegment)
    {
        m_previousSegment = previousSegment;
    }
}

void TrackSegment::SetBoundaryMargin(int margin)
{
    m_boundaryMargin = margin;
}

FString TrackSegment::GetSegmentID() const
{
    return m_segmentID; 
}

int TrackSegment::GetXLocation() const
{
    return m_absoluteXLoc;
}

int TrackSegment::GetYLocation() const
{
    return m_absoluteYLoc;
}

bool TrackSegment::IsWithInBounds(int x, int y) const
{
    return (x >= FMath::Min(m_xHitBox1, m_xHitBox2)) && (x <= FMath::Max(m_xHitBox1, m_xHitBox2)) && (y >= FMath::Min(m_yHitBox1, m_yHitBox2)) && (y <= FMath::Max(m_yHitBox1, m_yHitBox2));
}

bool TrackSegment::IsWithInBounds(SimulatedCar* car) const
{
    return IsWithInBounds(car->GetXLocation(), car->GetYLocation());
}

double TrackSegment::GetIdealDirection(SimulatedCar* car)
{
    return GetIdealDirection(car->GetXLocation(), car->GetYLocation());
}

double TrackSegment::GetIdealDirection(int x, int y)
{
    switch(SegmentShape)
    {
        case SEGSHAPE::LINEAR:
            return m_direction;

        case SEGSHAPE::NINETYDEGTURN:
            double direction{};
            double tempX{x - m_circleCenterX};
            double tempY{y - m_circleCenterY};
            if(tempX == 0)
            {
                direction = 90; 
            }
            else
            {
                direction = FMath::RadiansToDegrees(FMath::Atan(double(FMath::Abs(y)) / double(FMath::Abs(x))));
            }
            if(tempX < 0 && tempY >= 0)
            {
                direction = 180 - direction;
            }
            else if(tempX < 0 && tempY <= 0)
            {
                direction += 180;
            }
            else if(tempX > 0 && tempY <= 0)
            {
                direction = 360 - direction;
            }
            return static_cast<int>(direction + (m_turn90 * 90)) % 360;
    }
    return 0;
}

int TrackSegment::GetExitXLocation() const
{
    return m_absoluteExitXLoc; 
}

int TrackSegment::GetExitYLocation() const
{
    return m_absoluteExitYLoc;
}

int TrackSegment::GetDistanceFromCenterLine(int xLoc, int yLoc)
{
    int centerDistance{};
    switch(SegmentShape)
    {
        case SEGSHAPE::LINEAR:
            centerDistance = GetDistanceFromCenterLineLinear(xLoc, yLoc);
            break;
        case SEGSHAPE::CIRCULAR:
            centerDistance = GetDistanceFromCenterLineCircular(xLoc, yLoc);
            break;
        case SEGSHAPE::NINETYDEGTURN:
            centerDistance = GetDistanceFromCenterLine90DegreeTurn(xLoc, yLoc);
            break;
    }
    return centerDistance;
}

int TrackSegment::GetDistanceFromCenterLineLinear(int xLoc, int yLoc)
{
    int x{xLoc - m_absoluteXLoc};
    int y{yLoc - m_absoluteYLoc};

    int centerDistance{};
    if(m_direction == 0)
    {
        centerDistance = -y;
    }
    else if(m_direction == 90)
    {
        centerDistance = x;
    }
    else if(m_direction == 180)
    {
        centerDistance = y;
    }
    else if(m_direction == 270)
    {
        centerDistance = -x;
    }
    else
    {
        centerDistance = static_cast<int>(FMath::Abs(m_lineM * x - y) / FMath::Sqrt(m_lineM * 2 + 1));
        int centerY = static_cast<int>(m_lineM * x);
        if(((m_direction > 270 || m_direction < 90) && centerY < y) || (m_direction < 270 && m_direction > 90 && centerY > y))
        {
            centerDistance *= -1;
        }
    }
    return centerDistance;
}

int TrackSegment::GetDistanceFromCenterLineCircular(int xLoc, int yLoc)
{
    int centerDistance{};
    return centerDistance;
}

int TrackSegment::GetDistanceFromCenterLine90DegreeTurn(int xLoc, int yLoc)
{
    return static_cast<int>(FMath::Sqrt(FMath::Square(xLoc - m_circleCenterX) + FMath::Square(yLoc - m_circleCenterY)) - m_radius);
}

void TrackSegment::ComputerOtherParameters()
{
    switch(SegmentShape)
    {
        case SEGSHAPE::LINEAR:
            m_absoluteExitXLoc = static_cast<int>(m_length * UKismetMathLibrary::Cos(UKismetMathLibrary::DegreesToRadians(m_direction))) + m_absoluteXLoc;
            m_absoluteExitYLoc = static_cast<int>(m_length * UKismetMathLibrary::Sin(UKismetMathLibrary::DegreesToRadians(m_direction))) + m_absoluteYLoc;
            break;

        case SEGSHAPE::NINETYDEGTURN:
            double dist = UKismetMathLibrary::Sqrt(2) * m_radius;
            m_absoluteExitXLoc = static_cast<int>(dist * UKismetMathLibrary::Cos(UKismetMathLibrary::DegreesToRadians(m_direction + (m_turn90 * 45)))) + m_absoluteXLoc;
            m_absoluteExitYLoc = static_cast<int>(dist * UKismetMathLibrary::Sin(UKismetMathLibrary::DegreesToRadians(m_direction + (m_turn90 * 45)))) + m_absoluteYLoc;

            m_circleCenterX = static_cast<double>(m_radius * UKismetMathLibrary::Cos(UKismetMathLibrary::DegreesToRadians(90 + m_direction))) + m_absoluteXLoc;
            m_circleCenterY = static_cast<double>(m_radius * UKismetMathLibrary::Sin(UKismetMathLibrary::DegreesToRadians(90 + m_direction))) + m_absoluteYLoc;
            break;
    }
    SimpleBoundsCheck();
}

void TrackSegment::SimpleBoundsCheck()
{
    switch(SegmentShape)
    {
        case SEGSHAPE::LINEAR:
            if(m_direction == 90 || m_direction == 270)
            {
                m_xHitBox1 = static_cast<int>(m_absoluteXLoc - m_width/2 - m_boundaryMargin);
                m_xHitBox2 = static_cast<int>(m_absoluteXLoc + m_width/2 + m_boundaryMargin);
                if(m_absoluteYLoc < m_absoluteExitYLoc)
                {
                    m_yHitBox1 = m_absoluteYLoc - m_boundaryMargin;
                    m_yHitBox2 = m_absoluteExitYLoc + m_boundaryMargin;
                }
                else
                {
                    m_yHitBox1 = m_absoluteExitYLoc - m_boundaryMargin;
                    m_yHitBox2 = m_absoluteYLoc + m_boundaryMargin;
                }
            }
            else
            {
                m_yHitBox1 = static_cast<int>(m_absoluteYLoc - m_width/2 - m_boundaryMargin);
                m_yHitBox2 = static_cast<int>(m_absoluteExitYLoc + m_width/2 + m_boundaryMargin);
                if(m_absoluteXLoc < m_absoluteExitXLoc)
                {
                    m_xHitBox1 = m_absoluteXLoc - m_boundaryMargin;
                    m_xHitBox2 = m_absoluteExitXLoc + m_boundaryMargin;
                }
                else
                {
                    m_xHitBox1 = m_absoluteXLoc + m_boundaryMargin;
                    m_xHitBox2 = m_absoluteExitXLoc - m_boundaryMargin;
                }
            }
            break;

        case SEGSHAPE::NINETYDEGTURN:
            if(m_turn90 == 1)
            {
                switch(static_cast<int>(m_direction))
                {
                    case 0:
                        m_xHitBox1 = m_absoluteXLoc - m_boundaryMargin;
                        m_yHitBox1 = m_absoluteYLoc - static_cast<int>((m_width/2) - m_boundaryMargin);
                        m_xHitBox2 = m_absoluteExitXLoc + m_boundaryMargin;
                        m_yHitBox2 = m_absoluteExitYLoc + static_cast<int>((m_width/2) + m_boundaryMargin);
                        break;

                    case 90:
                        m_xHitBox1 = m_absoluteExitXLoc - m_boundaryMargin;
                        m_yHitBox1 = static_cast<int>(m_absoluteExitYLoc - m_radius - m_boundaryMargin);
                        m_xHitBox2 = m_absoluteXLoc + static_cast<int>(m_width/2) + m_boundaryMargin;
                        m_yHitBox2 = static_cast<int>(m_absoluteExitYLoc + (m_width/2) + m_boundaryMargin);
                        break;

                    case 180:
                        m_xHitBox1 = m_absoluteExitXLoc - static_cast<int>(m_width/2) - m_boundaryMargin;
                        m_yHitBox1 = m_absoluteExitYLoc - m_boundaryMargin;
                        m_xHitBox2 = m_absoluteXLoc + m_boundaryMargin;
                        m_yHitBox2 = m_absoluteYLoc + static_cast<int>(m_width/2) + m_boundaryMargin;
                        break;

                    case 270:
                        m_xHitBox1 = m_absoluteXLoc - static_cast<int>(m_width/2) - m_boundaryMargin;
                        m_yHitBox1 = m_absoluteExitYLoc - static_cast<int>(m_width/2) - m_boundaryMargin;
                        m_xHitBox2 = m_absoluteExitXLoc + m_boundaryMargin;
                        m_yHitBox2 = static_cast<int>(m_absoluteExitYLoc + m_radius + m_boundaryMargin);
                        break;
                }
            }
            else if(m_turn90 == -1)
            {
                switch(static_cast<int>(m_direction))
                {
                    case 0:
                        m_xHitBox1 = m_absoluteXLoc - m_boundaryMargin;
                        m_yHitBox1 = static_cast<int>(m_absoluteExitYLoc - m_boundaryMargin);
                        m_xHitBox2 = m_absoluteExitXLoc + static_cast<int>((m_width/2) + m_boundaryMargin);
                        m_yHitBox2 = m_absoluteYLoc + static_cast<int>((m_width/2) + m_boundaryMargin);
                        break;

                    case 90:
                        m_xHitBox1 = m_absoluteXLoc - static_cast<int>(m_width/2) - m_boundaryMargin;
                        m_yHitBox1 = m_absoluteYLoc - m_boundaryMargin;
                        m_xHitBox2 = m_absoluteExitXLoc + m_boundaryMargin;
                        m_yHitBox2 = static_cast<int>(m_absoluteExitYLoc + (m_width/2) + m_boundaryMargin);
                        break;

                    case 180:
                        m_xHitBox1 = m_absoluteExitXLoc - static_cast<int>(m_width/2) - m_boundaryMargin;
                        m_yHitBox1 = m_absoluteYLoc - static_cast<int>(m_width/2) - m_boundaryMargin;
                        m_xHitBox2 = m_absoluteXLoc + m_boundaryMargin;
                        m_yHitBox2 = m_absoluteYLoc + m_boundaryMargin;
                        break;

                    case 270:
                        m_xHitBox1 = m_absoluteExitXLoc - m_boundaryMargin;
                        m_yHitBox1 = m_absoluteExitYLoc - static_cast<int>(m_width/2) - m_boundaryMargin;
                        m_xHitBox2 = m_absoluteXLoc + static_cast<int>(m_width/2) + m_boundaryMargin;
                        m_yHitBox2 = m_absoluteYLoc + m_boundaryMargin;
                        break;
                }
            }
    }
}