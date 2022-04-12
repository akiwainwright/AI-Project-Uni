#pragma once
#include "Vector2D.h"

class SteeringBehaviours
{
private:

	//Private Variables
	Vector2D m_CurrentForce;
	Vector2D m_SteerForce = Vector2D(0.0f, 0.0f);

	float m_ArriveRadius;
	float m_FleeRadius;

	class Vehicle* m_car;

public:
	//Constructor and Destructor
	SteeringBehaviours(Vehicle* car);
	~SteeringBehaviours();

	//Public functions
	void SteeringUpdate();

private:

	//Private Functions
	Vector2D Seek();
	Vector2D Arrive();
	Vector2D Flee();
	Vector2D Pursuit();

	void StopMoving();
	

};

