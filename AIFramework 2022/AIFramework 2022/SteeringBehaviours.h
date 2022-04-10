#pragma once
#include "Vector2D.h"

class SteeringBehaviours
{
private:

	//Private Variables
	Vector2D m_CurrentForceDirection;
	Vector2D m_SteerForce;

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
	

};

