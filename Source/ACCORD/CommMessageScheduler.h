// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ACCORD_API CommMessageScheduler
{
public:
	CommMessageScheduler();
	~CommMessageScheduler();

	void CMDSetThrotte(int carID, int throttle);
};
