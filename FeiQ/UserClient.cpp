// UserClient.cpp: implementation of the CUserClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FeiQ.h"
#include "UserClient.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserClient::CUserClient()
{

}

CUserClient::~CUserClient()
{

}

BOOL CUserClient::InitClient( )
{   //����SOCKET
    m_ClientSocket = socket( AF_INET,
        SOCK_DGRAM, IPPROTO_UDP );
    if( INVALID_SOCKET == m_ClientSocket )
    {
        return FALSE;
    }
    //���ù㲥ģʽ
    BOOL bBroadcast = TRUE;
    setsockopt( m_ClientSocket, SOL_SOCKET,
        SO_BROADCAST, (char *)&bBroadcast,
        sizeof( bBroadcast ) );
    return TRUE;
}

BOOL CUserClient::Broadcast( )
{   
    //��������
    USERBROADCAST user = { 0 };
    user.header.nVersion = 1;
    user.header.nCmdID   = NETCMDID_USERBROADCAST;
    user.header.nDataLength = 
        sizeof( user ) - sizeof( NETHEADER );
    
    DWORD nLen = 128;
    GetComputerName( user.szName, &nLen );
    strcpy( user.szSign, "Hello A" );

    //�㲥����
    SendData( (LPSTR)&user, sizeof( user ) );
    return TRUE;
}

BOOL CUserClient::UserQuit( )
{   //��������
    USERQUIT quit = { 0 };
    quit.header.nVersion = 1;
    quit.header.nCmdID   = NETCMDID_USERQUIT;
    quit.header.nDataLength = 
        sizeof( quit ) - sizeof( NETHEADER );
    quit.nExitCode = 0;

    //�㲥����
    SendData( (LPSTR)&quit, sizeof( quit ) );
    return TRUE;
}

BOOL CUserClient::SendChat( CString strIP, 
                            CString strData, 
                            BOOL    bBroadcast )
{
    //��������
    USERCHAT chat = { 0 };
    chat.header.nVersion = 1;
    chat.header.nCmdID   = NETCMDID_USERCHAT;
    chat.header.nDataLength = 
        sizeof( chat ) - sizeof( NETHEADER );
    strcpy( chat.szChat, strData );
    //��������
    if( TRUE == bBroadcast )
    {   //�㲥
        SendData( (LPSTR)&chat, sizeof( chat ) );
    }
    else
    {   //���͵�ָ��IP
        SendData( (LPSTR)&chat, sizeof( chat ), strIP );
    }
    return TRUE;
}


BOOL CUserClient::SendData( CHAR * pszData, 
                            UINT nLen, 
                            LPCSTR pszIP )
{   //���ù㲥��ַ
    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_port   = htons( PORT_USERSVR );
    if( NULL == pszIP )//�㲥
    {
        addr.sin_addr.S_un.S_addr = 
            htonl( INADDR_BROADCAST );
        //���͹㲥����
        sendto( m_ClientSocket, pszData, nLen, 0, 
            (struct sockaddr *)&addr, sizeof( addr ) );
    }
    else//ָ����IP��ַ������Ϣ
    {
        SOCKET sock = socket( AF_INET, SOCK_DGRAM,
            IPPROTO_UDP );
        addr.sin_addr.S_un.S_addr = 
            inet_addr( pszIP );
        //��������
        sendto( sock, pszData, nLen, 0, 
            (struct sockaddr *)&addr, sizeof( addr ) );
        closesocket( sock );
    }
    return TRUE;
}
