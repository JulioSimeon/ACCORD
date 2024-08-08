// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

enum class SEGSHAPE
{
	CIRCULAR,
	LINEAR,
	NINETYDEGTURN,
	LEFTTURN,
	RIGHTTURN,
	STRAIGHTTURN,
};

class ACCORD_API TrackSegment
{
public:
	TrackSegment();
	virtual ~TrackSegment();
	
	void CreateLineSegment(int length, int width, double direction);
	void Create90DegreeTurn(int radius, bool leftTurn, int width, double entryDirection);
	void SetSegmentID(FString segmentID);
	void SetAbsoluteLocation(int x, int y);
	void ConnectSegments(TrackSegment* previousSegment, TrackSegment* nextSegment);
	void ConnectNextSegment(TrackSegment* nextSegment);
	void ConnectPreviousSegment(TrackSegment* previousSegment);
	void SetBoundaryMargin(int margin);
	FString GetSegmentID() const;
	int GetXLocation() const;
	int GetYLocation() const;
	bool IsIntersection() const { return m_bIsIntersection; }
	bool IsWithInBounds(int x, int y) const;
	bool IsWithInBounds(class SimulatedCar* car) const;
	TrackSegment* GetNextSegment() const { return m_nextSegment; }
	TrackSegment* GetPreviousSegment() const { return m_previousSegment; }
	double GetIdealDirection(SimulatedCar* car);
	double GetIdealDirection(int x, int y);
	int GetExitXLocation() const;
	int GetExitYLocation() const;
	SEGSHAPE GetSegmentShape() const { return SegmentShape; }
	int GetDistanceFromCenterLine(int xLoc, int yLoc);
	

protected:
    void SetIsIntersection(bool bIsIntersection);
	

private:
    bool m_bIsIntersection = false;
	int m_length{};
	int m_width{};
	double m_direction{};
	SEGSHAPE SegmentShape = SEGSHAPE::LINEAR;
	double m_lineM{}; 
	double m_radius;
	void ComputerOtherParameters();
	void SimpleBoundsCheck();

	int m_absoluteExitXLoc{};
	int m_absoluteExitYLoc{};
	int m_absoluteXLoc{};
	int m_absoluteYLoc{};
	int m_turn90{};
	double m_circleCenterX{};
	double m_circleCenterY{};

	int m_xHitBox1{};
	int m_yHitBox1{};
	int m_xHitBox2{};
	int m_yHitBox2{};
	int m_boundaryMargin{10};
	UPROPERTY()
	TrackSegment* m_previousSegment = nullptr;
	UPROPERTY()
	TrackSegment* m_nextSegment = nullptr;
	UPROPERTY()
	FString m_segmentID = "Track Segment";
	int GetDistanceFromCenterLineLinear(int xLoc, int yLoc);
	int GetDistanceFromCenterLineCircular(int xLoc, int yLoc);
	int GetDistanceFromCenterLine90DegreeTurn(int xLoc, int yLoc);
};