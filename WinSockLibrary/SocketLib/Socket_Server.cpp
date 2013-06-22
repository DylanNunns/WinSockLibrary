#include <SocketLib.hpp>

Socket_Server::Socket_Server( Protocol proto, const char * ip, unsigned short port ){
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

	CreateSocket( _socktype, _proto );
	BindServerSocket();
}

Socket_Server::~Socket_Server(){
	Close();
}

void Socket_Server::InitializeWinSock( unsigned char lobyte, unsigned char hibyte ){
	iResult = WSAStartup( MAKEWORD( lobyte, hibyte ), &wsaData );
	if( iResult != 0){
		throw std::exception("WSAStartup Failed");
	}
}

void Socket_Server::AddressInfo( const char * ip, unsigned short port, unsigned short family ){
	if( ip == NULL )
		ip = "127.0.0.1";
	
	service.sin_family = family;
	service.sin_addr.s_addr = inet_addr(ip);
	service.sin_port = htons(port);
}

void Socket_Server::CreateSocket( int type, int protocol ){
	serverSocket = socket( service.sin_family, type, protocol );
	if( serverSocket == INVALID_SOCKET ){
		WSACleanup();
		throw std::exception("Error creating Server Socket");
	}
}

void Socket_Server::BindServerSocket(){
	iResult = bind( serverSocket, (SOCKADDR*)&service, sizeof(service) );
	if( iResult == SOCKET_ERROR ){
		closesocket( serverSocket );
		WSACleanup();
		throw std::exception("Error binding socket");
	}

	_udp_ClientAddressLength = sizeof( _udp_ClientAddress );
}

void Socket_Server::_tcp_ListenOnSocket( int numListen ){
	if( numListen >= 1 && numListen <= 0x7fffffff)
		iResult = listen( serverSocket, numListen );
	else
		iResult = listen( serverSocket, SOMAXCONN );
	

	if( iResult == SOCKET_ERROR ){
		closesocket( serverSocket );
		WSACleanup();
		throw std::exception("Failed to listen");
	}
}

Client Socket_Server::_tcp_AcceptClient(){
	SOCKET clientSocket = (unsigned)SOCKET_ERROR;
	clientSocket = accept( serverSocket, NULL, NULL );
	if( clientSocket == INVALID_SOCKET ){
		throw std::exception("Invalid socket accepted.");
	}
	Client cli(clientSocket);
	
	return cli;
}

void Socket_Server::_tcp_ReceiveData( const Client & cli, char * buffer, int bufferSize ){
	bytes = recv( cli._client, buffer, bufferSize, 0 );
	if( bytes == 0 || bytes == SOCKET_ERROR ){
		throw std::exception("Error receiving data");
	}
}
int Socket_Server::_tcp_ReceiveData( const Client & cli, int val ){
	bytes = recv( cli._client, reinterpret_cast<char*>( &val ), sizeof( val ), 0 );
	if( bytes == 0 || bytes == SOCKET_ERROR ){
		throw std::exception("Error receiving data");
	}
	return val;
}

void Socket_Server::_tcp_SendData( const Client & cli, char * buffer, int bufferSize ){
	bytes = send( cli._client, buffer, bufferSize, 0 );
	if( bytes == SOCKET_ERROR ){
		throw std::exception("Error sending data");
	}
}
void Socket_Server::_tcp_SendData( const Client & cli, int val, int bufferSize ){
	bytes = send( cli._client, reinterpret_cast<char*>( &val ), bufferSize, 0 );
	if( bytes == SOCKET_ERROR ){
		throw std::exception("Error sending data");
	}
}

void Socket_Server::_udp_ReceiveData( char * buffer, int bufferSize ){
	bytes = recvfrom( serverSocket, buffer, bufferSize, 0, &_udp_ClientAddress, &_udp_ClientAddressLength );
	if( bytes == 0 || bytes == SOCKET_ERROR ){
		throw std::exception("Error receiving data");
	}
}

void Socket_Server::_udp_ReceiveData( int val, int bufferSize ){
	bytes = recvfrom( serverSocket, reinterpret_cast<char*>( &val ), bufferSize, 0, &_udp_ClientAddress, &_udp_ClientAddressLength );
	if( bytes == 0 || bytes == SOCKET_ERROR ){
		throw std::exception("Error receiving data");
	}
}

void Socket_Server::_udp_SendData( char * buffer, int bufferSize ){
	bytes = sendto( serverSocket, buffer, bufferSize, 0, &_udp_ClientAddress, _udp_ClientAddressLength );
	if( bytes == SOCKET_ERROR ){
		throw std::exception("Error sending data");
	}
}

void Socket_Server::_udp_SendData( int val, int bufferSize ){
	bytes = sendto( serverSocket, reinterpret_cast<char*>( &val ), bufferSize, 0, &_udp_ClientAddress, _udp_ClientAddressLength );
	if( bytes == SOCKET_ERROR ){
		throw std::exception("Error sending data");
	}
}

void Socket_Server::Close(){
	closesocket( serverSocket );
	WSACleanup();
}