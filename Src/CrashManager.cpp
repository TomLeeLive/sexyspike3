
#include		"../Include/DEFAULT_INCLUDE.h"
#include		"../Include/CrashManager.h"

bool		CrashManager::DetectSphereCollision(const BoundingSphere* pBase, const BoundingSphere* pDest)
{
	D3DXVECTOR3	sub		= pBase->vCenter - pDest->vCenter;
	float	fLength		= D3DXVec3Length( &sub );
	float	fSumRadius	= pBase->fRadius + pDest->fRadius;

	return	fLength <= fSumRadius;
}
bool		CrashManager::DetectCylinderCollisionSphere(const BoundingCylinder* pBase, const BoundingSphere* pDest)
{
	D3DXVECTOR3	vCenter	= pDest->vCenter - pBase->vCenter;

	if( (vCenter.y+pDest->fRadius) < 0.f || (vCenter.y-pDest->fRadius) > pBase->fHeight )
		return	false;

	vCenter.y			= 0;
	D3DXVECTOR3	sub		= D3DXVECTOR3(0.f,0.f,0.f) - vCenter;
	float	fLength		= D3DXVec3Length( &sub );
	float	fSumRadius	= pBase->fRadius + pDest->fRadius;

	return	fLength <= fSumRadius;
}

bool		CrashManager::DetectCylinderCollisionSphere( const BoundingCylinder* pSpike, const BoundingCylinder* pDefense, const BoundingSphere* pDest)
{
	if( DetectCylinderCollisionSphere( pSpike, pDest ) )
		return	true;

	return	DetectCylinderCollisionSphere( pDefense, pDest );
}

bool		CrashManager::DetectPlaneCollisionSphere( const BoundingPlane* pBase, const BoundingSphere *pDest )
{
	float	d			= D3DXVec3Dot( &pBase->n, &pDest->vCenter );
	if( fabs(fabs((pBase->d)) - fabs(d)) > pDest->fRadius )
		return false;

	D3DXVECTOR3	diff	= pDest->vCenter - pBase->LB;
	if( (diff.x+pDest->fRadius) < 0 || (diff.y+pDest->fRadius) < 0 || (diff.z+pDest->fRadius) < 0 )
		return	false;

	diff	= pDest->vCenter - pBase->RT;
	if( (diff.x-pDest->fRadius) > 0 || (diff.y-pDest->fRadius) > 0 || (diff.z-pDest->fRadius) > 0 )
		return	false;

	return true;
}
