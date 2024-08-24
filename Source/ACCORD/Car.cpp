// Fill out your copyright notice in the Description page of Project Settings.


#include "Car.h"
#include "ChaosVehicleMovementComponent.h"
#include "SimulationConstants.h"
#include "Kismet/GameplayStatics.h"

void ACar::SetAttributes(int id, int x, int y, double orientation, int xdim, int ydim, double speed)
{
    
}

double ACar::GetSpeedEquivalent(int throttle) const
{
    return 0;
}

void ACar::AddRouteDirection(int direction)
{
    
}

float ACar::GetSpeedKPH() const
{
    return FMath::Abs(GetVehicleMovementComponent()->GetForwardSpeed()) * 0.036;
}

float ACar::GetSpeed() const
{
    return FMath::Abs(GetVehicleMovementComponent()->GetForwardSpeed());
}

void ACar::SetThrottle(double throttle)
{
    GetVehicleMovementComponent()->SetThrottleInput(throttle);
}

void ACar::DecreaseThrottle(double throttle)
{
    if(GetVehicleMovementComponent()->GetThrottleInput() >= 0.1)
    {
        GetVehicleMovementComponent()->SetBrakeInput(0.25);
        GetVehicleMovementComponent()->DecreaseThrottleInput(throttle);
    }
    
}

void ACar::InitializeHeading()
{
    double rotation = GetActorRotation().Yaw;
    if(rotation < 0)
    {
        rotation += 360;
    }
    if(FMath::Abs(rotation) <= 30)
    {
        Heading = WEST;
    }
    else if(FMath::Abs(rotation - 90) <= 30)
    {
        Heading = NORTH;
    }
    else if(FMath::Abs(rotation - 180) <= 30)
    {
        Heading = EAST;
    }
    else if(FMath::Abs(rotation - 270) <= 30)
    {
        Heading = SOUTH;
    }
    else
    {
        Heading = -1;
    }
    UE_LOG(LogTemp, Warning, TEXT("Called Initialized Heading"));
    UE_LOG(LogTemp, Warning, TEXT("Heading: %d"), Heading);
}

void ACar::MaintainSpeed()
{
    if(GetSpeedKPH() > TargetSpeed)
    {
        GetVehicleMovementComponent()->DecreaseThrottleInput(0.1);
    }
    else if(GetSpeedKPH() < TargetSpeed)
    {
        GetVehicleMovementComponent()->IncreaseThrottleInput(0.1);
    }
}

void ACar::SetTargetSpeed(double speed)
{
    TargetSpeed = speed;
}

void ACar::BeginPlay()
{
    Super::BeginPlay();
    InitializeHeading();
    UE_LOG(LogTemp, Warning, TEXT("Direction: %d"), Direction);
    PathTag = GetPathTag();
    if(AActor* Path = GetActivePath())
    {
        UE_LOG(LogTemp, Warning, TEXT("ActivePath Set"));
        SetActivePath(Path);
    }
   
}

FName ACar::GetPathTag() const
{
    FString Tag;
    switch(Heading)
    {
        case NORTH:
            Tag = "N";
            break;
        case EAST:
            Tag = "E";
            break;
        case WEST:
            Tag = "W";
            break;
        case SOUTH:
            Tag = "S";
            break;
    }
    switch(Direction)
    {
        case STRAIGHT:
            Tag += "S";
            break;
        case RIGHT_TURN:
            Tag += "RT";
            break;
        case LEFT_TURN:
            Tag += "LT";
            break;
    }
    return FName(Tag);
}

AActor* ACar::GetActivePath() const
{
    TArray<AActor*> Paths;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, PathClass, PathTag, Paths);
    if(Paths.Num() >= 1)
    {
       return Paths[0]; 
    }
    return nullptr;
}

void ACar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(IsActive)
    {
        MaintainSpeed();
    }
}
