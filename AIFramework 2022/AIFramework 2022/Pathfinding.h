#pragma once
#include "Waypoint.h"
#include "WaypointManager.h"



class Pathfinding
{
private:
	bool m_isTraversingPath = false;
	
	int m_RemainingWaypoints;
	int m_CurrentTargetWaypoint = 0;
	
	class Vehicle* m_car;
	WaypointManager* m_wpm;

	vecWaypoints m_CalculatedPath{};
	vecWaypoints m_NearWaypoints{};

	vecWaypoints m_FreeWaypoints;
	vecWaypoints m_CheckedWaypoints;

	Waypoint* m_startWaypoint;
	Waypoint* m_endWaypoint;
	Waypoint* m_currentWaypoint;

	//vector<Waypoint*> m_CalculatedPath;
	//vecWaypoints m_NeigbouringPoints;

public:

	Pathfinding(Vehicle* car);
	~Pathfinding();

	void Update(float deltaTime);
	void CalculatePath();
	void TraversePath();

	void SetWaypointManager(WaypointManager* wpm) { m_wpm = wpm;}
};

