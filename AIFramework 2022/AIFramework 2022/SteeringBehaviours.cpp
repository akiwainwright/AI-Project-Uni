#include "SteeringBehaviours.h"

#include "constants.h"
#include "Vehicle.h"
#include "OuputStrings.h"


SteeringBehaviours::SteeringBehaviours(Vehicle* car)
{
	m_car = car;
	m_ArriveRadius = 200.0f;
}

SteeringBehaviours::~SteeringBehaviours()
{
	delete m_car;
	m_car = nullptr;
}

void SteeringBehaviours::SteeringUpdate()
{	
	m_SteerForce = Vector2D(0.0f, 0.0f);

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
	SeekForce.Normalize();
	SeekForce *= MAX_VELOCITY;
	
	m_SteerForce += SeekForce;

	return m_SteerForce;
}

Vector2D SteeringBehaviours::Arrive()
{
	Vector2D desiredForce = m_car->GetTargetPosition() - m_car->getPosition();
	desiredForce.Normalize();

	float distanceToTargetPostiion = (m_car->GetTargetPosition() - m_car->getPosition()).Length();

	Vector2D ArriveForce;

	if(distanceToTargetPostiion > m_ArriveRadius)
	{
		desiredForce *= MAX_VELOCITY;

		ArriveForce = desiredForce - m_car->GetVelocity();
		ArriveForce.Normalize();
		ArriveForce *= MAX_VELOCITY;
	}
	else if(distanceToTargetPostiion < m_ArriveRadius && distanceToTargetPostiion > 1.0f)
	{
		float speedDamp = MAX_VELOCITY * distanceToTargetPostiion / m_ArriveRadius;
		desiredForce = desiredForce * speedDamp;

		ArriveForce = desiredForce - m_car->GetVelocity();
		ArriveForce.Normalize();
		ArriveForce *= speedDamp;
	}
	else
	{
		StopMoving();
	}



	m_SteerForce += ArriveForce;
	
	return m_SteerForce;
}

Vector2D SteeringBehaviours::Flee()
{
	return Vector2D();
}

Vector2D SteeringBehaviours::Pursuit()
{
	return Vector2D();
}

void SteeringBehaviours::StopMoving()
{
	m_car->SetVelocity(Vector2D(0.0f, 0.0f));
	m_car->ResetAcceleration();
}
