#include "Pathfinding.h"

#include "constants.h"
#include "Vehicle.h"

Pathfinding::Pathfinding(Vehicle* car): m_RemainingWaypoints(0), m_startWaypoint(nullptr),
                                        m_endWaypoint(nullptr)
{
	m_car = car;
	m_wpm = nullptr;

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
	TraversePath();
}

void Pathfinding::CalculatePath()
{
	if(m_isTraversingPath)
	{
		return;
	}

	OutputDebugStringA("Calculating Path\n");
	m_currentWaypoint = m_wpm->getNearestWaypoint(m_car->getPosition());
	m_NearWaypoints = m_wpm->getNeighbouringWaypoints(m_currentWaypoint);
	
	for(Waypoint* waypoint : m_NearWaypoints)
	{
		m_CalculatedPath.push_back(waypoint);
	}
	
	m_RemainingWaypoints = m_CalculatedPath.size() - 1;
	m_CurrentTargetWaypoint = 0;
	m_car->setPositionTo(m_CalculatedPath[0]->getPosition());
	m_isTraversingPath = true;
}

void Pathfinding::TraversePath()
{	
	if(m_RemainingWaypoints < 1)
	{
		m_CalculatedPath.clear();
		m_isTraversingPath = false;
		CalculatePath();
		return;
	}

	if(m_CurrentTargetWaypoint < m_CalculatedPath.size() - 1)
	{
		Vector2D direction = m_car->getPositionTo() - m_car->getPosition();
		direction.Normalize();
		Vector2D velocity = direction * PATHFINDING_SPEED;

		if((m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition() - m_car->getPosition()).Length() < 2.0f)
		{
			m_CurrentTargetWaypoint++;
			m_RemainingWaypoints--;
			m_car->setPositionTo(m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition());
		}

		m_car->SetVelocity(velocity);
		m_car->MoveCar();
	}
}
