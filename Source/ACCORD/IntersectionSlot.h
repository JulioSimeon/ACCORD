#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ACCORD_API IntersectionSlot
{
public:
	IntersectionSlot();
    IntersectionSlot(class ACar* car, int heading, int direction, bool isReserved);
	~IntersectionSlot();
    
    ACar* Car = nullptr;
    int Heading{};
    int Direction{};
    bool IsReserved = false;

private:
    
};




