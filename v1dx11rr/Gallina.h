#pragma once
#include "Player.h"
class Gallina {
public:
	Gallina();
	~Gallina();

	void Seguir(Player* jugador);

	void Caminar();
};