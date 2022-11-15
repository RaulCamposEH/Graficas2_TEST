#pragma once

#include "GameModel.h"
#include "Gallina.h"
#include "ColBox.h"
#include "Item.h"

class Trampa {
public:
	GameModel* mTrampaModels[2];
	Camara* mCamera;

	bool activated = false;
	fvec3 position;
	float angleY = 0;

	float colRadio;

	Trampa(GameModel* Models[], Camara* Camara, fvec3 position, float radio) {
		mTrampaModels[0] = Models[0];
		mTrampaModels[1] = Models[1];
		mCamera = Camara;
		this->position = position;
		this->colRadio = radio;
	}

	void Draw(Camara* camara, float scale, float specForce) {
		if (activated) mTrampaModels[1]->Draw(camara, scale, specForce);
		else mTrampaModels[0]->Draw(camara, scale, specForce);
	}

	void Update(Gallina* gallina[], Player* jugador) {
		for (int i = 0; i < 3; i++) {
			bool col = gallina[i]->GetColBox()->CheckSphereColission(position, colRadio);
			if (col) {
				activated = true;
				gallina[i]->SetFallInTrap();
			}
		}
		//jugador->CajaDeColision->CheckSphereColission(position, colRadio);
	}

};