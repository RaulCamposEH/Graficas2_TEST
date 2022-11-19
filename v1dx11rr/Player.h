#pragma once

#include "GameModel.h"
#include "ColBox.h"
#include "Item.h"

class Player {
public:
	GameModel* mPlayerModel;
	Camara* mCamera;
	ColBox* CajaDeColision;
	//Colision del jugador

	fvec3 scale;

	fvec3 position;
	bool itemOnHand;
	bool itemOnInventory;
	bool FPC = true;
	int puntos = 0;

	Player(GameModel* Model, Camara* Camara, fvec3 ColSize) {
		mPlayerModel = Model;
		mCamera = Camara;
		itemOnHand = false;
		itemOnInventory = false;
		position = mPlayerModel->getPos();
		scale = ColSize;
		CajaDeColision = new ColBox(position, ColSize);
	}

	//cambiar entre primera y tercera persona
	void toggleView() {
		FPC = !FPC;
	}

	//en base a una colision obtener el item del juego
	void obtenerItem(Item* item) {
		if (!item->getItemState()) {
			if (CajaDeColision->CheckSphereColission(item->getPos(), item->GetColitionRadio())) {
				item->TakeItem();
				itemOnHand = item->getItemState();
				itemOnInventory = item->getItemState();
			}
		}
	}

	//poner item en la mano del jugador o quitarlo
	void toggleItem() {
		if (itemOnInventory) {
			itemOnHand = !itemOnHand;
		}
	}

	void Draw(Camara* camara, float scale, float specForce) {
		if (!FPC) {
			mPlayerModel->setPos(position);
			mPlayerModel->setAltura(position.y);
			CajaDeColision = CajaDeColision->reposBox(position, this->scale);
			mPlayerModel->Draw(camara, scale, specForce);
		}
	}

	//ejecutar antes de hacer un draw en render
	void Update(fvec3 newpos, const ColArray ColeccionColisiones) {
		//Mover el personaje
		fvec3 posAnterior = GetPos();
		//ColBox CajaAnterior = *CajaDeColision;
		SetPos(newpos);
		
		mPlayerModel->setPos(position);
		mPlayerModel->setAltura(position.y);
		CajaDeColision = CajaDeColision->reposBox(position, this->scale);

		for (auto Colision : ColeccionColisiones)
		{
			if (CajaDeColision->CheckColission(Colision)) 
			{
				SetPos(posAnterior);
			}
		}
	}

	fvec3 GetPos()
	{
		return position;
	}

	void SetPos(fvec3 value) {
		position = value;
	}

	void SetAltura(float altura) {
		position.y = altura;
		mPlayerModel->setAltura(altura);
	}

	void SumPoint() {
		puntos += 1;
	}
};