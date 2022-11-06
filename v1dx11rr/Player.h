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
	bool itemOnHand;
	bool itemOnInventory;
	bool FPC = true;

	Player(GameModel* Model, Camara* Camara, fvec3 ColSize) {
		mPlayerModel = Model;
		mCamera = Camara;
		itemOnHand = false;
		itemOnInventory = false;
		fvec3 playerpos = mPlayerModel->getPos();
		playerpos.y += (ColSize.y / 2);
		scale = ColSize;
		CajaDeColision = new ColBox(playerpos, ColSize);
	}

	//cambiar entre primera y tercera persona
	void toggleView() {
		FPC = !FPC;
	}

	//en base a una colision obtener el item del juego
	void obtenerItem(Item* item) {
		if (!item->getItemState()) 
		{
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
		if (!FPC)
			mPlayerModel->Draw(camara, scale, specForce);
	}

	//ejecutar antes de hacer un draw en render
	void Update(fvec3 newpos, const ColArray ColeccionColisiones) {
		//Mover el personaje
		fvec3 posAnterior = GetPos();
		//ColBox CajaAnterior = *CajaDeColision;
		SetPos(newpos);
		
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
		return mPlayerModel->getPos();
	}

	void SetPos(fvec3 value) {
		mPlayerModel->setPos(value);
		CajaDeColision = CajaDeColision->reposBox(value, scale);
	}

	void SetAltura(float altura) {
		this->mPlayerModel->setAltura(altura);
	}
};