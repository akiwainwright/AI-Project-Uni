#include "OuputStrings.h"
#include <iostream>
#include <sstream>

void OuputStrings::OutputVector(Vector2D vec)
{
    std::stringstream ss;
    ss << "X: " << vec.x << " | Y: " << vec.y << "\n";
    OutputDebugStringA(ss.str().c_str());
}
