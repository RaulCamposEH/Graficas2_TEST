#pragma once

#include "GameModel.h"
#include "ColBox.h"

class Player {
public:
	GameModel* mPlayerModel;
	Camara* mCamera;
	ColBox* CajaDeColision;
	//Colision del jugador

	bool comida;
	bool itemOnHand;

	Player(GameModel* Model, Camara* Camara, fvec3 ColSize) {
		mPlayerModel = Model;
		mCamera = Camara;
		comida = false;
		itemOnHand = false;
		fvec3 playerpos = mPlayerModel->getPos();
		playerpos.y += (ColSize.y / 2);
		CajaDeColision = new ColBox(playerpos, ColSize);
	}

	//cambiar entre primera y tercera persona
	void toggleView() {}

	//en base a una colision obtener el item del juego
	void obtenerItem() {}

	//poner item en la mano del jugador o quitarlo
	void toggleItem() {}

	void Draw() {}

};