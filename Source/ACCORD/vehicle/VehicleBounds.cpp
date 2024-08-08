// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleBounds.h"

VehicleBounds::VehicleBounds()
{
}

VehicleBounds::~VehicleBounds()
{
}

VehicleBounds::VehicleBounds(TArray<GridLine> bounds, TArray<FVector2D> corners)
{
    m_bounds = bounds;
    m_corners = corners;
}



