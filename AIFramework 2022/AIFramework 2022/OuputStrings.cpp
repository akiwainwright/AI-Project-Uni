#include "OuputStrings.h"
#include <iostream>
#include <sstream>

void OuputStrings::OutputVector(Vector2D vec)
{
    std::stringstream ss;
    ss << "(" << vec.x << " , " << vec.y << ")\n";
    OutputDebugStringA(ss.str().c_str());
}

void OuputStrings::OutputRemainingDistance(Vector2D current, Vector2D target)
{
    std::stringstream ss;
    float remainingDistance = (target - current).Length();
    ss << "Remaining Distance: " << remainingDistance << "\n";
    OutputDebugStringA(ss.str().c_str());
}
