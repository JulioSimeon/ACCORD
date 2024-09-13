// Fill out your copyright notice in the Description page of Project Settings.


#include "ACC.h"
#include "Car.h"
#include "SimulationConstants.h"
#include "Components/BoxComponent.h"
#include <sstream>
#include <string>

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

// Called when the game starts or when spawned
void AACC::BeginPlay()
{
	Super::BeginPlay();
	intersection = Intersection(940, 4);
	IntersectionBoundary->OnComponentEndOverlap.AddDynamic(this, &AACC::OnCarEndIntersectionOverlap);
	IntersectionBoundary->OnComponentBeginOverlap.AddDynamic(this, &AACC::OnCarBeginIntersectionOverlap);
	ACCBoundary->OnComponentBeginOverlap.AddDynamic(this, &AACC::OnCarBeginBoundaryOverlap);
	ACCBoundary->OnComponentEndOverlap.AddDynamic(this, &AACC::OnCarEndBoundaryOverlap);
}

// Called every frame
void AACC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(IsActive)
	{
		TArray<ACar*> OverlappingCars = GetOverlappingCars();
		if(CarsInsideIntersection.Num() == 1 && CarsInsideIntersection[0]->GetTargetSpeedKPH() < ACar::DEFAULT_SPEED_CEILING)
		{
			CarsInsideIntersection[0]->SetTargetSpeedKPH(ACar::DEFAULT_SPEED_CEILING);
		}
		if(ApproachingCars.Num() == 1 && ApproachingCars[0]->GetTargetSpeedKPH() < ACar::DEFAULT_SPEED_CEILING)
		{
			ApproachingCars[0]->SetTargetSpeedKPH(ACar::DEFAULT_SPEED_CEILING);
		}
		for(ACar* car : OverlappingCars)
		{
			if(FindSlot(car))
			{
				continue;
			}
			if(!car->IsEntrancePathSet())
			{
				car->SetEntrancePath();
				UE_LOG(LogTemp, Warning, TEXT("Setting Entrance Path of car %d"), car->GetID());
			}
			double EntryTime = GetWorld()->GetTimeSeconds();
			double ArrivalTime = EntryTime + GetTimeToEntrance(car);
			UE_LOG(LogTemp, Warning, TEXT("CarID: %d Car Speed: %f Entry Time: %f Arrival Time: %f Car Heading %d Car Throttle: %f Target Speed: %f"), car->GetID(), car->GetSpeedKPH(), EntryTime, ArrivalTime, car->GetHeading(), car->GetThrottleInput(), car->GetTargetSpeedKPH());
			if(ResMan.Reserve(&intersection, ArrivalTime, car->GetVehicleProperty(), car->GetTargetSpeedKPH() * 27.7778, 0, car->GetHeading(), car->GetDirection(), car->GetID(), m_timeBaseNs))
			{
				UE_LOG(LogTemp, Warning, TEXT("Reservation accepted: %d"), car->GetID());
				AddSlot(car);				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Reservation DENIED: %d"), car->GetID());
				if(car->GetTargetSpeedKPH() >= 5 && (car->GetSpeedKPH() - car->GetTargetSpeedKPH()) < 1)
				{
					UE_LOG(LogTemp, Warning, TEXT("Speed Reduced"));
					car->SetTargetSpeedKPH(car->GetTargetSpeedKPH() - 5);
				}
			}

		}
	}
	
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
			UE_LOG(LogTemp, Warning, TEXT("Distance to the intersection: %f"), FMath::Abs(car->GetActorLocation().Y) - (ACar::DEFAULT_XDim / 2) - 470);
			return (FMath::Abs(car->GetActorLocation().Y) - (ACar::DEFAULT_XDim / 2) - 470) / car->GetSpeed();
		case EAST:
		case WEST:
			UE_LOG(LogTemp, Warning, TEXT("Distance to the intersection: %f"), FMath::Abs(car->GetActorLocation().X) - (ACar::DEFAULT_XDim / 2) - 470);
			return (FMath::Abs(car->GetActorLocation().X) - (ACar::DEFAULT_XDim / 2) - 470) / car->GetSpeed();
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

		for(int i{}; i < Slots.Num(); i++)
		{
			if(Slots[i].Car->GetID() == car->GetID())
			{
				Slots.RemoveAt(i);
			}
		}
		car->SetExitPath();
		UE_LOG(LogTemp, Warning, TEXT("Setting Exit Path of car %d"), car->GetID());
	}
}

void AACC::OnCarEndIntersectionOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(ACar* car = Cast<ACar>(OtherActor))
	{
		ResMan.Remove(car->GetID());
		car->SetTargetSpeedKPH(ACar::DEFAULT_SPEED_CEILING);
		UE_LOG(LogTemp, Warning, TEXT("Car %d exited intersection at time: %f"), car->GetID(), GetWorld()->GetTimeSeconds());
		CarsInsideIntersection.Remove(car);
		ApproachingCars.Remove(car);
	}
}

void AACC::OnCarBeginBoundaryOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACar* car = Cast<ACar>(OtherActor))
	{
		ApproachingCars.Add(car);
	}
}

void AACC::OnCarBeginIntersectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ACar* car = Cast<ACar>(OtherActor))
	{
		CarsInsideIntersection.Add(car);
	}
	
}

TArray<ACar*> AACC::GetOverlappingCars()
{
	TArray<AActor*> Actors;
	TArray<ACar*> Cars;
	GetOverlappingActors(Actors);

	for(AActor* actor : Actors)
	{
		if(ACar* car = Cast<ACar>(actor))
		{
			Cars.Add(car);
		}
	}
	return Cars;
}

