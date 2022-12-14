#ifndef _camara
#define _camara

//Clase camara hecha por Rafael Rosas para los UltraLMADs
//Videojuegos

#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10math.h>

class Camara{
public:
	D3DXVECTOR3 posCam;
	D3DXVECTOR3 posCam3P;
	D3DXVECTOR3 hdveo;
	D3DXVECTOR3 hdvoy;
	D3DXVECTOR3 refUp;
	D3DXVECTOR3 refRight;
	D3DXVECTOR3 refFront;
	D3DXMATRIX vista;
	D3DXMATRIX proyeccion;
	int ancho;
	int alto;

	D3DXVECTOR3 pastPostCam;
	D3DXMATRIX rotacion;

	float ang = 180;
	float ang2 = 0;

	float objetivex = 0;
	float objetivez = 0;

	float* getPos() {
		float pos[2];
		pos[0] = posCam.x;
		pos[1] = posCam.z;

		return pos;
	}

	Camara(D3DXVECTOR3 eye, D3DXVECTOR3 target, D3DXVECTOR3 up, int Ancho, int Alto)
	{
		//posicion de la camara
		posCam = eye;

		posCam3P = eye;
		posCam3P.z += 15;
		posCam3P.y += 1;

		//a donde ve
		hdveo = target;
		refUp = up;
		ancho = Ancho;
		alto = Alto;

		//crea la matriz de vista
		D3DXMatrixLookAtLH(&vista, &posCam, &hdveo, &refUp);
		//la de proyeccion
		D3DXMatrixPerspectiveFovLH( &proyeccion, D3DX_PI/3, ancho / alto, 0.01f, 1000.0f );
		//las transpone para acelerar la multiplicacion
		D3DXMatrixTranspose( &vista, &vista );
		D3DXMatrixTranspose( &proyeccion, &proyeccion );

		D3DXVec3Normalize(&refUp, &refUp);

		refFront = D3DXVECTOR3(target.x - eye.x, target.y - eye.y, target.z - eye.z);
		D3DXVec3Normalize(&refFront, &refFront);

		D3DXVec3Cross(&refRight, &refFront, &refUp);
		D3DXVec3Normalize(&refRight, &refRight);
		
	}

	D3DXMATRIX UpdateCam(float vel, float arriaba, float izqder)
	{
		pastPostCam = posCam;
		D3DXMATRIX vistaPrev = vista;
		D3DXMatrixTranslation(&vista, 0, 0, 0);

		D3DXVECTOR4 tempo;
		D3DXQUATERNION quatern; //quaternion temporal para la camara
		D3DXMATRIX giraUp, giraRight; //matrices temporales para los giros

		//creamos al quaternion segun el vector up
		D3DXQuaternionRotationAxis(&quatern, &refUp, izqder); 
		//lo normalizamos para que no acumule error
		D3DXQuaternionNormalize(&quatern, &quatern);
		//creamos la matriz de rotacion basados en el quaternion
		D3DXMatrixRotationQuaternion(&giraUp, &quatern);

		//transformamos a los vectores ded la camara
		D3DXVec3Transform(&tempo, &refFront, &giraUp);
		//como el resultado de la operacion anterior es Vec4 lo casteamos para hacerlo vec3
		refFront = (D3DXVECTOR3)tempo;
		//normalizamos para no acumular error
		D3DXVec3Normalize(&refFront, &refFront);
		//Con el vector de referencia y el nuevo front calculamos right de nuevo
		D3DXVec3Cross(&refRight, &refFront, &refUp);

		//una vez calculado right a partir de front y up ahora rotamos sobre right
		//repetimos el procedimiento anterior
		D3DXQuaternionRotationAxis(&quatern, &refRight, arriaba);
		D3DXQuaternionNormalize(&quatern, &quatern);
		D3DXMatrixRotationQuaternion(&giraRight, &quatern);

		D3DXVec3Transform(&tempo, &refFront, &giraRight);
		refFront = (D3DXVECTOR3)tempo;	
		D3DXVec3Normalize(&refFront, &refFront);
		
		rotacion = giraRight;

		//ajustamos la matriz de vista con lo obtenido
		posCam += refFront * vel/10.0;
		posCam3P += refFront * vel/10.0;

		//primera persona
		hdveo = posCam + refFront;
		D3DXMatrixLookAtLH(&vistaPrev, &posCam, &hdveo, &refUp);


		D3DXMatrixMultiply(&vista, &vista, &vistaPrev);

		D3DXMatrixTranspose( &vista, &vista );
		return vista;
	}

	D3DXMATRIX UpdateCam2(float vel, float arriaba, float izqder, D3DXVECTOR3 objcar)
	{
		pastPostCam = posCam;
		D3DXMATRIX vistaPrev = vista;
		D3DXMatrixTranslation(&vista, 0, 0, 0);

		D3DXVECTOR3 refFront2 = D3DXVECTOR3(0, 0, 0);
		D3DXVec3Normalize(&refFront2, &refFront2);

		ang -= izqder * 10;
		ang2 -= izqder * 10;

		objetivex = objcar.x + (2 * (cos((ang2) * 3.1416 / 180)));
		objetivez = objcar.z + (2 * (sin((ang2) * 3.1416 / 180)));

		float x = objcar.x + (30 * (cos(ang * 3.1416 / 180)));
		float z = objcar.z + (30 * (sin(ang * 3.1416 / 180)));

		objcar.y += 15;

		//ajustamos la matriz de vista con lo obtenido
		//posCam += refFront2 * vel / 10.0;
		D3DXMatrixLookAtLH(&vista, &posCam, &objcar, &refUp);
		D3DXMatrixTranspose(&vista, &vista);

		posCam.x = x;
		posCam.z = z;
		posCam.y = objcar.y + 25.0f;
		return vista;
	}

	D3DXVECTOR3 Camaracontra() {
		D3DXVECTOR3 returnpos;
		returnpos.x = objetivex;
		returnpos.z = objetivez;
		returnpos.y = posCam.y;
		return returnpos;
	}

	~Camara()
	{
	}
};
#endif