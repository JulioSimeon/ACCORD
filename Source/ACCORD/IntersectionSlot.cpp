#include "IntersectionSlot.h"

IntersectionSlot::IntersectionSlot()
{
    
}

IntersectionSlot::IntersectionSlot(ACar* car, int heading, int direction, bool isReserved)
: Car{car}, Heading{heading}, Direction{direction}, IsReserved{isReserved}
{
    
}

IntersectionSlot::~IntersectionSlot()
{
    
}
