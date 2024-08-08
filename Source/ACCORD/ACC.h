// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ACC.generated.h"

UCLASS()
class ACCORD_API AACC : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AACC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	static void RunPredetermined2DGrid(TArray<int> tags, TArray<int> headings, TArray<int> manuevers);

	static void InitializeCar(class SimulatedCar* car, int tag, int heading, int direction);

	static void InitializeSpeedTable(class SimulatedCar* car);

private:
	
};
