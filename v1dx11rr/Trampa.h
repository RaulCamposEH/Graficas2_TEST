#pragma once

#include "GameModel.h"
#include "ColBox.h"
#include "Item.h"

class Trampa {
public:
	GameModel* mTrampaModels[2];
	Camara* mCamera;
	ColBox* CajaDeColision;

	bool activated = false;
	fvec3 position;
	fvec3 scale;
	float angleY = 0;

	Trampa(GameModel* Model1, GameModel* Model2, Camara* Camara, fvec3 ColSize) {
		mTrampaModels[0] = Model1;
		mTrampaModels[1] = Model2;
		mCamera = Camara;
		fvec3 pos = mTrampaModel[0]->getPos();
		pos.y += (ColSize.y / 2);
		scale = ColSize;
		CajaDeColision = new ColBox(pos, scale);
	}

	Trampa(GameModel* Model, Camara* Camara, fvec3 ColSize, float ColRotY) {
		mTrampaModel = Model;
		mCamera = Camara;
		fvec3 pos = mTrampaModel->getPos();
		pos.y += (ColSize.y / 2);
		scale = ColSize;
		CajaDeColision = new ColBox(pos, scale, ColRotY);
	}



	void Draw(Camara* camara, float scale, float specForce) {
		mTrampaModel->Draw(camara, scale, specForce);
	}

	void Update() {

	}

	fvec3 GetPos() { return this->position; }

	void SetPos(fvec3 value) { 
		this->position = value;
		mTrampaModel->setPos(position);
		CajaDeColision = CajaDeColision->reposBox(position, scale, angleY);
	}

	void SetAltura(float altura) {
		this->position.y = altura;
		CajaDeColision = CajaDeColision->reposBox(position, scale, angleY);
	}
};