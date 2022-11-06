#pragma once
#include <d3dx10math.h>
#include <vector>

typedef D3DXVECTOR3 fvec3;

class ColBox {
public:
	fvec3 mOrigen;
	fvec3 mEscala;

	ColBox(fvec3 _o, fvec3 _s) {
		mOrigen.x = _o.x;
		mOrigen.y = _o.y;
		mOrigen.z = _o.z;

		mEscala.x = _s.x;
		mEscala.y = _s.y;
		mEscala.z = _s.z;
	}

	ColBox(fvec3 _o, fvec3 _s, float angleY) {
		angleY = 
		mOrigen.x = (_o.x * cosf(angleY) - _o.z * sinf(angleY));
		mOrigen.y = _o.y;
		mOrigen.z = (_o.x * sinf(angleY) + _o.z * cosf(angleY));

		mEscala.x = _s.x;
		mEscala.y = _s.y;
		mEscala.z = _s.z;
	}

	bool CheckColission(ColBox& subject) {
		bool colX = (mOrigen.x + (mEscala.x / 2) <= subject.mOrigen.x + (subject.mEscala.x / 2) &&
			mOrigen.x + (mEscala.x / 2) >= subject.mOrigen.x - (subject.mEscala.x / 2)) ||
			(mOrigen.x - (mEscala.x / 2) <= subject.mOrigen.x + (subject.mEscala.x / 2) &&
				mOrigen.x - (mEscala.x / 2) >= subject.mOrigen.x - (subject.mEscala.x / 2));

		bool colY = (mOrigen.y + (mEscala.y / 2) <= subject.mOrigen.y + (subject.mEscala.y / 2) &&
			mOrigen.y + (mEscala.y / 2) >= subject.mOrigen.y - (subject.mEscala.y / 2)) ||
			(mOrigen.y - (mEscala.y / 2) <= subject.mOrigen.y + (subject.mEscala.y / 2) &&
				mOrigen.y - (mEscala.y / 2) >= subject.mOrigen.y - (subject.mEscala.y / 2));

		bool colZ = (mOrigen.z + (mEscala.z / 2) <= subject.mOrigen.z + (subject.mEscala.z / 2) &&
			mOrigen.z + (mEscala.z / 2) >= subject.mOrigen.z - (subject.mEscala.z / 2)) ||
			(mOrigen.z - (mEscala.z / 2) <= subject.mOrigen.z + (subject.mEscala.z / 2) &&
				mOrigen.z - (mEscala.z / 2) >= subject.mOrigen.z - (subject.mEscala.z / 2));

		return colX && colY && colZ;
	}


	bool CheckPointColission(fvec3 _point) {
		bool colX = _point.x <= mOrigen.x + (mEscala.x/2) && _point.x >= mOrigen.x - (mEscala.x / 2);
		bool colY = _point.z <= mOrigen.z + (mEscala.z/2) && _point.z >= mOrigen.z - (mEscala.z / 2);
		bool colZ = _point.y <= mOrigen.y + (mEscala.x/2) && _point.y >= mOrigen.y - (mEscala.y / 2);

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
typedef std::vector<ColBox*> pColArray;