#pragma once
#include "Player.h"

class Gallina {
public:
	Gallina(GameModel* model, fvec3 escala, float radio) {
		altura = 0.0f;
		anguloMira = 0.0f;

		mChickenModel = model;
		radioDetection = radio;
		fvec3 pos = model->getPos();
		pos.y += (escala.y / 2);
		position = pos;
		scale = escala;
		CajaDeColision = new ColBox(position, scale);
	}	

	~Gallina() {
		
	}

	fvec3 Seguir(Player* jugador) {
		fvec3 player_pos = jugador->GetPos();
		fvec3 actual_pos = position;
		bool enRango = sqrt(pow((actual_pos.x - player_pos.x), 2) + pow((actual_pos.z - player_pos.z), 2)) < radioDetection;
		if (enRango)
		{
			if (jugador->itemOnHand) {
				giroGallina(anguloMira, player_pos.z, player_pos.x, GetPos().z, GetPos().x);
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
		return actual_pos;
	}

	void Update(Player* jugador, fvec3& pos) {
		pos = Seguir(jugador);
		SetPos(pos);
	}

	void Draw(Camara* camara, float scale, float specForce) {
		mChickenModel->setPos(position);
		mChickenModel->setAltura(altura);
		mChickenModel->Draw(camara, 'Y', anguloMira, scale, specForce);
	}

	void SetAltura(float altura) {
		this->altura = altura;
		mChickenModel->setAltura(altura);
	}

	void SetPos(fvec3 pos) {
		position = pos;
		CajaDeColision->reposBox(position, scale);
	}

	fvec3 GetPos() {
		return this->position;
	}

	void GetPos(fvec3& pos) {
		pos = position;
	}


	void giroGallina(float& angulo, float jugadorZ, float jugadorX, float gallinaZ, float gallinaX) {
		if (jugadorZ < gallinaZ) {           // GIRO DEL PRIMER ZOMBIE
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
private:

	GameModel* mChickenModel;
	ColBox* CajaDeColision;
	fvec3 position;
	fvec3 scale;
	float altura;
	float anguloMira;
	float radioDetection;
};