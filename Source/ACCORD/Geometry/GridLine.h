#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

class ACCORD_API GridLine
{
public:
	GridLine();
	~GridLine();
    GridLine(double x1, double y1, double x2, double y2);
    GridLine(FVector2D p1, FVector2D p2);
    double m_x1{};
    double m_y1{};
    double m_x2{};
    double m_y2{};
    double GetSlope() const;
    
private:
    
};