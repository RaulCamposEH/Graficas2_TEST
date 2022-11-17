#pragma once

#include <string>
#include <vector>
#define CUSTOMFILENAME "trampasPositions.txt"

struct ObjectPositions
{
	float x, y, z;
};

bool saveinFile(std::string File, ObjectPositions position);

bool openandRead(std::string File, std::vector<ObjectPositions>& positions);