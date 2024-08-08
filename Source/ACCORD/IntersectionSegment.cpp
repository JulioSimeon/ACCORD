// Fill out your copyright notice in the Description page of Project Settings.


#include "IntersectionSegment.h"
#include "SimulationConstants.h"
#include "SimulatedCar.h"


IntersectionSegment::IntersectionSegment()
{
    
}

IntersectionSegment::IntersectionSegment(int size_mm)
{
    SetIsIntersection(true);
    m_DimensionSize = size_mm;
    double direction = 0;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            switch(j)
            {
                case 0:
                    IntersectSegs[i][j].SetNum(3);
                    IntersectSegs[i][j][0].CreateLineSegment(m_DimensionSize/2, m_DimensionSize/2, direction);
                    IntersectSegs[i][j][0].SetSegmentID("Intersection " + FString::FromInt(i) + FString::FromInt(j) + "0\t direction: " + FString::SanitizeFloat(direction));
                    IntersectSegs[i][j][1].Create90DegreeTurn(static_cast<int>(m_DimensionSize/4), true, static_cast<int>(m_DimensionSize/2), direction);
                    IntersectSegs[i][j][1].SetSegmentID("Intersection " + FString::FromInt(i) + FString::FromInt(j) + "1\t direction: " + FString::SanitizeFloat(direction));
                    IntersectSegs[i][j][2].CreateLineSegment(m_DimensionSize/2, m_DimensionSize/2, direction + 90);
                    IntersectSegs[i][j][2].SetSegmentID("Intersection " + FString::FromInt(i) + FString::FromInt(j) + "2\t direction: " + FString::SanitizeFloat(direction));
                    break;
                
                case 1:
                    IntersectSegs[i][j].SetNum(1);
                    IntersectSegs[i][j][0].CreateLineSegment(m_DimensionSize, m_DimensionSize/2, direction);
                    IntersectSegs[i][j][0].SetSegmentID("Intersection " + FString::FromInt(i) + FString::FromInt(j) + "0\t direction: " + FString::SanitizeFloat(direction));
                    break;
                
                case 2:
                    IntersectSegs[i][j].SetNum(1);
                    IntersectSegs[i][j][0].Create90DegreeTurn(static_cast<int>(m_DimensionSize/4), false, static_cast<int>(m_DimensionSize/2), direction);
                    IntersectSegs[i][j][0].SetSegmentID("Intersection " + FString::FromInt(i) + FString::FromInt(j) + "0\t direction: " + FString::SanitizeFloat(direction));
                    break;
            }
        }
        direction += 90;
        direction = static_cast<int>(direction) % 360;
    }
    SetInternalSegmentLocations(m_absoluteXLoc, m_absoluteYLoc);
    intersection = Intersection(m_DimensionSize, m_resolution);

}

IntersectionSegment::~IntersectionSegment()
{
}

void IntersectionSegment::ConnectEntranceExits(TArray<TrackSegment> entrances, TArray<TrackSegment> exits)
{
    m_entrances = entrances;
    m_exits = exits;
}

TrackSegment* IntersectionSegment::GetExitTo(TrackSegment* enterFrom, int direction)
{
    int segIndex = GetHeadingSegIndex(GetEntranceHeading(enterFrom));
    if(segIndex < 0)
    {
        return nullptr;
    }
    switch(direction)
    {
        case LEFT_TURN:
            segIndex++;
            break;
        case RIGHT_TURN:
            if(segIndex <= 0)
            {
                segIndex = 3;
            }
            else
            {
                segIndex--;
            }
            break;
        case STRAIGHT:
            break;
        default:
            return nullptr;
    }
    segIndex = segIndex % 4;
    return &m_exits[segIndex];
}

bool IntersectionSegment::IsReserved(SimulatedCar* car)
{
    return FindSlot(car) != nullptr;
}

bool IntersectionSegment::ReleaseReservation(SimulatedCar* car)
{
    return resMan.Remove(car->getID());
}

