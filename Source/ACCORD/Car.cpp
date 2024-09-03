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

void ACar::UpdateHeading()
{
    double rotation = GetActorRotation().Yaw;
    if(rotation < 0)
    {
        rotation += 360;
    }
    UE_LOG(LogTemp, Warning, TEXT("Update Heading Called for car: %d rotation: %f"), ID, rotation);
    if(FMath::Abs(rotation) <= 45 || FMath::Abs(rotation - 360) <= 45)
    {
        Heading = WEST;
    }
    else if(FMath::Abs(rotation - 90) <= 45)
    {
        Heading = NORTH;
    }
    else if(FMath::Abs(rotation - 180) <= 45)
    {
        Heading = EAST;
    }
    else if(FMath::Abs(rotation - 270) <= 45)
    {
        Heading = SOUTH;
    }
    else
    {
        Heading = -1;
    }
}

void ACar::MaintainSpeed()
{
    if(GetSpeedKPH() > TargetSpeedKPH)
    {
        GetVehicleMovementComponent()->DecreaseThrottleInput(0.1);
        if(GetVehicleMovementComponent()->GetThrottleInput() < 0.1 && (GetSpeedKPH() - TargetSpeedKPH) > 2.5)
        {
            GetVehicleMovementComponent()->SetBrakeInput(0.5);
            UE_LOG(LogTemp, Warning, TEXT("BREAKING!!!!! Car ID: %d Speed: %f TargetSpeed: %f"), ID, GetSpeedKPH(), TargetSpeedKPH);
        }
        else
        {
            GetVehicleMovementComponent()->SetBrakeInput(0);
        }
    }
    else if(GetSpeedKPH() < TargetSpeedKPH)
    {
        GetVehicleMovementComponent()->IncreaseThrottleInput(0.1);
    }
}

void ACar::SetTargetSpeedKPH(double speed)
{
    TargetSpeedKPH = speed;
}

double ACar::GetThrottleInput()
{
    return GetVehicleMovementComponent()->GetThrottleInput();
}

void ACar::SetExitPath()
{
    UpdateHeading();
    ActivePathTag = GetExitTag();
    if(AActor* Path = GetActivePath(ActivePathTag))
    {
        SetActivePath(Path);
    }
    bIsEntrancePathSet = false;
}

void ACar::SetEntrancePath()
{
    UpdateHeading();
    ActivePathTag = GetPathTag();
    if(AActor* Path = GetActivePath(ActivePathTag))
    {
        SetActivePath(Path);
    }
    bIsEntrancePathSet = true;
}

void ACar::BeginPlay()
{
    Super::BeginPlay();
    SetEntrancePath();
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

AActor* ACar::GetActivePath(FName PathTag) const
{
    TArray<AActor*> Paths;
    UGameplayStatics::GetAllActorsOfClassWithTag(this, PathClass, PathTag, Paths);
    if(Paths.Num() >= 1)
    {
       return Paths[0]; 
    }
    return nullptr;
}

void ACar::PrintLogs()
{
    UE_LOG(LogTemp, Warning, TEXT("Car ID: %d Speed KPH: %f Throttle Input: %f Target Speed: %f"), ID, GetSpeedKPH(), GetVehicleMovementComponent()->GetThrottleInput(), TargetSpeedKPH);
}

FName ACar::GetExitTag()
{
    switch(Heading)
    {
        case 0:
            return FName("N");
        case 1:
            return FName("E");
        case 2:
            return FName("W");
        case 3:
            return FName("S");
        default:
            return FName("0");
    }
}

void ACar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(IsActive)
    {
        MaintainSpeed();
    }
    if(ShouldPrintLogs)
    {
        PrintLogs();
    }
}
