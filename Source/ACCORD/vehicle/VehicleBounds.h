#pragma once

#include "CoreMinimal.h"
#include "../Geometry/GridLine.h"
/**
 * 
 */

class ACCORD_API VehicleBounds
{
public:
	VehicleBounds();
	~VehicleBounds();
	VehicleBounds(TArray<GridLine> bounds, TArray<FVector2D> corners);
	TArray<GridLine> GetBounds() const { return m_bounds; }
   
    
private:
	TArray<GridLine> m_bounds;
	TArray<FVector2D> m_corners;
	
};