bool IntersectionSegment::Reserve(SimulatedCar* car, TrackSegment* entrance, int turn)
{
    if(!car || !entrance)
    {
        return false;
    }
    if(FindSlot(car))
    {
        return true;
    }

    double timeReservationMade = FPlatformTime::Seconds();
    double timeOfArrival = FPlatformTime::Seconds();
    VehicleProperty* vehicleProperty = car->GetVehicleProperty();
    int heading = GetEntranceHeading(entrance);
    int direction = turn;
    bool res = false;
    if(car->GetSpeed() > 0)
    {
        timeOfArrival = FPlatformTime::Seconds();
        timeOfArrival -= m_start;
        double timeToEntrance = GetTimeToEntrance(car, entrance);
        timeOfArrival += timeToEntrance;

        res = resMan.Reserve(&intersection, timeOfArrival, vehicleProperty, car->GetSpeed(), 0, heading, direction, car->getID(), 0);

        if(CheckFront(car))
        {
            res = false;
        }
    }
    else
    {
        res = false;
    }

    if(res)
    {
        AddSlot(car, heading, direction);
    }

    return res;
}

TrackSegment* IntersectionSegment::GetCurrentInternalSegment(SimulatedCar* car)
{
    IntersectionSlot* slot = FindSlot(car);
    if(!slot)
    {
        return nullptr;
    }
    for(int i{}; i < slot->route.Num(); i++)
    {
        if(slot->route[i]->IsWithInBounds(car))
        {
            return slot->route[i];
        }
    }
    return nullptr;
}

int IntersectionSegment::GetHeadingSegIndex(int heading)
{
    switch(heading)
    {
        case EAST:
            return 0;
        case NORTH:
            return 1;
        case WEST:
            return 2;
        case SOUTH:
            return 3;
        default:
            return -1;
    }
}

int IntersectionSegment::GetEntranceHeading(TrackSegment* enterFrom)
{
    if(enterFrom == &m_entrances[0])
    {
        return EAST;
    }
    else if(enterFrom == &m_entrances[1])
    {
        return NORTH;
    }
    else if(enterFrom == &m_entrances[2])
    {
        return WEST;
    }
    else if(enterFrom == &m_entrances[3])
    {
        return SOUTH;
    }
    return 0;
}

IntersectionSlot* IntersectionSegment::FindSlot(SimulatedCar* car)
{
    for(IntersectionSlot& slot : m_slots)
    {
        if(slot.car == car)
        {
            return &slot;
        }
    }
    return nullptr;
}

double IntersectionSegment::GetTimeToEntrance(SimulatedCar* car, TrackSegment* entrance)
{
    double time{};
    double enterX = entrance->GetExitXLocation();
    double enterY = entrance->GetExitYLocation();
    double x = car->GetXLocation();
    double y = car->GetYLocation();
    double speed = car->GetSpeed();

    time  = FMath::Sqrt(FMath::Square(enterX - x) + FMath::Square(enterY - y)) / speed;
    return time;
}

void IntersectionSegment::SetInternalSegmentLocations(int x, int y)
{
    IntersectSegs[0][0][0].SetAbsoluteLocation(x, y + m_DimensionSize / 4);
    IntersectSegs[0][0][1].SetAbsoluteLocation(x + m_DimensionSize / 2, y + m_DimensionSize / 4);
    IntersectSegs[0][0][2].SetAbsoluteLocation(x + static_cast<int>(m_DimensionSize * 0.75), y + m_DimensionSize / 2);

    IntersectSegs[0][1][0].SetAbsoluteLocation(x, y + m_DimensionSize / 4);
    IntersectSegs[0][2][0].SetAbsoluteLocation(x, y + m_DimensionSize / 4);

    IntersectSegs[1][0][0].SetAbsoluteLocation(x + static_cast<int>(m_DimensionSize * 0.75), y);
    IntersectSegs[1][0][1].SetAbsoluteLocation(x + static_cast<int>(m_DimensionSize * 0.75), y + m_DimensionSize / 2 );
    IntersectSegs[1][0][2].SetAbsoluteLocation(x + m_DimensionSize / 2, y +  static_cast<int>(m_DimensionSize * 0.75));

    IntersectSegs[1][1][0].SetAbsoluteLocation(x + static_cast<int>(m_DimensionSize * 0.75), y);
    IntersectSegs[1][2][0].SetAbsoluteLocation(x + static_cast<int>(m_DimensionSize * 0.75), y);

    IntersectSegs[2][0][0].SetAbsoluteLocation(x + m_DimensionSize, y + static_cast<int>(m_DimensionSize * 0.75));
    IntersectSegs[2][0][1].SetAbsoluteLocation(x + m_DimensionSize / 2, y + static_cast<int>(m_DimensionSize * 0.75));
    IntersectSegs[2][0][2].SetAbsoluteLocation(x + m_DimensionSize / 4, y + m_DimensionSize / 2);

    IntersectSegs[2][1][0].SetAbsoluteLocation(x + m_DimensionSize, y + static_cast<int>(m_DimensionSize * 0.75));
    IntersectSegs[2][2][0].SetAbsoluteLocation(x + m_DimensionSize, y + static_cast<int>(m_DimensionSize * 0.75));

    IntersectSegs[3][0][0].SetAbsoluteLocation(x + m_DimensionSize / 4, y + m_DimensionSize);
    IntersectSegs[3][0][1].SetAbsoluteLocation(x + m_DimensionSize / 4, y + m_DimensionSize / 2);
    IntersectSegs[3][0][2].SetAbsoluteLocation(x + m_DimensionSize / 2, y + m_DimensionSize / 4);

    IntersectSegs[3][1][0].SetAbsoluteLocation(x + m_DimensionSize / 4, y + m_DimensionSize);
    IntersectSegs[3][2][0].SetAbsoluteLocation(x + m_DimensionSize / 4, y + m_DimensionSize);


}

