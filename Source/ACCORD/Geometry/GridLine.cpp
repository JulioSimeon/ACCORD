// Fill out your copyright notice in the Description page of Project Settings.


#include "GridLine.h"

GridLine::GridLine()
{
}

GridLine::~GridLine()
{
}

GridLine::GridLine(double x1, double y1, double x2, double y2)
    : m_x1{x1}, m_y1{y1}, m_x2{x2}, m_y2{x2}
{
    
}

GridLine::GridLine(FVector2D p1, FVector2D p2)
    : m_x1{p1.X}, m_y1{p1.Y}, m_x2{p2.X}, m_y2{p2.Y}
{
    
}

double GridLine::GetSlope() const
{
    return (m_y2 - m_y1) / (m_x2 - m_x1);
} 