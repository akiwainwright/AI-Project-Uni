#pragma once
#include "DrawableGameObject.h"

struct Node
{
    float f;
    float g;
    float h;

    Node() { f = 0.0f, g = 0.0f, h = 0.0f;}
    
};

class Waypoint :
    public DrawableGameObject
{
public:
	
    virtual HRESULT init(ID3D11Device* pd3dDevice, bool isOnTrack, bool checkPointID);

    bool    isOnTrack()     { return m_isOnTrack; }
    int     getCheckpointID() { return m_checkpointID; }
	float   distanceToWaypoint(Waypoint* waypoint);

#pragma region Node Data Getters and Setters
    inline float GetNodeFValue() { return m_NodeData.f;}
    inline void SetNodeFValue() { m_NodeData.f = m_NodeData.g + m_NodeData.h;}

    inline float GetNodeGValue() { return m_NodeData.g;}
    inline void SetNodeGValue(float gValue) { m_NodeData.g = gValue;}

    inline float GetNodeHValue() { return m_NodeData.h; }
    inline void SetNodeHValue(float hValue) { m_NodeData.h = hValue; }

    inline Waypoint* GetParentWaypoint() { return m_ParentWaypoint; }
    inline void SetParentWaypoint(Waypoint* parent) { m_ParentWaypoint = parent; }
#pragma endregion 

private:
    bool m_isOnTrack; // is the waypoint a 'track' (path) waypoint. If it isn't, it is used to define the corners of buildings (and shouldn't be used for path finding)
    int  m_checkpointID;

    //Pathfinding variables;
    Node m_NodeData;
    Waypoint* m_ParentWaypoint = nullptr;
};

