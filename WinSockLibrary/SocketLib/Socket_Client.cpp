#include <SocketLib.hpp>

Socket_Client::Socket_Client( Protocol proto, const char * ip, unsigned short port ){
	InitializeWinSock( 2, 2 );
	AddressInfo( ip, port, AF_INET );

	int _socktype, _proto;

	switch( proto ){
	case TCP:
		_socktype = SOCK_STREAM;
		_proto = IPPROTO_TCP;
		break;
	case UDP:
		_socktype = SOCK_DGRAM;
		_proto = IPPROTO_UDP;
		break;
	default:
		_socktype = SOCK_STREAM;
		_proto = IPPROTO_TCP;
		break;
	}
	protocol = _proto;
	CreateSocket( _socktype, _proto );
	_tcp_Connect();
}

Socket_Client::~Socket_Client(){
	Close();
}

void Socket_Client::InitializeWinSock( unsigned char lobyte, unsigned char hibyte ){
	iResult = WSAStartup( MAKEWORD( lobyte, hibyte ), &wsaData );
	if( iResult != 0){
		throw std::exception("WSAStartup Failed");
	}
}

void Socket_Client::AddressInfo( const char * serverIP, unsigned short serverPort, unsigned short family ){
	if( serverIP == NULL )
		serverIP = "127.0.0.1";

	serverAddress.sin_family = family;
	serverAddress.sin_addr.s_addr = inet_addr( serverIP );
	serverAddress.sin_port = htons( serverPort );
}

void Socket_Client::CreateSocket( int type, int protocol ){
	connectSocket = socket( serverAddress.sin_family, type, protocol );
	if( connectSocket == INVALID_SOCKET ){
		WSACleanup();
		throw std::exception("Error creating Server Socket");
	}
}

void Socket_Client::_tcp_Connect(){
	iResult = connect( connectSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress) );
	if( iResult == SOCKET_ERROR ){
		closesocket(connectSocket);
		connectSocket = INVALID_SOCKET;
	}
}

void Socket_Client::sendData( char * buffer, int bufferSize ){
	if( protocol == IPPROTO_TCP )
		bytes = send( connectSocket, buffer, bufferSize, 0 );
	if( protocol == IPPROTO_UDP )
		bytes = sendto( connectSocket, buffer, bufferSize, 0, (SOCKADDR*)&serverAddress, sizeof(serverAddress) );
	
	if( bytes == SOCKET_ERROR ){
		throw std::exception("Error sending data");
	}
}
void Socket_Client::sendData( int val ){
	if( protocol == IPPROTO_TCP )
		bytes = send( connectSocket, reinterpret_cast<char*>( &val ), sizeof ( val ), 0 );
	if( protocol == IPPROTO_UDP )
		bytes = sendto( connectSocket, reinterpret_cast<char*>( &val ), sizeof ( val ), 0, (SOCKADDR*)&serverAddress, sizeof(serverAddress) );

	if( bytes == SOCKET_ERROR ){
		throw std::exception("Error sending data");
	}
}

void Socket_Client::receiveData( char * buffer, int bufferSize ){
	if( protocol == IPPROTO_TCP )
		bytes = recv( connectSocket, buffer, bufferSize, 0 );
	if( protocol == IPPROTO_UDP )
		bytes = recvfrom( connectSocket, buffer, bufferSize, 0, NULL, NULL );

	if( bytes == 0 || bytes == SOCKET_ERROR ){
		throw std::exception("Error receiving data");
	}
}
void Socket_Client::receiveData( int val, int bufferSize ){
	if( protocol == IPPROTO_TCP )
		bytes = recv( connectSocket, reinterpret_cast<char*>( &val ), bufferSize, 0 );
	if( protocol == IPPROTO_UDP )
		bytes = recvfrom( connectSocket, reinterpret_cast<char*>( &val ), bufferSize, 0, NULL, NULL );

	if( bytes == 0 || bytes == SOCKET_ERROR ){
		throw std::exception("Error receiving data");
	}
}

void Socket_Client::Close(){
	closesocket( connectSocket );
	WSACleanup();
}