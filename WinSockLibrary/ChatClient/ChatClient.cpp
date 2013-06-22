#include <SocketLib.hpp>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

Socket_Client* client;
std::string displayName;
bool justSentMsg;

/**
 * Receives packet data from the Server and outputs it to the console.
 * Has a simple bool check to see if it is your own message being
 * received so that you don't see your message twice.
 */
void GetData(){
	char buf[4096];
	while( true ){
		try{
			client->receiveData( buf, sizeof(buf) );
			if( justSentMsg == false)
				std::cout << buf << std::endl;
			justSentMsg = false;
		}
		catch( std::exception ){
			std::cout << "Server Disconnected" << std::endl;
			break;
		}
	}
}

/**
 * Sends a message to the server
 */
void SendData(std::string input){
	try{
		char buf[4096];
		std::string msg = displayName + ": " + input;
		strcpy_s(buf, msg.c_str());
		client->sendData( buf, sizeof(buf) );
		justSentMsg = true;
	}
	catch( std::exception e ){
		std::cout << e.what() << std::endl;
	}
}

/**
 * Attempts to connect to the Chat Server on localhost
 */
void Connect(){
	try{
		client = new Socket_Client( TCP, "127.0.0.1", 27015 );
		std::cout << "Connected to Dylan's Chat Room!" << std::endl;
	}
	catch( std::exception e ){
		std::cout << e.what() << std::endl;
	}
}

/**
 * Allows you to set your Display Name that other users
 * will see you as.
 */
bool SetDisplayName(){
	std::cout << "Set your display name: ";
	std::cin >> displayName;
	std::cin.ignore(INT_MAX, '\n');
	if( displayName.size() >= 1 )
		return true;
	else
		return false;
}

int main(){
SetDisplayName(); // Prompts you to set your display name
std::thread launchClient( Connect ); // Launches a Connection to the server in a thread
launchClient.join(); // Joins that thread
std::thread receiveMessages( GetData ); // Launches a seperate thread that receives any incoming messages
receiveMessages.detach(); // Detached so that it runs independently
std::string line;
while( std::getline( std::cin, line ) ){
	try{
		std::thread SendMsg( SendData, line );
		SendMsg.detach();
	}
	catch( std::exception e ){
		std::cout << e.what() << std::endl;
	}
}
}