#include <SocketLib.hpp>

Client::Client() { }

Client::Client( SOCKET client ){
	_client = client;
}

void Client::Close(){
	closesocket(_client);
}