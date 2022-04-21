#include "AIManager.h"
#include "Vehicle.h"
#include "DrawableGameObject.h"
#include "PickupItem.h"
#include "Waypoint.h"
#include "main.h"
#include "constants.h"
#include <sstream>

#include "OuputStrings.h"

AIManager::AIManager()
{
	m_pCar = nullptr;
}

AIManager::~AIManager()
{
	release();
}

void AIManager::release()
{
	clearDrawList();

	for (PickupItem* pu : m_pickups)
	{
		delete pu;
	}
	m_pickups.clear();

	delete m_pCar;
	m_pCar = nullptr;
}

HRESULT AIManager::initialise(ID3D11Device* pd3dDevice)
{
    // create the vehicle 
    float xPos = -500; // an abtrirary start point
    float yPos = 300;

    m_pCar = new Vehicle();
    HRESULT hr = m_pCar->initMesh(pd3dDevice, carColour::blueCar);
    m_pCar->setVehiclePosition(Vector2D(xPos, yPos));
    if (FAILED(hr))
        return hr;

    m_pCar2 = new Vehicle();
    hr = m_pCar2->initMesh(pd3dDevice, carColour::redCar);
    m_pCar2->setVehiclePosition(Vector2D(400, -250));

    m_pCar3 = new Vehicle();
    hr = m_pCar3->initMesh(pd3dDevice, carColour::redCar);
    m_pCar3->setVehiclePosition(Vector2D(0.0f, 0.0f));

    m_pCar4 = new Vehicle();
    hr = m_pCar4->initMesh(pd3dDevice, carColour::redCar);
    m_pCar4->setVehiclePosition(Vector2D(450.0f, 300.0f));
    m_pCar4->SetVelocity(Vector2D(0.0f, 0.0f));

    // setup the waypoints
    m_waypointManager.createWaypoints(pd3dDevice);
    m_pCar->GetPathfinder()->SetWaypointManager(&m_waypointManager);

    // create a passenger pickup item
    PickupItem* pPickupPassenger = new PickupItem();
    hr = pPickupPassenger->initMesh(pd3dDevice, pickuptype::Passenger);
    m_pickups.push_back(pPickupPassenger);

    // NOTE!! for fuel and speedboost - you will need to create these here yourself
    PickupItem* pPickupFuel = new PickupItem();
    hr = pPickupFuel->initMesh(pd3dDevice, pickuptype::Fuel);
    m_pickups.push_back(pPickupFuel);

    PickupItem* pPickupSpeedBoost = new PickupItem();
    hr = pPickupSpeedBoost->initMesh(pd3dDevice, pickuptype::SpeedBoost);
    m_pickups.push_back(pPickupSpeedBoost);
    
    // (needs to be done after waypoint setup)
    setRandomPickupPosition(pPickupPassenger);
    setRandomPickupPosition(pPickupFuel);
    setRandomPickupPosition(pPickupSpeedBoost);

    m_pCar->SetVehicleMode(Mode::Pathfinding);
    m_pCar->GetPathfinder()->SetWaypointManager(&m_waypointManager);
    m_pCar->setPosition(m_waypointManager.getNearestWaypoint(m_pCar->getPosition())->getPosition());
    m_pCar->SetPickupTarget(m_pickups[2]);
    m_pCar->SetPathfindDestination(m_waypointManager.getNearestWaypoint(m_pCar->GetPickupTarget()->getPosition()));

    m_pCar->SetPursuitTarget(m_pCar2);
    m_pCar->SetAvoidTarget(m_pCar3);
    m_pCar->SetFleeTarget(m_pCar4);

    m_pCar2->SetFleeTarget(m_pCar);
    m_pCar2->ToggleFlee(true);
    m_pCar2->ToggleWander(true);

    m_pCar4->SetPursuitTarget(m_pCar);
    m_pCar4->TogglePursuit(true);

    return hr;
}


