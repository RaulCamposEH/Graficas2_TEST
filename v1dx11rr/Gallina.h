#pragma once
#include "Player.h"

class Gallina {
private:
	GameModel* mChickenModel;
	fvec3 scale;
	float radioDetection;

	fvec3 position;
	
	ColBox* CajaDeColision;
	float anguloMira;
	bool traped = false;

public:
	Gallina(GameModel* model, fvec3 escala, float radio) 
	{
		mChickenModel = model;
		scale = escala;
		radioDetection = radio;

		position = mChickenModel->getPos();
		
		CajaDeColision = CajaDeColision->reposBox(position, fvec3(5.0f, 5.0f, 5.0f));
		anguloMira = 0.0f;
	}	

	~Gallina() {
		
	}

	fvec3 Seguir(Player* jugador) {
		fvec3 player_pos = jugador->GetPos();
		fvec3 actual_pos = position;
		bool enRango = sqrt(pow((actual_pos.x - player_pos.x), 2) + pow((actual_pos.z - player_pos.z), 2)) < radioDetection;
		if (!traped) 
		{
			if (enRango)
			{
				if (jugador->itemOnHand) 
				{
					giroGallina(anguloMira, player_pos.z, player_pos.x, position.z, position.x);
					//seguir al jugador
					float Vx = player_pos.x - actual_pos.x;
					float Vz = player_pos.z - actual_pos.z;
					float mag = sqrt(pow(Vx, 2) + pow(Vz, 2));
					if (mag == 0) {
						mag = 0.1;
					}
					Vx /= mag;
					Vz /= mag;

					fvec3 newpos = actual_pos;
					newpos.x += Vx / 4;
					newpos.z += Vz / 4;
					return newpos;
				}
			}
		}
		return actual_pos;
	}

	void Update(Player* jugador, fvec3& pos) {
		pos = Seguir(jugador);
		SetPos(pos);
	}

	void Draw(Camara* camara, float scale, float specForce) {
		mChickenModel->setPos(position);
		mChickenModel->setAltura(position.y);
		CajaDeColision = CajaDeColision->reposBox(position, fvec3(5.0f, 5.0f, 5.0f));
		mChickenModel->setYRot(anguloMira);
		mChickenModel->Draw(camara, scale, specForce);
	}

	void giroGallina(float& angulo, float jugadorZ, float jugadorX, float gallinaZ, float gallinaX) {
		if (jugadorZ < gallinaZ) {
			float auxX = gallinaX - jugadorX;

			float auxz = gallinaZ - jugadorZ;

			angulo = atan(auxX / auxz) - 3.14159f;
		}

		if (jugadorZ >= gallinaZ) {
			float auxX = jugadorX - gallinaX;

			float auxz = jugadorZ - gallinaZ;

			angulo = atan(auxX / auxz);
		}

	}
	
	void SetAltura(float altura, bool repos = false) {
		position.y = altura;
		mChickenModel->setAltura(position.y);
	}
	void SetPos(fvec3 pos, bool repos = false) {
		position = pos;
	}

	void SetFallInTrap() { traped = true; }

	fvec3 GetPos() { return this->position; }
	ColBox* GetColBox() { return CajaDeColision; }
	void GetPos(fvec3& pos) { pos = position; }
	float GetRadio() { return radioDetection; }
};