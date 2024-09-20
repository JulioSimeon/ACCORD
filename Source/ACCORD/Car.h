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

	static const int DEFAULT_XDim = 470; //centimeters
    static const int DEFAULT_YDim = 225; //centimeters
	static const int DEFAULT_Wheelbase = 265; //centimeters
	static const int DEFAULT_SPEED_CEILING = 50; //kilometers per hour
	static inline const double DEFAULT_SPEED_CEILING_CMS = 1400; //centimeters per second
	static inline const double DEFAULT_MAX_ACCELERATION_CMS = 233; //centimeters per second
	static inline const double DEFAULT_STEERING_ANGLE = 23.48; //degrees; default: 17 calculated: 23.48 upper: 40
	static const int DEFAULT_THROTTLE_CEILING = 1;
    static const int DEFAULT_THROTTLE_FLOOR = 0;
	static inline const double DEFAULT_THROTTLE_STEP = 0.1;
	int GetID() const { return ID; }
	int GetHeading() const { return Heading; }
	int GetDirection() const { return Direction; }
	VehicleProperty* GetVehicleProperty() { return &m_vehicleProperty; }
	double GetSpeedKPH() const;
	double GetSpeed() const;
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
	VehicleProperty m_vehicleProperty{DEFAULT_XDim, DEFAULT_YDim, DEFAULT_Wheelbase, DEFAULT_SPEED_CEILING_CMS, DEFAULT_MAX_ACCELERATION_CMS, FMath::DegreesToRadians(DEFAULT_STEERING_ANGLE)}; 
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> PathClass;
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
