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
		position = model->getPos();

		ColitionRadio = radio;
		scale = fvec3(0,0,0);
		tomado = false;
	}

	~Item() {
	
	}

	void Update() {

	}

	void Draw(Camara* camara, float _scale, float specForce) {
		if (!tomado) {
			this->mItemModel->Draw(camara, _scale, specForce);
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
		position = pos;
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

	fvec3 getPos() {
		return position;
	}
private:
	GameModel* mItemModel;
	fvec3 scale;
	ColBox* BoxColision;
	float ColitionRadio;
	bool tomado;
	fvec3 position;
};