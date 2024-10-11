// Fill out your copyright notice in the Description page of Project Settings.


#include "ACC.h"
#include "Car.h"
#include "SimulationConstants.h"
#include "Components/BoxComponent.h"
#include "ChaosVehicleMovementComponent.h"


// Sets default values
AACC::AACC()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ACCBoundary = CreateDefaultSubobject<UBoxComponent>(TEXT("ACC Boundary"));
	RootComponent = ACCBoundary;
	IntersectionBoundary = CreateDefaultSubobject<UBoxComponent>(TEXT("Intersection Boundary"));
	IntersectionBoundary->SetupAttachment(RootComponent);
}

void AACC::ResetReservations()
{
	Slots.Empty();
	TimerMap.Empty();
	GetWorldTimerManager().ClearAllTimersForObject(this);
	ResMan.Reset();
}

// Called when the game starts or when spawned
void AACC::BeginPlay()
{
	Super::BeginPlay();
	m_intersection = Intersection(LaneWidth * 2, IntersectionResolution);
	IntersectionBoundary->OnComponentEndOverlap.AddDynamic(this, &AACC::OnCarEndIntersectionOverlap);
	IntersectionBoundary->OnComponentBeginOverlap.AddDynamic(this, &AACC::OnCarBeginIntersectionOverlap);
	ACCBoundary->OnComponentBeginOverlap.AddDynamic(this, &AACC::OnCarBeginBoundaryOverlap);
	ACCBoundary->OnComponentEndOverlap.AddDynamic(this, &AACC::OnCarEndBoundaryOverlap);
}

// Called every frame
void AACC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

IntersectionSlot* AACC::FindSlot(ACar* car)
{
	for(int i{}; i < Slots.Num(); i++)
	{
		if(Slots[i].Car == car)
		{
			return &Slots[i];
		}
	}
	return nullptr;
}

double AACC::GetTimeToEntrance(ACar* car) const
{
	switch(car->GetHeading())
	{
		case NORTH:
		case SOUTH: 
			UE_LOG(LogTemp, Warning, TEXT("Distance to the intersection: %f"), FMath::Abs(car->GetActorLocation().Y) - (ACar::DEFAULT_XDim / 2) - LaneWidth);
			return (FMath::Abs(car->GetActorLocation().Y) - (ACar::DEFAULT_XDim / 2) - LaneWidth) / car->GetSpeed();
		case EAST:
		case WEST:
			UE_LOG(LogTemp, Warning, TEXT("Distance to the intersection: %f"), FMath::Abs(car->GetActorLocation().X) - (ACar::DEFAULT_XDim / 2) - LaneWidth);
			return (FMath::Abs(car->GetActorLocation().X) - (ACar::DEFAULT_XDim / 2) - LaneWidth) / car->GetSpeed();
		default:
			return 0;
	}
}

void AACC::AddSlot(ACar* car)
{
	IntersectionSlot slot(car, car->GetHeading(), car->GetDirection(), true);
	Slots.Add(slot);
}

void AACC::OnCarEndBoundaryOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ACar* car = Cast<ACar>(OtherActor))
	{
		car->SetExitPath();
	}
}

void AACC::OnCarEndIntersectionOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ACar* car = Cast<ACar>(OtherActor))
	{
		for(int i{}; i < Slots.Num(); i++)
		{
			if(Slots[i].Car->GetID() == car->GetID())
			{
				Slots.RemoveAt(i);
				AccelerateLeadCar();
			}
		}
		ResMan.Remove(car->GetID());
		car->SetTargetSpeedKPH(ACar::DEFAULT_SPEED_CEILING);
		UE_LOG(LogTemp, Warning, TEXT("Car %d exited intersection at time: %f"), car->GetID(), GetWorld()->GetTimeSeconds());
	}
}

void AACC::OnCarBeginBoundaryOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACar* car = Cast<ACar>(OtherActor))
	{
		if(FindSlot(car) || TimerMap.Contains(car)) // car already has reservation || timer has been set to retry reservation again
		{
			return;
		}
		SetEntrancePathIfUnset(car);
		Reserve(car);
	}
}

void AACC::OnCarBeginIntersectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AACC::Reserve(ACar* car)
{
	double EntryTime = GetWorld()->GetTimeSeconds();
	double ArrivalTime = EntryTime + GetTimeToEntrance(car);
	if(ResMan.Reserve(&m_intersection, ArrivalTime, car->GetVehicleProperty(), car->GetTargetSpeedKPH() * 27.7778, 0, car->GetHeading(), car->GetDirection(), car->GetID(), m_timeBaseNs))
	{
		UE_LOG(LogTemp, Warning, TEXT("Reservation accepted: %d"), car->GetID());
		AddSlot(car);
		if(TimerMap.Contains(car))
		{
			if(GetWorldTimerManager().IsTimerActive(TimerMap[car]))
			{
				GetWorldTimerManager().ClearTimer(TimerMap[car]);
			}
			TimerMap.Remove(car);
		}
	}
	else if(!TimerMap.Contains(car))
	{
		UE_LOG(LogTemp, Warning, TEXT("Reservation DENIED: %d Reservation attempted at: %f"), car->GetID(), GetWorld()->GetTimeSeconds());
		DecelerateCar(car);
		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "Reserve", car);
		FTimerHandle TimerHandle;
		TimerMap.Add(car, TimerHandle);
		GetWorldTimerManager().SetTimer(TimerMap[car], Delegate, TimerDuration, true);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Reservation DENIED: %d Reservation attempted at: %f"), car->GetID(), GetWorld()->GetTimeSeconds());
		DecelerateCar(car);
	}
}

void AACC::DecelerateCar(ACar* car)
{
	if(car->GetTargetSpeedKPH() > CarSpeedLowerBound && (car->GetSpeedKPH() - car->GetTargetSpeedKPH()) < CarSpeedDeviationThreshold)
	{
		car->SetTargetSpeedKPH(car->GetTargetSpeedKPH() - CarSpeedDecrementStep);
	}
	else if(car->GetTargetSpeedKPH() <= CarSpeedLowerBound)
	{
		car->SetTargetSpeedKPH(CarSpeedFloor);
	}
}

void AACC::SetEntrancePathIfUnset(ACar* car)
{
	if(!car->IsEntrancePathSet())
	{
		car->SetEntrancePath();
		UE_LOG(LogTemp, Warning, TEXT("Setting Entrance Path of car %d"), car->GetID());
	}
}

void AACC::AccelerateLeadCar()
{
	if(Slots.Num() >= 1 && Slots[0].Car->GetTargetSpeedKPH() < ACar::DEFAULT_SPEED_CEILING && FMath::Abs(Slots[0].Car->GetVehicleMovementComponent()->GetSteeringInput()) < CarSteeringInputThreshold)
	{
		Slots[0].Car->SetTargetSpeedKPH(ACar::DEFAULT_SPEED_CEILING);
	}
}


