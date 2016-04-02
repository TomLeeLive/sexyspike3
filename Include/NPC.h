
#pragma		once

#ifndef		__NPC_H_
#define		__NPC_H_

class		CNpc	:	public CPlayer
{
	CNpc( const Character& SrcModel, int id, LPCSTR pName ) : CPlayer( SrcModel, id, pName ){}

public:	
	~CNpc();

	static	CNpc*	Create(const Character& SrcModel, int id, LPCSTR pName);

	HRESULT			FrameMove( float fElapsedTime = 0.f );

	void			ProcessMacro(float fElapsedTime);
	void			SetMacro( eBehavior BEHAVIOR );
	eBehavior		GetNextBehavior();

};

#endif		__NPC_H_