void AIManager::update(const float fDeltaTime)
{
    //Draw A* Path
    if(m_pCar->GetPathfinder()->GetTraverseState())
    {
        for(Waypoint* waypoint : m_pCar->GetPathfinder()->CalculatedPath())
        {
            AddItemToDrawList(waypoint);
        }
    }
    
    
    {
        for (unsigned int i = 0; i < m_waypointManager.getWaypointCount(); i++) {
            m_waypointManager.getWaypoint(i)->update(fDeltaTime);
            if(m_pCar->GetVehicleMode() == Mode::Steering)
            {
                AddItemToDrawList(m_waypointManager.getWaypoint(i)); // if you uncomment this, it will display the waypoints
            }
        }
    }
    for (int i = 0; i < m_waypointManager.getQuadpointCount(); i++)
    {
        Waypoint* qp = m_waypointManager.getQuadpoint(i);
        qp->update(fDeltaTime);
        //AddItemToDrawList(qp); // if you uncomment this, it will display the quad waypoints
    }

    // update and display the pickups
    for (unsigned int i = 0; i < m_pickups.size(); i++) {
        m_pickups[i]->update(fDeltaTime);
        AddItemToDrawList(m_pickups[i]);
    }

	// draw the waypoints nearest to the car
	/*
    Waypoint* wp = m_waypointManager.getNearestWaypoint(m_pCar->getPosition());
	if (wp != nullptr)
	{
		vecWaypoints vwps = m_waypointManager.getNeighbouringWaypoints(wp);
		for (Waypoint* wp : vwps)
		{
			AddItemToDrawList(wp);
		}
	}*/

    

    // update and draw the car (and check for pickup collisions)
	if (m_pCar != nullptr)
	{
		m_pCar->update(fDeltaTime);
		checkForCollisions();
		AddItemToDrawList(m_pCar);
	}

    if(m_pCar->GetVehicleMode() == Mode::Steering)
    {
        if (m_pCar2 != nullptr)
        {
            m_pCar2->update(fDeltaTime);
            checkForCollisions();
            AddItemToDrawList(m_pCar2);
        }

        if (m_pCar3 != nullptr)
        {
            m_pCar3->update(fDeltaTime);
            checkForCollisions();
            AddItemToDrawList(m_pCar3);
        }

        if(m_pCar->GetFleeState())
        {
            if (m_pCar4 != nullptr)
            {
                m_pCar4->update(fDeltaTime);
                checkForCollisions();
                AddItemToDrawList(m_pCar4);
            }
        }
    }

    if (m_pCar->GetVehicleMode() == Mode::Pathfinding)
    {
        switch (m_pCar->GetPathfinder()->GetCurrentState())
        {
            case VehicleState::GetSpeedBoost:
            {
                m_pCar->SetPickupTarget(m_pickups[2]);
                break;
            }
            case VehicleState::GetFuel:
            {
                m_pCar->SetPickupTarget(m_pickups[1]);
                break;
            }
            case VehicleState::GetPassenger:
            {
                m_pCar->SetPickupTarget(m_pickups[0]);
                break;
            }
            case VehicleState::GetFuelImmediately:
            {
                m_pCar->SetPickupTarget(m_pickups[1]);
                if (m_pCar->GetPathfinder()->GetTraverseState())
                {
                    m_pCar->GetPathfinder()->StopTraversingPath();
                    m_pCar->GetPathfinder()->ResetCurrentWaypoint();
                    m_pCar->GetPathfinder()->CalculatePath();
                    m_pCar->GetPathfinder()->IsGettingFuel();
                    //m_pCar->GetPathfinder()->ResetState();
                }
                break;
            }
        }
    }
}

void AIManager::mouseUp(int x, int y)
{
	// get a waypoint near the mouse click, then set the car to move to the this waypoint
	Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
	if (wp == nullptr)
		return;

    // steering mode
    m_pCar->SetTargetPosition(wp->getPosition());    
}

void AIManager::keyUp(WPARAM param)
{
    const WPARAM key_a = 65;
    switch (param)
    {
        case key_a:
        {
            //OutputDebugStringA("a Up \n");
            break;
        }
    }
}

