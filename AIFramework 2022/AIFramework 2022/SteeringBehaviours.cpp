#include "SteeringBehaviours.h"
#include "Vehicle.h"
#include "OuputStrings.h"

#define MAX_FORCE 100.0f
#define MAX_VELOCITY 5.0f


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
	m_CurrentForceDirection = m_car->getPosition() + m_car->GetVelocity();
	m_CurrentForceDirection.Normalize();
	
	if (m_car->GetSeekState())
	{
		Seek();
	}
	
	if (m_SteerForce.Length() > MAX_FORCE)
	{
		m_SteerForce = ClampVector(m_SteerForce, MAX_FORCE);
	}
	
	m_car->CalculateAcceleration(m_SteerForce);	
}

Vector2D SteeringBehaviours::Seek()
{

	Vector2D desiredForceDirection = m_car->GetTargetPosition() - m_car->getPosition();
	Vector2D SeekForce = desiredForceDirection - m_CurrentForceDirection;
	m_SteerForce += SeekForce;

	return m_SteerForce;
}
