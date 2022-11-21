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
	int seguidoPor = 0;
	int vidas = 3;
	bool invulnerable = false;
	float tiempo_inv = 0.0f;
	bool colisionando = false;

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

	void getDamage() {
		if (vidas != 0) {
			bool recibirdamage = !invulnerable;
			if (recibirdamage) {
				vidas -= 1;
				invulnerable = !invulnerable;
				tiempo_inv = 10.0f;
			}
		}
	}
	/*
		if (vidas != 0) {
		bool colision = Jugador->CajaDeColision->CheckSphereColission(chickenOne->GetPos(), chickenOne->GetRadio());
		bool recibirdamage = !invulnerable;

		chickenTwo->Update(Jugador, pos);
		chickenTwo->SetAltura(terreno->Superficie(pos.x, pos.z));
			if (colision && recibirdamage) {
				vidas -= 1;
				invulnerable = !invulnerable;
				tiempo_inv = 10.0f;
			}
			if (!recibirdamage) {
				tiempo_inv -= 0.08f;
			}
			if (tiempo_inv <= 0.0f) {
				invulnerable = false;
				tiempo_inv = 0.0f;
			}
		}

	*/
	//en base a una colision obtener el item del juego
	void obtenerItem(Item* item, bool &result) {
		if (!item->getItemState()) {
			if (CajaDeColision->CheckSphereColission(item->getPos(), item->GetColitionRadio())) {
				item->TakeItem();
				itemOnHand = item->getItemState();
				itemOnInventory = item->getItemState();
				result = true;
			}
		}
		else {
			result = false;
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
			XMFLOAT3 m_lightPos(0.0f, 0.0f, 0.0f);

			mPlayerModel->setPos(position);
			mPlayerModel->setAltura(position.y);
			CajaDeColision = CajaDeColision->reposBox(position, this->scale);
			mPlayerModel->Draw(camara, scale, specForce, m_lightPos);
		}
	}

	//ejecutar antes de hacer un draw en render
	void Update(fvec3& newpos, const ColArray& ColeccionColisiones, const RadioColArray& RadioColisiones) {
		//Mover el personaje

		fvec3 posAnterior = GetPos();
		ColBox CajaAnterior = *CajaDeColision;
		if (vidas == 0)
		{
			newpos = posAnterior;
			return;
		}
		SetPos(newpos);
		if (invulnerable) {
			tiempo_inv -= 0.08f;
			if (tiempo_inv <= 0.0f) {
				invulnerable = false;
				tiempo_inv = 0.0f;
			}
		}

		CajaDeColision = CajaDeColision->reposBox(newpos, this->scale);
		for (ColBox Colision : ColeccionColisiones)
		{
			if (CajaDeColision->CheckColission(Colision))
			{
				SetPos(posAnterior);
				CajaDeColision = CajaDeColision->reposBox(posAnterior, this->scale);
				newpos = posAnterior;
			}
		}
		for (fvec3 RadioCol : RadioColisiones) {
			if (CajaDeColision->CheckSphereColission(RadioCol, RadioCol.y))
			{
				SetPos(posAnterior);
				CajaDeColision = CajaDeColision->reposBox(posAnterior, this->scale);
				newpos = posAnterior;
			}
		}
	}

	fvec3 GetPos()
	{
		return position;
	}

	int GetSeguimientoGallina() {
		return seguidoPor;
	}

	void SetPos(fvec3 value) {
		position = value;
	}

	void SetAltura(float altura) {
		position.y = altura;
	}

	void SumPoint() {
		puntos += 1;
	}

	void SetGallina(int id) {
		seguidoPor = id;
	}
};