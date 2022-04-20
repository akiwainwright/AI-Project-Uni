#include "Pathfinding.h"

#include <string>

#include "constants.h"
#include "Vehicle.h"

Pathfinding::Pathfinding(Vehicle* car): m_startWaypoint(nullptr),
                                        m_endWaypoint(nullptr)
{
	m_car = car;
	m_wpm = nullptr;
	m_isTraversingPath = false;
	m_isFindingPath = false;
	m_fuel = m_maxFuel;
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
		string fuel = "Remaining fuel: " + to_string(m_fuel) + "\n";
		OutputDebugStringA(fuel.c_str());
		
		m_fuel -= deltaTime * m_fuelLossPerFrame;
		TraversePath();
	}
}

void Pathfinding::CalculatePath()
{
	ResetWaypointPathData();
	//Clearing previously calculated path
	m_CalculatedPath.clear();
	m_FreeWaypoints.clear();
	m_CheckedWaypoints.clear();
	
	m_startWaypoint = nullptr;
	m_endWaypoint = nullptr;
	m_currentWaypoint = nullptr;
	

	//Setting the start and end nodes for A* pathfinding
	m_startWaypoint = m_wpm->getNearestWaypoint(m_car->getPosition());
	m_endWaypoint = m_wpm->getNearestWaypoint(m_car->GetPathfindDestination()->getPosition());
	m_FreeWaypoints.push_back(m_startWaypoint);

	m_startWaypoint->SetNodeGValue(0.0f);
	m_startWaypoint->SetNodeHValue(m_startWaypoint->getPosition().Distance(m_endWaypoint->getPosition()));
	m_startWaypoint->SetNodeFValue();
	
	AStarPath();
}

void Pathfinding::AStarPath()
{
	FindWaypointWithLowestF();

	MoveNodeFromFreeToChecked();

	if(m_currentWaypoint == m_endWaypoint)
	{
		GeneratePath();
		return;
	}

	vecWaypoints NearWaypoints = m_wpm->getNeighbouringWaypoints(m_currentWaypoint);

	for(int i = 0; i < NearWaypoints.size(); ++i)
	{
		if(!CheckNodeHasAlreadyBeenChecked(NearWaypoints[i]))
		{
			Waypoint* waypointToAdd;
			waypointToAdd = NearWaypoints[i];
			float distanceFromCurrentWaypoint = m_currentWaypoint->getPosition().Distance(waypointToAdd->getPosition());

			//Check to see if the node has another parent
			if(waypointToAdd->GetParentWaypoint() != nullptr)
			{
				//Comparing fvalues with current waypoint if the waypoint being checked has a parent waypoint
				Waypoint* temp = waypointToAdd;
				temp->SetNodeGValue(distanceFromCurrentWaypoint + m_currentWaypoint->GetNodeGValue());
				temp->SetNodeHValue(waypointToAdd->getPosition().Distance(m_endWaypoint->getPosition()));
				temp->SetNodeFValue();
				
				//Re-parenting waypoint if fvalue is smaller
				if(temp->GetNodeFValue() < waypointToAdd->GetNodeFValue())
				{
					waypointToAdd->SetParentWaypoint(m_currentWaypoint);
					waypointToAdd->SetNodeGValue(temp->GetNodeGValue());
					waypointToAdd->SetNodeHValue(temp->GetNodeHValue());
					waypointToAdd->SetNodeFValue();
				}
				
			}
			else
			{
				//Calculating values for waypoint and setting parent
				waypointToAdd->SetParentWaypoint(m_currentWaypoint);
				waypointToAdd->SetNodeGValue(distanceFromCurrentWaypoint + m_currentWaypoint->GetNodeGValue());
				waypointToAdd->SetNodeHValue(waypointToAdd->getPosition().Distance(m_endWaypoint->getPosition()));
				waypointToAdd->SetNodeFValue();				
			}			

			m_FreeWaypoints.push_back(waypointToAdd);
		}
	}
	AStarPath();
}

