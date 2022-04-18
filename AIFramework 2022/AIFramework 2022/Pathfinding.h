#pragma once
#include "Waypoint.h"
#include "WaypointManager.h"



class Pathfinding
{
private:
	bool m_isTraversingPath = false;
	bool m_isFindingPath = false;
	
	int m_CurrentTargetWaypoint = 0;
	
	class Vehicle* m_car;
	WaypointManager* m_wpm;

	vecWaypoints m_CalculatedPath{};

	vecWaypoints m_FreeWaypoints;
	vecWaypoints m_CheckedWaypoints;

	Waypoint* m_startWaypoint;
	Waypoint* m_endWaypoint;
	Waypoint* m_currentWaypoint;

public:

	Pathfinding(Vehicle* car);
	~Pathfinding();

	void Update(float deltaTime);
	void CalculatePath();
	void AStarPath();
	void FindWaypointWithLowestF();
	void MoveNodeFromFreeToChecked();
	void GeneratePath();
	void TraversePath();
	void ResetWaypointPathData();

	void SetWaypointManager(WaypointManager* wpm) { m_wpm = wpm;}

	bool CheckNodeHasAlreadyBeenChecked(Waypoint* wp);

	inline bool GetTraverseState() { return m_isTraversingPath;}
	inline void SetTraverseState(bool state) { m_isTraversingPath = state; }
	inline vecWaypoints CalculatedPath() { return m_CalculatedPath;}
};