void AIManager::keyDown(WPARAM param)
{
	// hint 65-90 are a-z
	//const WPARAM key_a = 0x41;
	//const WPARAM key_s = 0x53;
	//const WPARAM key_p = 0x50;
	//const WPARAM key_o = 0x4F;
    //const WPARAM key_t = 84;

    //Windows VM Keys
    const WPARAM TAB_KEY = 0x09;
    const WPARAM C_KEY = 0x43;
    const WPARAM S_KEY = 0x53;
    const WPARAM A_KEY = 0x41;
    const WPARAM F_KEY = 0x46;
    const WPARAM P_KEY = 0x50;
    const WPARAM W_KEY = 0x57;
    const WPARAM O_KEY = 0x4f;
    const WPARAM T_KEY = 0x54;

    switch (m_pCar->GetVehicleMode())
    {

        case Mode::Steering:
#pragma region Steering Mode Keyboard input
        {
            switch(param)
            {
                case TAB_KEY:
                {
                    m_pCar->SetVehicleMode(Mode::Pathfinding);
                    m_pCar->GetPathfinder()->SetTraverseState(false);
                    m_pCar->GetPathfinder()->ResetCurrentWaypoint();
                    OutputDebugStringA("Switched to Pathfinding Mode\n");
                    m_pCar->ResetSteeringBehaviours();
                    break;
                }
                case C_KEY: //Input to display Current Active Steering Behaviours
                {
                    m_pCar->OutputCurrentModes();
                    break;
                }
                case S_KEY: //Input Key for toggling Seek Behaviour
                {
                    if (!m_pCar->GetSeekState())
                    {
                        m_pCar->ToggleSeek(true);
                        m_pCar->ToggleArrive(false);
                        m_pCar->ToggleWander(false);
                        m_pCar->TogglePursuit(false);
                        m_pCar->OutputCurrentModes();
                    }
                    else
                    {
                        m_pCar->ToggleSeek(false);
                        m_pCar->OutputCurrentModes();
                    }
                    break;
                }
                case A_KEY: //Input Key for toggling Arrive Behaviour
                {
                    if (!m_pCar->GetArriveState())
                    {
                        m_pCar->ToggleSeek(false);
                        m_pCar->ToggleArrive(true);
                        m_pCar->ToggleWander(false);
                        m_pCar->TogglePursuit(false);
                        m_pCar->OutputCurrentModes();
                    }
                    else
                    {
                        m_pCar->ToggleArrive(false);
                        m_pCar->OutputCurrentModes();
                    }
                    break;
                }
                case F_KEY: //Input key for toggling Flee Behaviour
                {
                    if (!m_pCar->GetFleeState())
                    {
                        m_pCar->ToggleFlee(true);
                        m_pCar->OutputCurrentModes();
                    }
                    else
                    {
                        m_pCar->ToggleFlee(false);
                        m_pCar->OutputCurrentModes();
                    }
                    break;
                }
                case P_KEY: //Input Key for Toggling Pursuit Behaviour
                {
                    if (!m_pCar->GetPursuitState())
                    {
                        m_pCar->TogglePursuit(true);
                        m_pCar->ToggleSeek(false);
                        m_pCar->ToggleArrive(false);
                        m_pCar->ToggleWander(false);
                        m_pCar->OutputCurrentModes();
                    }
                    else
                    {
                        m_pCar->TogglePursuit(false);
                        m_pCar->OutputCurrentModes();
                    }
                    break;
                }
                case W_KEY: //Input key for Toggling Wander Behaviour
                {
                    if (!m_pCar->GetWanderState())
                    {
                        m_pCar->ToggleSeek(false);
                        m_pCar->ToggleArrive(false);
                        m_pCar->ToggleWander(true);
                        m_pCar->OutputCurrentModes();
                    }
                    else
                    {
                        m_pCar->ToggleWander(false);
                        m_pCar->OutputCurrentModes();
                    }
                    break;
                }
                case O_KEY: //Input key for Toggling Object Avoidance
                {
                    if (!m_pCar->GetObjectAvoidanceState())
                    {
                        m_pCar->ToggleObjectAvoidance(true);
                        m_pCar->OutputCurrentModes();
                    }
                    else
                    {
                        m_pCar->ToggleObjectAvoidance(false);
                        m_pCar->OutputCurrentModes();
                    }
                    break;
                }
                case T_KEY: //Input key for displaying target position
                {
                    OutputDebugStringA("Target Position: ");
                    OuputStrings::OutputVector(m_pCar->GetTargetPosition());
                    break;
                }

            }
            break;
        }
#pragma endregion
        case Mode::Pathfinding:
#pragma region Pathfinding Mode Keyboarud Input
        {
            switch (param)
            {
                case TAB_KEY:
                    m_pCar->SetVehicleMode(Mode::Steering);
                m_pCar->SetVelocity(Vector2D(0.0f, 0.0f));
                    OutputDebugStringA("Switched to Steering Behaviours\n");
                    m_pCar->OutputCurrentModes();
                    break;
            }
            break;
        }
#pragma endregion
    }
}

