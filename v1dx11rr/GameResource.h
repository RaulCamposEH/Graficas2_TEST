#pragma once
#include <string>;

enum class resType {
	Image,
	Model,
	Shader
};

class GameResource
{
	const wchar_t *path;
	resType type;

};

