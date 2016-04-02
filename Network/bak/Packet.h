#pragma once


enum	Signal_Type
{
	PACKET_RECV
	,	PACKET_SEND

};

typedef	struct
{
	union {
		struct 
		{
			BYTE	SIZE;				// 총 패킷사이즈
			char	INDEX;				// 소켓 인덱스
			char	STATE;				// 서버? 로비? 룸?
			char	EXETYPE;			// 패킷 형식
			char	ROOMNUMBER;			// 룸정보
			char	NAME[PACKET_NAME];	// 닉네임
			char	ORDER;				// 명령어 형식,에러메세지일경우 에러코드로 사용한다.
			char	DEFINITION;			// 데이터 형식
			BYTE	DATASIZE;			// 데이터 사이즈
			char	DATA[PACKET_DATA];	// 데이터
		} ;
		BYTE packet[PACKET_TOTAL];
	};
}PACKETDATA;

class CPacket
{
protected:
	short	m_Secretkey;
	FILE*	m_fp;
public:
	
	CPacket();
	virtual ~CPacket();
	
	virtual	VOID	SetSecretkey(short iKey);
	virtual INT		PacketInsertSocketNumber(BYTE* pOutPacket, int index);			// 패킷에소켓번호 넣기
	
	
	virtual INT		PacketAssemble(  BYTE* pOut,	   char   INDEX, char STATE, char EXETYPE
									,char  ROOMNUMBER, char*  NAME,  char ORDER, char DEFINITION
									,BYTE  DATASIZE,   void*  pData );				// 패킷조립
	virtual INT		PacketPieces(PACKETDATA* pOut,void* pData, int iSize);			// 패킷분해
	
	virtual	INT		PacketEnCrypt(BYTE* pData, int iSize);							// 패킷 암호화
	virtual INT		PacketDecrypt(BYTE* pData, int iSize);							// 패킷 복호화

	INT				LogSet(char* FilName);											// 로그파일 설정
	virtual INT		LogPrint();
};