#pragma once
#include <d3dx10math.h>
#include <vector>

typedef D3DXVECTOR3 fvec3;

class ColBox {
public:
	fvec3 mOrigen;
	fvec3 mInicial;
	fvec3 mFinal;
	fvec3 mEscala;

	ColBox(fvec3 _o, fvec3 _s) {
		mOrigen.x = _o.x;
		mOrigen.y = _o.y;
		mOrigen.z = _o.z;

		mEscala.x = _s.x;
		mEscala.y = _s.y;
		mEscala.z = _s.z;

		_o.x -= _s.x;
		_o.y -= _s.y;
		_o.z -= _s.z;
		mInicial = _o;

		_o.x += (_s.x * 2);
		_o.y += (_s.y * 2);
		_o.z += (_s.z * 2);
		mFinal = _o;
	}

	ColBox(fvec3 _o, fvec3 _s, float angleY) {
		float s = sin(angleY);
		float c = cos(angleY);
		/*
		p'x = cos(theta) * (px-ox) - sin(theta) * (py-oy) + ox
		p'y = sin(theta) * (px-ox) + cos(theta) * (py-oy) + oy
		*/

		mOrigen.x = _o.x;	
		mOrigen.y = _o.y;
		mOrigen.z = _o.z;

		mEscala.x = _s.x;
		mEscala.y = _s.y;
		mEscala.z = _s.z;

		_o.x = (_o.x - _s.x);
		_o.y = (_o.y - _s.y);
		_o.z = (_o.z - _s.z);
		mInicial = _o;
		float ox = mOrigen.x;
		float oz = mOrigen.z;

		float magx = mInicial.x - mOrigen.x;
		float magz = mInicial.z - mOrigen.z;

		mInicial.x = ((c * magx) - (s * magz)) + ox;
		mInicial.z = ((s * magx) + (c * magz)) + oz;

		_o.x += (_s.x * 2);
		_o.y += (_s.y * 2);
		_o.z += (_s.z * 2);
		mFinal = _o;
		magx = mFinal.x - mOrigen.x;
		magz = mFinal.z - mOrigen.z;
		mFinal.x = ((c * magx) - (s * magz)) + ox;
		mFinal.z = ((s * magx) + (c * magz)) + oz;
	}

	bool CheckColission(ColBox& subject) {
		float x = mOrigen.x;
		float y = mOrigen.y;
		float z = mOrigen.z;

		bool colX = x <= subject.mFinal.x && x >= subject.mInicial.x;
		bool colY = y <= subject.mFinal.y && y >= subject.mInicial.y;
		bool colZ = z <= subject.mFinal.z && z >= subject.mInicial.z;

		return colX && colY && colZ;
	}


	bool CheckPointColission(fvec3 _point) {
		bool colX = _point.x <= mInicial.x && _point.x >= mFinal.x;
		bool colY = _point.z <= mInicial.z && _point.z >= mFinal.z;
		bool colZ = _point.y <= mInicial.y && _point.y >= mFinal.y;

		return colX && colY && colZ;
	}

	bool CheckSphereColission(fvec3 _pos, float radio) {
		float Distance = sqrt( ((mOrigen.x - _pos.x) * (mOrigen.x - _pos.x)) + ((mOrigen.z - _pos.z) * (mOrigen.z - _pos.z)) );
		bool Colission = Distance < radio;
		return Colission;
	}

	ColBox* reposBox(fvec3& _pos, fvec3& scale) {
		return new ColBox(_pos, scale);
	}

	ColBox* reposBox(fvec3& _pos, fvec3& scale, float angle) {
		return new ColBox(_pos, scale, angle);
	}
};


typedef std::vector<ColBox> ColArray;
typedef std::vector<fvec3> RadioColArray;
typedef std::vector<ColBox*> pColArray;