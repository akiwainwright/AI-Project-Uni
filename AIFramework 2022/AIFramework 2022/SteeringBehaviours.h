#pragma once
#include "Vector2D.h"

class SteeringBehaviours
{
private:

	//Private Variables
	Vector2D m_CurrentForce;
	Vector2D m_SteerForce;

	float m_ArriveRadius;

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

	void StopMoving();
	

};

