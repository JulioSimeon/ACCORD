// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ReservationManager/ReservationManager.h"
#include "Tiles/Intersection.h"
#include "IntersectionSlot.h"
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

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UBoxComponent* ACCBoundary;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* IntersectionBoundary;
	Intersection intersection;
	TArray<class ACar*> GetOverlappingCars();
	TArray<ACar*> CarsInsideIntersection;
	TArray<ACar*> ApproachingCars;
	TArray<IntersectionSlot> Slots;
	IntersectionSlot* FindSlot(ACar* car);
	ReservationManager ResMan;
	int m_timeBaseNs{10 * 1000000};
	double GetTimeToEntrance(ACar* car) const;
	void AddSlot(ACar* car);
	UFUNCTION()
	void OnCarEndBoundaryOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnCarEndIntersectionOverlap(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnCarBeginBoundaryOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCarBeginIntersectionOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY(EditInstanceOnly)
	bool IsActive{true};


};
