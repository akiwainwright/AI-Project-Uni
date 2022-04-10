#pragma once

#include "DrawableGameObject.h"
#include "WaypointManager.h"
#include "Vector2D.h"
#include "Collidable.h"
#include "SteeringBehaviours.h"


enum class carColour
{
	redCar,
	blueCar,
};

class Vehicle : public DrawableGameObject, public Collidable
{
public:
	virtual HRESULT initMesh(ID3D11Device* pd3dDevice, carColour colour);
	virtual void update(const float deltaTime);

	void setMaxSpeed(const float maxSpeed) { m_maxSpeed = maxSpeed; }
	void setCurrentSpeed(const float speed); // a ratio: a value between 0 and 1 (1 being max speed)
	void setPositionTo(Vector2D positionTo); // a position to move to
	void setVehiclePosition(Vector2D position); // the current position - this resets positionTo
	void setWaypointManager(WaypointManager* wpm);
	void hasCollided() {}

	//Physics related functions
	inline void CalculateAcceleration(Vector2D force) { m_Acceleration = force / m_Mass; }
	inline Vector2D GetAcceleration() { return m_Acceleration; }
	inline Vector2D GetVelocity() { return m_Velocity; }
	inline void SetVelocity(Vector2D velocity) { m_Velocity = velocity;} 


	//Steering Related Setters and Getters
	#pragma region Steering Get and Set

	inline void ToggleSeek(bool state) { m_SeekOn = state; }
	inline bool GetSeekState() { return m_SeekOn; }

	inline void ToggleArrive(bool state) { m_ArriveOn = state; }
	inline bool GetArriveState() { return m_ArriveOn; }

	inline void ToggleFlee(bool state) { m_FleeOn = state; }
	inline bool GetFleeState() { return m_FleeOn; }

	inline void TogglePursuit(bool state) { m_PursuitOn = state; }
	inline bool GetPursuitState() { return m_PursuitOn; }

	inline void ToggleObjectAvoidance(bool state) { m_ObjectAvoidance = state; }
	inline bool GetObjectAvoidanceState() { return m_ObjectAvoidance; }

	inline void ToggleWander(bool state) { m_WanderingOn = state; }
	inline bool GetWanderState() { return m_WanderingOn; }

	#pragma endregion

	inline void SetTargetPosition(Vector2D targetPos) { m_targetPosition = targetPos; }
	inline Vector2D GetTargetPosition() { return m_targetPosition; }


protected: // protected methods


protected: // preotected properties
	float m_maxSpeed;
	float m_currentSpeed;
	
	Vector2D m_currentPosition;
	Vector2D m_startPosition;
	Vector2D m_positionTo;
	Vector2D m_targetPosition;
	Vector2D m_lastPosition;
	WaypointManager* m_waypointManager;

	//Physic related Variables
	Vector2D m_Velocity;
	Vector2D m_ClampedVelocity;
	Vector2D m_Acceleration;
	float m_Mass;

	//Steering Variables
	SteeringBehaviours* m_Steering;

	bool m_SeekOn = false;
	bool m_ArriveOn = false;
	bool m_FleeOn = false;
	bool m_PursuitOn = false;
	bool m_ObjectAvoidance = false;
	bool m_WanderingOn = false;

};