bool IntersectionSegment::CheckFront(SimulatedCar* car)
{
    int ID = car->getID();
    switch(ID)
    {
        case 0x5555:
        case 0x6666:
        case 0x7777:
        case 0x8888:
            return false;
        
        default:
            int frontIDs[] {0x5555, 0x6666, 0x7777, 0x8888};
            int count{};
            for(auto& slot : m_slots)
            {
                if(slot.car->getID() == frontIDs[0] || slot.car->getID() == frontIDs[1] || slot.car->getID() == frontIDs[2] || slot.car->getID() == frontIDs[3])
                {
                    count++;
                }
            }
            return count < 4;
    }
}

void IntersectionSegment::AddSlot(SimulatedCar* car, int heading, int direction)
{
    if(!car)
    {
        return;
    }

    IntersectionSlot s1;
    s1.car = car;
    s1.heading = heading;
    s1.direction = direction;
    s1.isReserved = true;

    switch(heading)
    {
        case EAST:
            s1.enterFrom = &m_entrances[0];
            switch(direction)
            {
                case LEFT_TURN:

                    s1.exitTo = &m_exits[1];
                    for(auto& segment : IntersectSegs[0][0])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case STRAIGHT:
                    s1.exitTo = &m_exits[0];
                    for(auto& segment : IntersectSegs[0][1])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case RIGHT_TURN:
                    s1.exitTo = &m_exits[3];
                    for(auto& segment : IntersectSegs[0][2])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
            }
            break;

        case NORTH:
            s1.enterFrom = &m_entrances[1];
            switch(direction)
            {
                case LEFT_TURN:

                    s1.exitTo = &m_exits[2];
                    for(auto& segment : IntersectSegs[1][0])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case STRAIGHT:
                    s1.exitTo = &m_exits[1];
                    for(auto& segment : IntersectSegs[1][1])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case RIGHT_TURN:
                    s1.exitTo = &m_exits[0];
                    for(auto& segment : IntersectSegs[1][2])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
            }
            break;
        
        case WEST:
            s1.enterFrom = &m_entrances[2];
            switch(direction)
            {
                case LEFT_TURN:

                    s1.exitTo = &m_exits[3];
                    for(auto& segment : IntersectSegs[2][0])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case STRAIGHT:
                    s1.exitTo = &m_exits[2];
                    for(auto& segment : IntersectSegs[2][1])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case RIGHT_TURN:
                    s1.exitTo = &m_exits[1];
                    for(auto& segment : IntersectSegs[2][2])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
            }
            break;

        case SOUTH:
            s1.enterFrom = &m_entrances[3];
            switch(direction)
            {
                case LEFT_TURN:

                    s1.exitTo = &m_exits[0];
                    for(auto& segment : IntersectSegs[3][0])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case STRAIGHT:
                    s1.exitTo = &m_exits[3];
                    for(auto& segment : IntersectSegs[3][1])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
                case RIGHT_TURN:
                    s1.exitTo = &m_exits[2];
                    for(auto& segment : IntersectSegs[3][2])
                    {
                        s1.route.Add(&segment);
                    }
                    break;
            }
            break;
    }
    m_slots.Add(s1);


}

