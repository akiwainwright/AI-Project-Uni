#include "Vehicle.h"
#include "OuputStrings.h"
#include "constants.h"
#include <sstream>
#include <string>

#define NORMAL_MAX_SPEED 200


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
	m_Mass = 3.0f;
	
	m_Pathfinding = new Pathfinding(this);

	m_VehicleMode = Mode::Steering;
	
	return hr;
}

void Vehicle::update(const float deltaTime)
{
	switch (m_VehicleMode)
	{
		case Mode::Steering:
		{
			m_Steering->SteeringUpdate();

			m_Velocity = m_Velocity + (2 * m_Acceleration * deltaTime);
			m_Velocity.Truncate(MAX_SPEED);

			#pragma region Keeping Car On Screen
			if (m_currentPosition.x > SCREEN_WIDTH / 2)
			{
				m_currentPosition.x -= SCREEN_WIDTH;
			}
			else if (m_currentPosition.x < -SCREEN_WIDTH / 2)
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

			MoveCar();
				
			break;
		}
		case Mode::Pathfinding:
		{
			m_Pathfinding->Update(deltaTime);

			break;
		}
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

Vehicle::~Vehicle()
{
	delete m_waypointManager;
	m_waypointManager = nullptr;

	m_FleeTarget = nullptr;

	m_PursuitTarget = nullptr;

	m_AvoidTarget = nullptr;
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

void Vehicle::ResetSteeringBehaviours()
{
	m_SeekOn = false;
	m_ArriveOn = false;
	m_FleeOn = false;
	m_PursuitOn = false;
	m_WanderingOn = false;
	m_ObjectAvoidanceOn = false;
}

void Vehicle::OutputCurrentModes()
{
	string OutputMessage = "Active Behaviours: ";

	if (!m_SeekOn && !m_ArriveOn && !m_FleeOn && !m_PursuitOn && !m_WanderingOn && !m_ObjectAvoidanceOn)
	{
		OutputDebugStringA("No Active Steering Behaviours\n");
		return;
	}

	if (m_SeekOn)
	{
		string Mode = "Seek ";
		OutputMessage += Mode;
	}

	if (m_ArriveOn)
	{
		string Mode = "Arrive ";
		OutputMessage += Mode;
	}

	if (m_FleeOn)
	{
		string Mode = "Flee ";
		OutputMessage += Mode;
	}

	if (m_PursuitOn)
	{
		string Mode = "Pursuit ";
		OutputMessage += Mode;
	}

	if (m_WanderingOn)
	{
		string Mode = "Wander ";
		OutputMessage += Mode;
	}

	if (m_ObjectAvoidanceOn)
	{
		string Mode = "ObjectAvoidance ";
		OutputMessage += Mode;
	}

	OutputMessage += "\n";
	OutputDebugStringA(OutputMessage.c_str());
}


