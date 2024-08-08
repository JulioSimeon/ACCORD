#pragma once

#include "CoreMinimal.h"
/**
 * 
 */
class ACCORD_API Intersection
{
public:
	Intersection();
	~Intersection();
    Intersection(double length, int resolution);
    double GetLength() const { return m_length; }
    bool IsInside(FVector2D point) const;
	int GetResolution() const { return m_resolution; }
    double GetTileLength() const { return m_tileLength; }
    
private:
	double m_length{};
    int m_resolution{};
    double m_tileLength{};
	
};