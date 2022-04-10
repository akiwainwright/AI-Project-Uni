#include "Vehicle.h"
#include "OuputStrings.h"
#include "constants.h"
#include <sstream>
#include <string>

#define NORMAL_MAX_SPEED 200
#define MAX_SPEED 3.0f

HRESULT	Vehicle::initMesh(ID3D11Device* pd3dDevice, carColour colour)
{
	m_scale = XMFLOAT3(30, 20, 1);

	if (colour == carColour::redCar)
	{
		setTextureName(L"Resources\\car_red.dds");
	}
	else if (colour == carColour::blueCar)
	{
		setTextureName(L"Resources\\car_blue.dds");
	}

	HRESULT hr = DrawableGameObject::initMesh(pd3dDevice);

	m_maxSpeed = NORMAL_MAX_SPEED;
	m_currentSpeed = m_maxSpeed;
	setVehiclePosition(Vector2D(0, 0));

	m_lastPosition = Vector2D(0, 0);

	m_Steering = new SteeringBehaviours(this);
	m_Mass = 1.0f;
	

	return hr;
}

void Vehicle::update(const float deltaTime)
{
#pragma region Steering Behaviour Based Movement
	OuputStrings::OutputVector(m_targetPosition);
	if (m_SeekOn || m_ArriveOn || m_FleeOn || m_PursuitOn || m_ObjectAvoidance || m_WanderingOn)
	{
		m_Steering->SteeringUpdate();

		m_Velocity = m_Velocity + m_Acceleration * deltaTime;

		if(m_Velocity.Length() > MAX_SPEED)
		{
			m_ClampedVelocity = ClampVector(m_Velocity, MAX_SPEED);
			m_currentPosition += m_ClampedVelocity;
		}
		else
		{
			m_ClampedVelocity = m_Velocity;
			m_currentPosition += m_ClampedVelocity;	
		}

#pragma region Keeping Car On Screen
		if (m_currentPosition.x > SCREEN_WIDTH / 2)
		{
			m_currentPosition.x -= SCREEN_WIDTH;
		}
		else if(m_currentPosition.x < -SCREEN_WIDTH / 2)
		{
			m_currentPosition.x += SCREEN_WIDTH;
		}

		if (m_currentPosition.y > SCREEN_HEIGHT / 2)
		{
			m_currentPosition.y -= SCREEN_HEIGHT;
		}
		else if (m_currentPosition.y < -SCREEN_HEIGHT / 2)
		{
			m_currentPosition.y += SCREEN_HEIGHT;
		}
#pragma  endregion 
		
		m_Steering->ResetSteerForce();
	}

	

	// rotate the object based on its last & current position
	Vector2D diff = m_currentPosition - m_lastPosition;
	if (diff.Length() > 0) { // if zero then don't update rotation
		diff.Normalize();
		m_radianRotation = atan2f((float)diff.y, (float)diff.x); // this is used by DrawableGameObject to set the rotation
	}
	m_lastPosition = m_currentPosition;

	// set the current poistion for the drawablegameobject
	setPosition(Vector2D(m_currentPosition));

	DrawableGameObject::update(deltaTime);
}


// a ratio: a value between 0 and 1 (1 being max speed)
void Vehicle::setCurrentSpeed(const float speed)
{
	m_currentSpeed = m_maxSpeed * speed;
	m_currentSpeed = max(0, m_currentSpeed);
	m_currentSpeed = min(m_maxSpeed, m_currentSpeed);
}

// set a position to move to
void Vehicle::setPositionTo(Vector2D position)
{
	m_startPosition = m_currentPosition;
	
	m_positionTo = position;
}

// set the current position
void Vehicle::setVehiclePosition(Vector2D position)
{
	m_currentPosition = position;
	m_positionTo = position;
	m_startPosition = position;
	setPosition(position);
}

void Vehicle::setWaypointManager(WaypointManager* wpm)
{
	m_waypointManager = wpm;
}


