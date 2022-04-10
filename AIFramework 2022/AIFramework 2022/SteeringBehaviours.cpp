#include "SteeringBehaviours.h"

#include "constants.h"
#include "Vehicle.h"
#include "OuputStrings.h"


SteeringBehaviours::SteeringBehaviours(Vehicle* car)
{
	m_car = car;
}

SteeringBehaviours::~SteeringBehaviours()
{
	delete m_car;
	m_car = nullptr;
}

void SteeringBehaviours::SteeringUpdate()
{	
	if (m_car->GetSeekState())
	{
		Seek();
	}
	
	m_SteerForce.Truncate(MAX_FORCE);
	
	m_car->CalculateAcceleration(m_SteerForce);	
}

Vector2D SteeringBehaviours::Seek()
{

	Vector2D desiredForce = m_car->GetTargetPosition() - m_car->getPosition();
	desiredForce.Normalize();
	desiredForce *= MAX_VELOCITY;
	
	Vector2D SeekForce = desiredForce - m_car->GetVelocity();
	
	m_SteerForce += SeekForce;

	return m_SteerForce;
}
