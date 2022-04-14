#include "Pathfinding.h"
#include "Vehicle.h"

Pathfinding::Pathfinding(Vehicle* car)
{
	m_car = car;
}

Pathfinding::~Pathfinding()
{
	delete m_car;
	m_car = nullptr;
}
