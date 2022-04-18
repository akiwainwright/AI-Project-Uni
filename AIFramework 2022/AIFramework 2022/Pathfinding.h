#pragma once
#include <vector>
#include "Waypoint.h"
#include "WaypointManager.h"



class Pathfinding
{
private:
	int m_CurrentTargetWaypoint;
	
	class Vehicle* m_car;
	WaypointManager* m_wpm;

	//vector<Waypoint*> m_CalculatedPath;
	//vecWaypoints m_NeigbouringPoints;

public:

	Pathfinding(Vehicle* car);
	~Pathfinding();

	void Update(float deltaTime);
	void TraversePath();
};

