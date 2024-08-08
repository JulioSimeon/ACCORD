// Fill out your copyright notice in the Description page of Project Settings.


#include "ACC.h"
#include "CommMessageScheduler.h"
#include "Car.h"
#include "Track.h"
#include "CarSimulator.h"
#include "SimulatedCar.h"
#include "SimulationConstants.h"
#include <sstream>
#include <string>

// Sets default values
AACC::AACC()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AACC::BeginPlay()
{
	Super::BeginPlay();
	TArray<int> IDS {0x6a40, 0x6743, 0x673b, 0x6a1a};
	TArray<int> HEADINGS {0, 1, 2, 3};
	TArray<int> MANUEVERS {-3, -3, -3, -3};
	RunPredetermined2DGrid(IDS, HEADINGS, MANUEVERS);
}

// Called every frame
void AACC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AACC::RunPredetermined2DGrid(TArray<int> tags, TArray<int> headings, TArray<int> manuevers)
{
	CommMessageScheduler CommSched;

	int nCars = tags.Num();
	

	TArray<SimulatedCar> cars;
	cars.SetNum(nCars);

	Track track(640, 800, 1160, 0);
	track.PrintAllSegments();
	TArray<Track> routes;
	routes.SetNum(nCars);

	CarSimulator CarSim;
	CarSim.SetScheduler(&CommSched);
	CarSim.SetTrack(&track);

	// for(int i = 0; i < cars.Num(); i++)
	// {
	// 	InitializeCar(&cars[i], tags[i], headings[i], manuevers[i]);
	// 	InitializeSpeedTable(&cars[i]);
	// 	cars[i].AdjustThrottle(SimulatedCar::SPEED_LIMIT);
	// 	CarTracker carTracker = track.UpdateCarTracker(&cars[i]);
	// 	routes[i] = track.GetRouteTrack(&cars[i], carTracker.currentSeg);
	// 	std::stringstream ss;
    // 	ss << std::hex << cars[i].getID();
	// 	FString ID (ss.str().c_str());
	// 	UE_LOG(LogTemp, Display, TEXT("Track route for car 0x%s"), *(ID));
	// 	routes[i].PrintAllSegments();

	// 	// CarSim.AddCar(&cars[i], &routes[i]);
	// 	// cars[i].UpdateLocation();
	// 	// cars[i].AlignXAxis();
	// }

	// double initWait = FPlatformTime::Seconds() + 10;
	// while(initWait > FPlatformTime::Seconds())
	// {
	// 	CarSim.Simulate();
	// }
	
}

void AACC::InitializeCar(SimulatedCar* car, int tag, int heading, int direction)
{
	int startX{};
	int startY{};
	double orientation{};
	switch(heading)
	{
		case EAST:
			startX = 1;
			startY = 2280;
			orientation = 0;
			break;
		case NORTH:
			startX = 2920;
			startY = 1;
			orientation = 90;
			break;
		case WEST:
			startX = 5199;
			startY = 2920;
			orientation = 180;
			break;
		case SOUTH:
			startX = 2280;
			startY = 5199;
			orientation = 270;
			break;

	}
	car->SetAttributes(tag, startX, startY, orientation, SimulatedCar::DEFAULT_XDim, SimulatedCar::DEFAULT_YDim, car->GetSpeedEquivalent(SimulatedCar::DEFAULT_THROTTLE_FLOOR));
	car->AddRouteDirection(direction);
}

void AACC::InitializeSpeedTable(SimulatedCar* car)
{
	TArray<int> throttleColumn { 0, 50, 60, 75, 90, 100};
	TArray<double> speedColumn {0, 370, 750, 1035, 1200, 1300};

	car->InitThrottleToSpeedTable(throttleColumn, speedColumn);

	switch(car->getID())
	{
		case 0x6a40:
			throttleColumn[0] = 0;
			throttleColumn[1] = 50;
			throttleColumn[2] = 60;
			throttleColumn[3] = 75;
			throttleColumn[4] = 90;
			throttleColumn[5] = 100;

			speedColumn[0] = 0;
			speedColumn[1] = 618;
			speedColumn[2] = 870;
			speedColumn[3] = 1090;
			speedColumn[4] = 1394;
			speedColumn[5] = 1456;
			car->InitThrottleToSpeedTable(throttleColumn, speedColumn);
			break;

		case 0x6743:
			throttleColumn[0] = 0;
			throttleColumn[1] = 40;
			throttleColumn[2] = 60;
			throttleColumn[3] = 75;
			throttleColumn[4] = 90;
			throttleColumn[5] = 100;

			speedColumn[0] = 0;
			speedColumn[1] = 385;
			speedColumn[2] = 571;
			speedColumn[3] = 942;
			speedColumn[4] = 1235;
			speedColumn[5] = 1318;
			car->InitThrottleToSpeedTable(throttleColumn, speedColumn);
			break;

		case 0x673b:
			throttleColumn[0] = 0;
			throttleColumn[1] = 50;
			throttleColumn[2] = 60;
			throttleColumn[3] = 75;
			throttleColumn[4] = 90;
			throttleColumn[5] = 100;

			speedColumn[0] = 0;
			speedColumn[1] = 295;
			speedColumn[2] = 722;
			speedColumn[3] = 920;
			speedColumn[4] = 1218;
			speedColumn[5] = 1281;
			car->InitThrottleToSpeedTable(throttleColumn, speedColumn);
			break;
		
		case 0x6a1a:
			throttleColumn[0] = 0;
			throttleColumn[1] = 50;
			throttleColumn[2] = 60;
			throttleColumn[3] = 75;
			throttleColumn[4] = 90;
			throttleColumn[5] = 100;

			speedColumn[0] = 0;
			speedColumn[1] = 570;
			speedColumn[2] = 870;
			speedColumn[3] = 1190;
			speedColumn[4] = 1330;
			speedColumn[5] = 1420;
			car->InitThrottleToSpeedTable(throttleColumn, speedColumn);
			break;
	}
	car->InitThrottleToSpeedTable(throttleColumn, speedColumn);
}

