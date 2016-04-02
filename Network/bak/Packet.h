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
			BYTE	SIZE;				// �� ��Ŷ������
			char	INDEX;				// ���� �ε���
			char	STATE;				// ����? �κ�? ��?
			char	EXETYPE;			// ��Ŷ ����
			char	ROOMNUMBER;			// ������
			char	NAME[PACKET_NAME];	// �г���
			char	ORDER;				// ��ɾ� ����,�����޼����ϰ�� �����ڵ�� ����Ѵ�.
			char	DEFINITION;			// ������ ����
			BYTE	DATASIZE;			// ������ ������
			char	DATA[PACKET_DATA];	// ������
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
	virtual INT		PacketInsertSocketNumber(BYTE* pOutPacket, int index);			// ��Ŷ�����Ϲ�ȣ �ֱ�
	
	
	virtual INT		PacketAssemble(  BYTE* pOut,	   char   INDEX, char STATE, char EXETYPE
									,char  ROOMNUMBER, char*  NAME,  char ORDER, char DEFINITION
									,BYTE  DATASIZE,   void*  pData );				// ��Ŷ����
	virtual INT		PacketPieces(PACKETDATA* pOut,void* pData, int iSize);			// ��Ŷ����
	
	virtual	INT		PacketEnCrypt(BYTE* pData, int iSize);							// ��Ŷ ��ȣȭ
	virtual INT		PacketDecrypt(BYTE* pData, int iSize);							// ��Ŷ ��ȣȭ

	INT				LogSet(char* FilName);											// �α����� ����
	virtual INT		LogPrint();
};