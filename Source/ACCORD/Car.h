// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "vehicle/VehiclePropety.h"
#include "Car.generated.h"

/**
 * 
 */
UCLASS()
class ACCORD_API ACar : public AWheeledVehiclePawn
{
	GENERATED_BODY()
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void SetAttributes(int id, int x, int y, double orientation, int xdim, int ydim, double speed);
	int getID() const;

	//dimension in centimeters
	static const int DEFAULT_XDim = 470;
    static const int DEFAULT_YDim = 225;
	static const int DEFAULT_Wheelbase = 265;
	static const int DEFAULT_SPEED_CEILING = 50;
	static const int DEFAULT_THROTTLE_CEILING = 1;
    static const int DEFAULT_THROTTLE_FLOOR = 0;
	static const int DEFAULT_THROTTLE_STEP = 0.1;
	virtual double GetSpeedEquivalent(int throttle) const;
	virtual void AddRouteDirection(int direction);
	int GetID() const { return ID; }
	int GetHeading() const { return Heading; }
	int GetDirection() const { return Direction; }
	VehicleProperty* GetVehicleProperty() { return &m_vehicleProperty; }
	float GetSpeedKPH() const;
	float GetSpeed() const;
	void SetThrottle(double throttle);
	void DecreaseThrottle(double throttle);
	void UpdateHeading();
	void MaintainSpeed();
	void SetTargetSpeedKPH(double speed);
	double GetTargetSpeedKPH() const { return TargetSpeedKPH; }
	double GetThrottleInput();
	void SetExitPath();
	UFUNCTION(BlueprintCallable)
	void SetEntrancePath();
	bool IsEntrancePathSet() const { return bIsEntrancePathSet; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int Direction{-1};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int Heading{};
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName ActivePathTag{};
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	bool IsActive{};
	FName GetPathTag() const;
	AActor* GetActivePath(FName PathTag) const;
	UFUNCTION(BlueprintImplementableEvent)
	void SetActivePath(AActor* Path);
	UFUNCTION(BlueprintCallable)
	void SetIsActive();
	UFUNCTION(BlueprintCallable)
	void SetDirection(int NewDirection);
	
private:
	UPROPERTY(EditInstanceOnly)
	int ID{};
	VehicleProperty m_vehicleProperty{DEFAULT_XDim, DEFAULT_YDim, DEFAULT_Wheelbase, 1400, 233, FMath::DegreesToRadians(23.48)}; //default: 17 calculated: 23.48 upper: 40
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> PathClass;
	int m_x{};
	int m_y{};
	double m_orientation{};
	int m_xdim{DEFAULT_XDim};
	int m_ydim{DEFAULT_YDim};
	double m_speed{};
	UPROPERTY(EditAnywhere)
	double TargetSpeedKPH{DEFAULT_SPEED_CEILING};
	void PrintLogs();
	UPROPERTY(EditInstanceOnly)
	bool ShouldPrintLogs{};
	FName GetExitTag();
	bool bIsEntrancePathSet{};
};
