
#pragma		once

#ifndef		__CRASH_MANAGER_H_
#define		__CRASH_MANAGER_H_

class		CrashManager
{

public:
	CrashManager(){}

	~CrashManager(){}

	//	박스충돌 검출
	bool		DetectBoxCollision();
	//	원 충돌 검출
	bool		DetectSphereCollision( const BoundingSphere* pBase, const BoundingSphere* pDest);

	//	실린더, 원 충돌
	bool		DetectCylinderCollisionSphere( const BoundingCylinder* pBase, const BoundingSphere* pDest);

	//	실린더(2), 원 충돌
	bool		DetectCylinderCollisionSphere( const BoundingCylinder* pSpike, const BoundingCylinder* pDefense, const BoundingSphere* pDest);

	//	면, 원 충돌
	bool		DetectPlaneCollisionSphere( const BoundingPlane* pBase, const BoundingSphere *pDest );

};

#endif