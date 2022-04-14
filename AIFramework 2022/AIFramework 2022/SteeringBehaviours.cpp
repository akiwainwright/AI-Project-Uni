#include "SteeringBehaviours.h"

#include "constants.h"
#include "Vehicle.h"
#include "OuputStrings.h"
#include "CollisionHelper.h"
#include <cstdlib>
#include <ctime>


SteeringBehaviours::SteeringBehaviours(Vehicle* car)
{
	srand(time(NULL));

	m_car = car;

	m_ArriveRadius = 200.0f;

	m_FleeRadius = 250.0f;

	m_WanderAngleRange = 180;
	m_WanderCircleDistance = 40.0f;
	m_WanderCircleRadius = 10.0f;

	m_AvoidSensorLength = 50.0f;
	m_AvoidRadius = 150.0f;
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

	if (m_car->GetFleeState())
	{
		Flee();
	}

	if (m_car->GetPursuitState())
	{
		Pursuit();
	}

	if (m_car->GetWanderState())
	{
		Wander();
	}

	if (m_car->GetObjectAvoidanceState())
	{
		ObjectAvoidance();
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
	Vector2D FleeForce;

	if (m_car->getPosition().Distance(m_car->GetFleeTarget()->getPosition()) <= m_FleeRadius)
	{
		Vector2D desiredForce = (m_car->GetFleeTarget()->getPosition() - m_car->getPosition());
		desiredForce.Normalize();
		desiredForce *= -MAX_VELOCITY;

		FleeForce = desiredForce - m_car->GetVelocity();
		FleeForce.Normalize();
		FleeForce *= MAX_VELOCITY * 3;
	}

	m_SteerForce += FleeForce;

	return m_SteerForce;
}

Vector2D SteeringBehaviours::Pursuit()
{
	Vector2D targetPredictedForce = m_car->GetPursuitTarget()->getPosition() + m_car->GetPursuitTarget()->GetVelocity();
	
	Vector2D desiredForce = targetPredictedForce - m_car->getPosition();
	desiredForce.Normalize();
	desiredForce *= MAX_VELOCITY;

	Vector2D PursuitForce = desiredForce - m_car->GetVelocity();
	PursuitForce.Normalize();
	PursuitForce *= MAX_VELOCITY;

	m_SteerForce += PursuitForce;

	return m_SteerForce;
}

Vector2D SteeringBehaviours::Wander()
{
	Vector2D currentForce;

	//Check if car is stationary
	if (m_car->GetVelocity().Length() == 0.0f)
	{
		currentForce = Vector2D((rand() % 2) / 10.0f, (rand() % 2) / 10.0f);
	}
	else
	{
		currentForce = m_car->GetVelocity();
	}

	Vector2D currentDirection = currentForce;
	currentDirection.Normalize();

	Vector2D wanderCircleCenter = m_car->getPosition() + (currentDirection * m_WanderCircleDistance);
	Vector2D vecToCircleCenter = wanderCircleCenter - m_car->getPosition();
	Vector2D wanderOffset = vecToCircleCenter;
	wanderOffset.Normalize();

	float angleOffset = rand() % ((m_WanderAngleRange * 2) + 1);
	angleOffset -= m_WanderAngleRange;

	wanderOffset = rotateVector(wanderOffset, angleOffset);
	wanderOffset *= m_WanderCircleRadius;

	Vector2D desiredForce = (wanderCircleCenter + wanderOffset) - m_car->getPosition();
	desiredForce.Normalize();
	desiredForce *= MAX_VELOCITY;

	Vector2D WanderForce = desiredForce - currentForce;
	WanderForce.Normalize();
	WanderForce *= MAX_WANDER_FORCE;

	m_SteerForce += WanderForce;


	return m_SteerForce;
}

Vector2D SteeringBehaviours::ObjectAvoidance()
{
	
	Vector2D AvoidSensor = m_car->GetVelocity();
	AvoidSensor.Normalize();
	AvoidSensor *= m_AvoidSensorLength;

	Vector2D avoidPoint1 = m_car->getPosition() + AvoidSensor;
	Vector2D avoidPoint2 = m_car->getPosition() + (AvoidSensor / 2);

	Vector2D avoidTargetPosition = m_car->GetAvoidTarget()->getPosition();

	Vector2D AvoidForce;

	if (avoidTargetPosition.Distance(avoidPoint1) < m_AvoidRadius && avoidTargetPosition.Distance(avoidPoint2) < m_AvoidRadius)
	{
		//Choosing to avoid closer target
		AvoidForce = AvoidSensor * -1;
		AvoidForce.Normalize();
		AvoidForce *= MAX_AVOID_FORCE;

		m_SteerForce += AvoidForce;
	}
	else if (avoidTargetPosition.Distance(avoidPoint1) < m_AvoidRadius)
	{
		OutputDebugStringA("Avoiding\n");
		AvoidForce = AvoidSensor * -1;
		AvoidForce.Normalize();
		AvoidForce *= MAX_AVOID_FORCE;

		m_SteerForce += AvoidForce;
	}
	else if (avoidTargetPosition.Distance(avoidPoint2) < m_AvoidRadius)
	{
		AvoidForce = AvoidSensor * -1;
		AvoidForce.Normalize();
		AvoidForce *= MAX_AVOID_FORCE;

		m_SteerForce += AvoidForce;
	}
	

	return m_SteerForce;
}

void SteeringBehaviours::StopMoving()
{
	m_car->SetVelocity(Vector2D(0.0f, 0.0f));
	m_car->ResetAcceleration();
}

Vector2D SteeringBehaviours::rotateVector(Vector2D currentVector, float angle)
{
	Vector2D rotatedVector;
	angle = degToRad(angle);

	rotatedVector.x = currentVector.x * cos(angle) - currentVector.y * sin(angle);
	rotatedVector.y = currentVector.x * sin(angle) + currentVector.y * cos(angle);

	return rotatedVector;
}
