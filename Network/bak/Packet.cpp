
#include "../Include/INCLUDE.h"


extern	std::map< LPCSTR, void*, StrCmpI >	g_mapNetObjects;

CPacket::CPacket()
{
	m_Secretkey = 1234;
	m_fp		= NULL;
}

CPacket::~CPacket()
{
	if(m_fp)
	{
		fclose(m_fp);
	}
}

void CPacket::SetSecretkey(short iKey)
{
	m_Secretkey = iKey;
}


INT	CPacket::PacketInsertSocketNumber(BYTE* pOutPacket, int index)
{
	
	memcpy(pOutPacket+PACKET_SIZE,&index,PACKET_INDEX);

	//int inx = 0;
	//// 패킷이 연속적으로 붙어서 들어올 경우
	//// 패킷하나씩 끊어서 번호를 넣어준다.
	//while(1)
	// {
	//	if(inx < PacketSize)
	//	{
	//		memcpy(pOutPacket + inx + PACKET_SIZE,&index,PACKET_INDEX);
	//		inx+=pOutPacket[inx];
	//	}
	//	else
	//	{
	//		break;
	//	}
	//	 
	// }

	return S_OK;
}

INT	CPacket::PacketAssemble( BYTE* pOut,	   char   INDEX, char STATE, char EXETYPE
							,char  ROOMNUMBER, char*  NAME,  char ORDER, char DEFINITION
							,BYTE  DATASIZE,   void*  pData )
{

	PACKETDATA  tempPacket  ;
	memset(&tempPacket,0,sizeof(PACKETDATA));


	if( INDEX ) { tempPacket.INDEX = INDEX; }
	tempPacket.EXETYPE   = EXETYPE ;
	if( NAME )  { strcpy(tempPacket.NAME,NAME); }

	tempPacket.STATE   = STATE ;
	tempPacket.ROOMNUMBER = ROOMNUMBER ;
	tempPacket.ORDER   = ORDER  ;
	tempPacket.DEFINITION = DEFINITION ;
	tempPacket.DATASIZE   = DATASIZE ;

	memcpy(tempPacket.DATA,pData,DATASIZE);

	tempPacket.SIZE = PACKET_SIZE + PACKET_INDEX + PACKET_STATE + PACKET_EXETYPE +
		+ PACKET_ROOMNUMBER + PACKET_NAME + PACKET_ORDER + PACKET_DEFINITION
		+ PACKET_DATASIZE + DATASIZE;

	memcpy(pOut,&tempPacket,tempPacket.SIZE);

	return S_OK;
}

INT	CPacket::PacketPieces(PACKETDATA* pOut, void* pData, int iSize)
{
	memcpy(pOut,(PACKETDATA*)pData,iSize);
	return S_OK;
}



INT	CPacket::PacketEnCrypt(BYTE* pData,int iSize)
{
	/*int i;
	for(i=1;i<iSize;++i)
	{
		pData[i] = pData[i] ^ m_Secretkey;
	}*/


	return S_OK;
}
INT	CPacket::PacketDecrypt(BYTE* pData,int iSize)
{
	/*int i;
	for(i=1;i<iSize;++i)
	{
		pData[i] = m_Secretkey ^ pData[i];
	}*/


	/*int i;
	int HeaderIdx = pData[0];
	for(i=1;i<iSize;++i)
	{
	if(HeaderIdx == i)
	{
	HeaderIdx+=pData[i];
	}
	else
	{
	pData[i] = m_Secretkey ^ pData[i];
	}
	}*/


	return S_OK;
}
INT	CPacket::LogSet(char* FilName)
{
	m_fp = fopen(FilName,"at");
	return S_OK;
}
INT CPacket::LogPrint()
{
	return S_OK;
}