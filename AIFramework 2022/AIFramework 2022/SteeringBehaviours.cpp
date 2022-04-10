#include "SteeringBehaviours.h"

#include "constants.h"
#include "Vehicle.h"
#include "OuputStrings.h"


SteeringBehaviours::SteeringBehaviours(Vehicle* car)
{
	m_car = car;
	m_ArriveRadius = 150.0f;
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

	if(m_car->GetArriveState())
	{
		Arrive();
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

Vector2D SteeringBehaviours::Arrive()
{
	Vector2D desiredForce = m_car->GetTargetPosition() - m_car->getPosition();
	desiredForce.Normalize();

	float distanceToTargetPostiion = (m_car->GetTargetPosition() - m_car->getPosition()).Length();

	if(distanceToTargetPostiion > m_ArriveRadius)
	{
		desiredForce = desiredForce * MAX_VELOCITY;
	}
	else if(distanceToTargetPostiion < m_ArriveRadius && distanceToTargetPostiion > 5.0f)
	{
		OutputDebugStringA("Slowing Down\n");
		desiredForce = desiredForce * MAX_VELOCITY * (distanceToTargetPostiion/m_ArriveRadius);
	}
	else
	{
		StopMoving();
	}

	Vector2D ArriveForce = desiredForce - m_car->GetVelocity();

	m_SteerForce += ArriveForce;
	
	return m_SteerForce;
}

Vector2D SteeringBehaviours::StopMoving()
{
	m_car->SetVelocity(Vector2D(0.0f, 0.0f));
	m_car->CalculateAcceleration(Vector2D(0.0f,0.0f));
}
