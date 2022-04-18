#include "Pathfinding.h"
#include "Vehicle.h"

Pathfinding::Pathfinding(Vehicle* car)
{
	m_car = car;
	m_wpm = car->getWaypointManager();
	//m_CalculatedPath = vector<Waypoint*>();
	
	//m_NeigbouringPoints = m_wpm->getNeighbouringWaypoints(m_wpm->getNearestWaypoint(m_car->getPosition()));

	// m_CalculatedPath.push_back(m_NeigbouringPoints[0]);
	// m_CalculatedPath.push_back(m_NeigbouringPoints[1]);
	// m_CalculatedPath.push_back(m_NeigbouringPoints[3]);

	m_CurrentTargetWaypoint = 0;
}

Pathfinding::~Pathfinding()
{
	delete m_car;
	m_car = nullptr;

	delete m_wpm;
	m_wpm = nullptr;
}

void Pathfinding::Update(float deltaTime)
{
	
}
