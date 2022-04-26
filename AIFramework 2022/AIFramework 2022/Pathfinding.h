#pragma once
#include "Waypoint.h"
#include "WaypointManager.h"

enum class VehicleState
{
	GetPassenger,
	GetFuel,
	GetFuelImmediately,
	GetSpeedBoost
};

class Pathfinding
{
private:
	bool m_isTraversingPath = false;
	bool m_pathHasBeenReset = false;
	bool m_isFindingPath = false;
	bool m_hasSpeedBoost = false;

	float m_maxFuel = 85.0f;
	float m_fuelLossPerFrame = 5.0f;
	float m_fuel;
	float m_speedMultiplier = 2.5f;
	float m_speedBoostTimerStart = 15.0f;
	float m_speedBoostCountdown;
	int m_CurrentTargetWaypoint = 0;
	
	class Vehicle* m_car;
	WaypointManager* m_wpm;

	vecWaypoints m_CalculatedPath{};

	vecWaypoints m_FreeWaypoints;
	vecWaypoints m_CheckedWaypoints;

	Waypoint* m_startWaypoint;
	Waypoint* m_endWaypoint;
	Waypoint* m_currentWaypoint;

	VehicleState m_currentState;

public:

	Pathfinding(Vehicle* car);
	~Pathfinding();

	void Update(float deltaTime);
	void ResetPath();
	void CalculatePath();
	void FindWaypointWithLowestF();
	void MoveNodeFromFreeToChecked();
	void GeneratePath();
	void TraversePath();
	void ResetWaypointPathData();

	void SetWaypointManager(WaypointManager* wpm) { m_wpm = wpm;}

	bool CheckNodeHasAlreadyBeenChecked(Waypoint* wp);

	inline bool GetTraverseState() { return m_isTraversingPath;}
	inline void SetTraverseState(bool state) { m_isTraversingPath = state; }
	inline vecWaypoints CalculatedPath() { return m_CalculatedPath;}

	inline float GetRemainingFuel() { return m_fuel; }

	inline VehicleState GetCurrentState() { return m_currentState;  }

	inline void ResetFuel() { m_fuel = m_maxFuel; }

	inline void SwitchBoostOn() { m_hasSpeedBoost = true; }
	inline void ResetBoostTimer() { m_speedBoostCountdown = m_speedBoostTimerStart; }

	inline void StopTraversingPath() { m_isTraversingPath = false; }

	inline void ResetCurrentWaypoint() { m_CurrentTargetWaypoint = 0; }
	inline void ResetState() { m_currentState = VehicleState::GetPassenger; }

	inline void GetFuel() { m_currentState = VehicleState::GetFuel; }
	void SetBaseStates();
};

