
#pragma		once

#ifndef		__CRASH_MANAGER_H_
#define		__CRASH_MANAGER_H_

class		CrashManager
{

public:
	CrashManager(){}

	~CrashManager(){}

	//	�ڽ��浹 ����
	bool		DetectBoxCollision();
	//	�� �浹 ����
	bool		DetectSphereCollision( const BoundingSphere* pBase, const BoundingSphere* pDest);

	//	�Ǹ���, �� �浹
	bool		DetectCylinderCollisionSphere( const BoundingCylinder* pBase, const BoundingSphere* pDest);

	//	�Ǹ���(2), �� �浹
	bool		DetectCylinderCollisionSphere( const BoundingCylinder* pSpike, const BoundingCylinder* pDefense, const BoundingSphere* pDest);

	//	��, �� �浹
	bool		DetectPlaneCollisionSphere( const BoundingPlane* pBase, const BoundingSphere *pDest );

};

#endif