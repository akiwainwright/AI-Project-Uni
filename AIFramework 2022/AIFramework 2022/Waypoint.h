#pragma once
#include "DrawableGameObject.h"


class Waypoint :
    public DrawableGameObject
{
public:
	
    virtual HRESULT init(ID3D11Device* pd3dDevice, bool isOnTrack, bool checkPointID);

    bool    isOnTrack()     { return m_isOnTrack; }
    int     getCheckpointID() { return m_checkpointID; }
	float   distanceToWaypoint(Waypoint* waypoint);

#pragma region Node Data Getters and Setters
    inline float GetNodeFValue() { return m_WaypointFValue;}
    inline void SetNodeFValue() { m_WaypointFValue = m_WaypointGValue + m_WaypointHValue;}

    inline float GetNodeGValue() { return m_WaypointGValue;}
    inline void SetNodeGValue(float gValue) { m_WaypointGValue= gValue;}

    inline float GetNodeHValue() { return m_WaypointHValue; }
    inline void SetNodeHValue(float hValue) { m_WaypointHValue = hValue; }

    inline Waypoint* GetParentWaypoint() { return m_ParentWaypoint; }
    inline void SetParentWaypoint(Waypoint* parent) { m_ParentWaypoint = parent; }
#pragma endregion 

private:
    bool m_isOnTrack; // is the waypoint a 'track' (path) waypoint. If it isn't, it is used to define the corners of buildings (and shouldn't be used for path finding)
    int  m_checkpointID;

    //Pathfinding variables;
    float m_WaypointGValue = 0;
    float m_WaypointHValue = 0;
    float m_WaypointFValue = 0;
    Waypoint* m_ParentWaypoint = nullptr;
};

