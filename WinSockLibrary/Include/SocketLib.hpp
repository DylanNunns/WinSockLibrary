/* 
 * Windows WinSock TCP/UDP Library
 * Creators: Dylan Nunns
 * April 21, 2013
*/

#ifndef __SOCKET_LIB__
#define __SOCKET_LIB__

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
#include <iostream>
#pragma comment (lib, "ws2_32.lib")

#if defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib,"SocketLib-mt-sgd.lib")
#elif defined(_DEBUG) && defined(_DLL)
#pragma comment (lib,"SocketLib-mt-gd.lib")
#elif !defined(_DEBUG) && !defined(_DLL)
#pragma comment (lib,"SocketLib-mt-s.lib")
#elif !defined(_DEBUG) && defined(_DLL)
#pragma comment (lib,"SocketLib-mt.lib")
#endif

static enum Protocol { TCP, UDP };

class Socket_Server;

/* Class: Client
 * Purpose: Handles connected clients
*/
class Client{
	friend Socket_Server;
	SOCKET _client;

public:
	Client();
	Client( SOCKET client );
	void Close();
};

/* Class: Socket_Server
 * Purpose: Handles the creation of TCP/UDP Server Applications
*/
class Socket_Server{
private:
	WSADATA		wsaData;
	SOCKET		serverSocket;
	sockaddr_in	service;
	sockaddr	_udp_ClientAddress;
	socklen_t	_udp_ClientAddressLength;
	int			iResult;
	
	void InitializeWinSock( unsigned char lobyte, unsigned char hibyte );
	void AddressInfo( const char * ip, unsigned short port, unsigned short family );
	void CreateSocket( int type, int protocol );
	void BindServerSocket();

public:
	int bytes;
	
	Socket_Server( Protocol proto, const char * ip, unsigned short port );
	~Socket_Server();

	void _tcp_ListenOnSocket( int numListen );
	Client _tcp_AcceptClient();

	void _tcp_SendData( const Client & cli, char * buffer, int bufferSize );
	void _tcp_SendData( const Client & cli, int val, int bufferSize );
	void _tcp_ReceiveData( const Client & cli, char * buffer, int bufferSize );
	int _tcp_ReceiveData( const Client & cli, int val );

	void _udp_SendData( char * buffer, int bufferSize );
	void _udp_SendData( int val, int bufferSize );
	void _udp_ReceiveData( char * buffer, int bufferSize );
	void _udp_ReceiveData( int val, int bufferSize );

	void Close();
};

/* Class: Socket_Client
 * Purpose: Handles the creation of TCP/UDP Client Applications
*/
class Socket_Client{
private:
	WSADATA		wsaData;
	SOCKET		connectSocket;
	sockaddr_in	serverAddress;
	int			iResult;
	
	void InitializeWinSock( unsigned char lobyte, unsigned char hibyte );
	void AddressInfo( const char * serverIP, unsigned short serverPort, unsigned short family );
	void CreateSocket( int type, int protocol );
	void _tcp_Connect();

public:
	int bytes;
	int protocol;

	Socket_Client( Protocol proto, const char * ip, unsigned short port );
	~Socket_Client();

	void sendData( char * buffer, int bufferSize );
	void sendData( int val );
	void receiveData( char * buffer, int bufferSize );
	void receiveData( int val, int bufferSize );

	void Close();
};

#endif