void Pathfinding::FindWaypointWithLowestF()
{
	if(m_FreeWaypoints.size() < 2)
	{
		m_currentWaypoint = m_FreeWaypoints[m_FreeWaypoints.size() - 1];
	}
	else
	{
		Waypoint* temp = nullptr;
		for(int i = 0; i < m_FreeWaypoints.size(); ++i)
		{
			if(temp == nullptr)
			{
				temp = m_FreeWaypoints[i];
			}
			else
			{
				if(m_FreeWaypoints[i]->GetNodeFValue() < temp->GetNodeFValue())
				{
					temp = m_FreeWaypoints[i];
				}
			}
		}
		m_currentWaypoint = temp;
		
	}
}

void Pathfinding::MoveNodeFromFreeToChecked()
{
	for(int i = 0; i < m_FreeWaypoints.size(); ++i)
	{
		if(m_FreeWaypoints[i] == m_currentWaypoint)
		{
			m_FreeWaypoints.erase(m_FreeWaypoints.begin() + i);
			m_CheckedWaypoints.push_back(m_currentWaypoint);
			break;
		}
	}
}

void Pathfinding::GeneratePath()
{
	if(m_currentWaypoint->GetParentWaypoint() != nullptr)
	{
		m_CalculatedPath.insert(m_CalculatedPath.begin(), m_currentWaypoint);
		m_currentWaypoint = m_currentWaypoint->GetParentWaypoint();
		GeneratePath();
	}
	else
	{
		m_CalculatedPath.insert(m_CalculatedPath.begin(), m_startWaypoint);
		m_isTraversingPath = true;
	}
}

bool Pathfinding::CheckNodeHasAlreadyBeenChecked(Waypoint* wp)
{
	for(int i = 0; i < m_CheckedWaypoints.size(); ++i)
	{
		if(wp == m_CheckedWaypoints[i])
		{
			return true;
		}
	}
	return false;
}

void Pathfinding::TraversePath()
{
	//Calculating direction to the next waypoint and setting the speed to travel
	Vector2D direction = m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition() - m_car->getPosition();
	direction.Normalize();
	Vector2D velocity;
	
	if(m_fuel > 0)
	{
		velocity = direction * PATH_TRAVERSE_SPEED;
	}
	else
	{
		m_fuel = 0.0f;
		velocity = direction * 0.05f * PATH_TRAVERSE_SPEED;
	}
	
	m_car->SetVelocity(velocity);

	//Creating a buffer zone to allow the vehicle to detect it has reached the target waypoint
	if((m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition() - m_car->getPosition()).Length() < 5.0f)
	{
		OutputDebugStringA("Got To Next Waypoint\n");
		m_CurrentTargetWaypoint++;

		//Checking to make sure there is another waypoint to got 
		if(m_CurrentTargetWaypoint < m_CalculatedPath.size())
		{
			m_car->setPositionTo(m_CalculatedPath[m_CurrentTargetWaypoint]->getPosition()); //if there is updating target waypoint to the next one in the calulated path
		}
		else
		{
			m_CurrentTargetWaypoint = 0;
			m_isTraversingPath = false; //notifying that vehicle is no waypoint left in the calculate path
			return;
		}
		
	}
	
	m_car->MoveCar();
}

void Pathfinding::ResetWaypointPathData()
{
	for(int i =0; i < m_FreeWaypoints.size(); ++i)
	{
		m_FreeWaypoints[i]->SetParentWaypoint(nullptr);
		m_FreeWaypoints[i]->SetNodeGValue(0.0f);
		m_FreeWaypoints[i]->SetNodeHValue(0.0f);
		m_FreeWaypoints[i]->SetNodeHValue(0.0f);
	}

	for(int i = 0; i < m_CheckedWaypoints.size(); ++i)
	{
		m_CheckedWaypoints[i]->SetParentWaypoint(nullptr);
		m_CheckedWaypoints[i]->SetNodeGValue(0.0f);
		m_CheckedWaypoints[i]->SetNodeHValue(0.0f);
		m_CheckedWaypoints[i]->SetNodeHValue(0.0f);
	}

	for(int i = 0; i < m_CalculatedPath.size(); ++i)
	{
		m_CalculatedPath[i]->SetParentWaypoint(nullptr);
		m_CalculatedPath[i]->SetNodeGValue(0.0f);
		m_CalculatedPath[i]->SetNodeHValue(0.0f);
		m_CalculatedPath[i]->SetNodeHValue(0.0f);
	}
}
