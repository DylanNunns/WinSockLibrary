#include <SocketLib.hpp> //My Socket Library
#include <string>
#include <vector>
#include <thread>
#include <mutex>

std::mutex msgMutex;
std::mutex connectMutex;
Socket_Server* server;
std::vector<Client> connectedClients;

/**
 * The Message Loop that once a client is connected is launched in its own thread.
 */
void messageLoop(Client _cli){
	char buf[4096]; 
	while( true ){
		try{
			server->_tcp_ReceiveData( _cli, buf, sizeof(buf) );
			msgMutex.lock();

			std::vector<Client>::iterator it;
			for( it = connectedClients.begin(); it != connectedClients.end(); ++it ){
				try{
					server->_tcp_SendData( *it, buf, sizeof(buf) );
				}
				catch( std::exception ){
					it->Close();
					it = connectedClients.erase( it );
					it--;
				}
			}

			msgMutex.unlock();
		}
		catch( std::exception ){
			std::cout << "Client disconnected." << std::endl;;
			break;
		}
	}
}

void acceptConnections(){
	while( true ){
		try{
			Client client;
			client = server->_tcp_AcceptClient();

			connectMutex.lock();
			connectedClients.push_back(client);
			connectMutex.unlock();

			std::thread t(messageLoop, client);
			t.detach();
			std::cout << "Client connected." << std::endl;
		}
		catch( std::exception ){
			break;
		}
	}
}

int main()
{	
	try{
		std::cout << "Dylan's Chat Room Server!" << std::endl
				  << "**********************************" << std::endl;
		server = new Socket_Server( TCP, "127.0.0.1", 27015 );
		server->_tcp_ListenOnSocket( NULL );
		std::cout << "Waiting for clients.." << std::endl;
		std::thread acceptClients( acceptConnections );
		acceptClients.join();
	}
	catch( std::exception e ){
		std::cout << e.what() << std::endl;
	}
}
