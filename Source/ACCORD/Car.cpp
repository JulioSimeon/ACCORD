// Fill out your copyright notice in the Description page of Project Settings.


#include "Car.h"
#include "ChaosVehicleMovementComponent.h"
#include "SimulationConstants.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
 #include "Kismet/KismetMathLibrary.h"

double ACar::GetSpeedKPH() const
{
    return FMath::Abs(GetVehicleMovementComponent()->GetForwardSpeed()) * 0.036;
}

double ACar::GetSpeed() const
{
    return FMath::Abs(GetVehicleMovementComponent()->GetForwardSpeed());
}

void ACar::UpdateHeading()
{
    double rotation = GetActorRotation().Yaw;
    if(rotation < 0)
    {
        rotation += 360;
    }
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
        GetVehicleMovementComponent()->DecreaseThrottleInput(DEFAULT_THROTTLE_STEP);
        if(GetVehicleMovementComponent()->GetThrottleInput() < MaximumBrakingThrottleInput && (GetSpeedKPH() - TargetSpeedKPH) > CarSpeedDeviationThreshold)
        {
            GetVehicleMovementComponent()->SetBrakeInput(BrakePower);
            //UE_LOG(LogTemp, Warning, TEXT("BRAKING!!!!! Car ID: %d Speed: %f TargetSpeed: %f"), ID, GetSpeedKPH(), TargetSpeedKPH);
        }
        else
        {
            GetVehicleMovementComponent()->SetBrakeInput(0);
        }
    }
    else if(GetSpeedKPH() < TargetSpeedKPH)
    {
        GetVehicleMovementComponent()->IncreaseThrottleInput(DEFAULT_THROTTLE_STEP);
        GetVehicleMovementComponent()->SetBrakeInput(0);
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
    SetActivePathSplineComponent(ActivePathTag);
    bIsEntrancePathSet = false;
}

void ACar::SetEntrancePath()
{
    UpdateHeading();
    ActivePathTag = GetPathTag();
    SetActivePathSplineComponent(ActivePathTag);
    bIsEntrancePathSet = true;
}

void ACar::BeginPlay()
{
    Super::BeginPlay();
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

void ACar::SetIsActive()
{
    IsActive = true;
}

void ACar::SetDirection(int NewDirection)
{
    Direction = NewDirection;
}

void ACar::SetID(int NewID)
{
    ID = NewID;
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

double ACar::GetCorrectSteeringValue()
{
    if(ActivePathSplineComponent)
    {
        FVector tangent = ActivePathSplineComponent->FindTangentClosestToWorldLocation(GetActorLocation(), ESplineCoordinateSpace::World);
        tangent.Normalize(NormalizeTolerance);
        tangent *= TangentMultiplier;
        FVector target = ActivePathSplineComponent->FindLocationClosestToWorldLocation(GetActorLocation() + tangent, ESplineCoordinateSpace::World);
        double SteeringValue = UKismetMathLibrary::NormalizedDeltaRotator(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), target), GetActorRotation()).Yaw;
        return UKismetMathLibrary::MapRangeClamped(SteeringValue, -90, 90, -1, 1);
    }
    return 0;
}

void ACar::SetActivePathSplineComponent(FName PathTag)
{
    if(AActor* Path = GetActivePath(PathTag))
    {
        ActivePathSplineComponent = Path->FindComponentByClass<USplineComponent>();
    }
}

void ACar::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if(IsActive)
    {
        MaintainSpeed();
        GetVehicleMovementComponent()->SetSteeringInput(GetCorrectSteeringValue());
    }
    if(ShouldPrintLogs)
    {
        PrintLogs();
    }
}
