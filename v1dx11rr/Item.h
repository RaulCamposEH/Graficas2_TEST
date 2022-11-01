#pragma once

#include "GameModel.h"
#include "ColBox.h"

class Item {
public:
	Item(GameModel* model, fvec3 _scale) {
		this->mItemModel = model;
		this->scale = _scale;
		this->BoxColision = new ColBox(mItemModel->getPos(), scale);
		tomado = false;
	}

	Item(GameModel* model, float radio) {
		this->mItemModel = model;
		ColitionRadio = radio;
		scale = fvec3(0,0,0);
		tomado = false;
	}

	~Item() {
	
	}

	void Update() {

	}

	void Draw(Camara* camara, float scale, float specForce) {
		if (!tomado) {
			this->mItemModel->Draw(camara, 'A', 0, scale, specForce);
		}
	}

	void TakeItem() {
		tomado = true;
	}

	void RespawnItem() {
		tomado = false;
	}

	bool getItemState() {
		return tomado;
	}

	float GetColitionRadio() {
		return ColitionRadio;
	}

	ColBox GetColitionBox() {
		return *BoxColision;
	}

	void SetPos(fvec3 pos) {
		mItemModel->setPos(pos);
	}

	void SetAltura(float altura) {
		mItemModel->setAltura(altura);
	}

	float getX() {
		return mItemModel->getX();
	}
	float getZ() {
		return mItemModel->getZ();
	}
private:
	GameModel* mItemModel;
	fvec3 scale;
	ColBox* BoxColision;
	float ColitionRadio;
	bool tomado;
};