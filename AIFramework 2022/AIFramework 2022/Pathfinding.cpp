#include "Pathfinding.h"

#include <string>

#include "constants.h"
#include "Vehicle.h"

Pathfinding::Pathfinding(Vehicle* car): m_RemainingWaypoints(0), m_startWaypoint(nullptr),
                                        m_endWaypoint(nullptr)
{
	m_car = car;
	m_wpm = nullptr;
	m_isTraversingPath = false;
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
	if(!m_isTraversingPath)
	{
		CalculatePath();
	}
	else
	{
		TraversePath();
	}
	
}

void Pathfinding::CalculatePath()
{
	
	//Clearing previously calculated path
	m_CalculatedPath.clear();

	m_currentWaypoint = m_wpm->getNearestWaypoint(m_car->getPosition());
	m_currentWaypoint->SetNodeGValue(0.0f);
	m_CheckedWaypoints.push_back(m_currentWaypoint);
	
	m_NearWaypoints = m_wpm->getNeighbouringWaypoints(m_currentWaypoint);
	
	for(int i =0; i < m_NearWaypoints.size(); ++i)
	{
		for(int y = 0;  y < m_CheckedWaypoints.size(); ++y)
		{
			if(m_NearWaypoints[i] == m_CheckedWaypoints[y])
			{
				OutputDebugStringA("Already in path\n");
				break;
			}
			else
			{
				m_CalculatedPath.push_back(m_NearWaypoints[i]);
			}
		}
		
	}
	
	m_CurrentTargetWaypoint = 0;
	m_car->setPositionTo(m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition());
	m_isTraversingPath = true;
}

void Pathfinding::TraversePath()
{
	//Calculating direction to the next waypoint and setting the speed to travel
	Vector2D direction = m_car->getPositionTo() - m_car->getPosition();
	direction.Normalize();
	Vector2D velocity = direction * PATHFINDING_SPEED;
	m_car->SetVelocity(velocity);

	//Creating a buffer zone to allow the vehicle to detect it has reached the target waypoint
	if((m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition() - m_car->getPosition()).Length() < 5.0f)
	{		
		m_CurrentTargetWaypoint++;

		//Checking to make sure there is another waypoint to got 
		if(m_CurrentTargetWaypoint < m_CalculatedPath.size())
		{
			m_car->setPositionTo(m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition()); //if there is updating target waypoint to the next one in the calulated path
		}
		else
		{
			m_isTraversingPath = false; //notifying that vehicle is no waypoint left in the calculatepath
			return;
		}
		
	}
	
	m_car->MoveCar();
}