void AIManager::setRandomPickupPosition(PickupItem* pickup)
{
    if (pickup == nullptr)
        return;

    int x = (rand() % SCREEN_WIDTH) - (SCREEN_WIDTH / 2);
    int y = (rand() % SCREEN_HEIGHT) - (SCREEN_HEIGHT / 2);

    Waypoint* wp = m_waypointManager.getNearestWaypoint(Vector2D(x, y));
    if (wp) {
        pickup->setPosition(wp->getPosition());
    }
}

/*
// IMPORTANT
// hello. This is hopefully the only time you are exposed to directx code 
// you shouldn't need to edit this, but marked in the code below is a section where you may need to add code to handle pickup collisions (speed boost / fuel)
// the code below does the following:
// gets the *first* pickup item "m_pickups[0]"
// does a collision test with it and the car
// creates a new random pickup position for that pickup

// the relevant #includes are already in place, but if you create your own collision class (or use this code anywhere else) 
// make sure you have the following:
#include <d3d11_1.h> // this has the appropriate directx structures / objects
#include <DirectXCollision.h> // this is the dx collision class helper
using namespace DirectX; // this means you don't need to put DirectX:: in front of objects like XMVECTOR and so on. 
*/

bool AIManager::checkForCollisions()
{
    if (m_pickups.size() == 0)
        return false;

    XMVECTOR dummy;

    // get the position and scale of the car and store in dx friendly xmvectors
    XMVECTOR carPos;
    XMVECTOR carScale;
    XMMatrixDecompose(
        &carScale,
        &dummy,
        &carPos,
        XMLoadFloat4x4(m_pCar->getTransform())
    );

    // create a bounding sphere for the car
    XMFLOAT3 scale;
    XMStoreFloat3(&scale, carScale);
    BoundingSphere boundingSphereCar;
    XMStoreFloat3(&boundingSphereCar.Center, carPos);
    boundingSphereCar.Radius = scale.x;

    // do the same for a pickup item
    // a pickup - !! NOTE it is only referring the first one in the list !!
    // to get the passenger, fuel or speedboost specifically you will need to iterate the pickups and test their type (getType()) - see the pickup class  

    BoundingSphere pickupCollisionSphere[3];

    for (int i = 0; i < m_pickups.size(); ++i)
    {
        XMVECTOR puPos;
        XMVECTOR puScale;
        XMMatrixDecompose(
            &puScale,
            &dummy,
            &puPos,
            XMLoadFloat4x4(m_pickups[i]->getTransform())
        );

        // bounding sphere for pickup item
        XMStoreFloat3(&scale, puScale);
        BoundingSphere boundingSpherePU;
        XMStoreFloat3(&boundingSpherePU.Center, puPos);
        boundingSpherePU.Radius = scale.x;

        pickupCollisionSphere[i] = boundingSpherePU;
    }
    

	// THIS IS generally where you enter code to test each type of pickup
    // does the car bounding sphere collide with the pickup bounding sphere?
    for (int i = 0; i < m_pickups.size(); ++i)
    {
        if (boundingSphereCar.Intersects(pickupCollisionSphere[i]))
        {
            pickuptype Type = m_pickups[i]->getType();

            switch (Type)
            {
            case pickuptype::Fuel:
                //OutputDebugStringA("Collected Fuel\n");
                m_pickups[1]->hasCollided();
                setRandomPickupPosition(m_pickups[1]);
                m_pCar->GetPathfinder()->ResetFuel();
                m_pCar->GetPathfinder()->GotFuel();

                m_pCar->SetPathfindDestination(m_waypointManager.getNearestWaypoint(m_pCar->GetPickupTarget()->getPosition()));
                break;
            case pickuptype::SpeedBoost:
                //OutputDebugStringA("Collected SpeedBoost\n");
                m_pickups[2]->hasCollided();
                setRandomPickupPosition(m_pickups[2]);
                m_pCar->GetPathfinder()->ResetBoostTimer();
                m_pCar->GetPathfinder()->SwitchBoostOn();

                m_pCar->SetPathfindDestination(m_waypointManager.getNearestWaypoint(m_pCar->GetPickupTarget()->getPosition()));
                break;
            case pickuptype::Passenger:
                //OutputDebugStringA("Collected Passenger\n");
                m_pickups[0]->hasCollided();
                setRandomPickupPosition(m_pickups[0]);

                m_pCar->SetPathfindDestination(m_waypointManager.getNearestWaypoint(m_pCar->GetPickupTarget()->getPosition()));
                break;
            default:
                break;
            }
        }
    }



    return false;
}





