// Fill out your copyright notice in the Description page of Project Settings.


#include "Intersection.h"

Intersection::Intersection()
{
}

Intersection::~Intersection()
{
}

Intersection::Intersection(double length, int resolution)
{
    m_length = length;
    m_resolution = resolution;
    m_tileLength = length / resolution;
}

bool Intersection::IsInside(FVector2D point) const
{
    return point.X >= 0 && point.X <= m_length && point.Y >= 0 && point.Y <= m_length;
}


