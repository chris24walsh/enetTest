// enetTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <enet\enet.h>
#include <string>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	
	cout << "Enter 1 to host, or 2 to join: ";
	int choice;
	cin >> choice;

	//Server
	if (choice==1) {

		//Initialise the library
		if (enet_initialize () != 0)
		{
			fprintf (stderr, "An error occurred while initializing ENet.\n");
			_getch();
			return EXIT_FAILURE;
		}
		atexit (enet_deinitialize); //deinitialise upon program exit
		
		//Create server
		ENetHost * server;
		ENetAddress address;
		enet_address_set_host(&address, "localhost"); //use localhost
		address.port = 1234;
		server = enet_host_create (& address, 32, 2, 0, 0);
		if (server == NULL) {
			fprintf (stderr, "An error occurred while trying to create an ENet server host.\n");
			_getch();
			exit (EXIT_FAILURE);
		}

		cout << "Server set up. Waiting for clients..." << endl;

		//Wait for connections...
		while(true) {
			ENetEvent event;
			/* Wait up to 1000 milliseconds for an event. */
			while (enet_host_service (server, & event, 1000) > 0)
			{
				switch (event.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					printf ("A new client connected from %x:%u.\n", 
							event.peer -> address.host,
							event.peer -> address.port);
					/* Store any relevant client information here. */
					event.peer -> data = "Client information";
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
							event.packet -> dataLength,
							event.packet -> data,
							event.peer -> data,
							event.channelID);
					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy (event.packet);
        
					break;
       
				case ENET_EVENT_TYPE_DISCONNECT:
					printf ("%s disconnected.\n", event.peer -> data);
					/* Reset the peer's client information. */
					event.peer -> data = NULL;
				}
			}
			/*ENetPacket * packet;
			/string message = "test";
			//getline(cin, message);
			packet = enet_packet_create(message.c_str(), strlen(message.c_str())+1, 1);
			enet_peer_send(peer, 0, packet);
			ENetEvent event;
			if (enet_host_service(server, &event, 0)) {// && event.type == ENET_EVENT_TYPE_RECEIVE) { //Non-blocking check for data
				cout << "Packet received" << endl;
				cout << (char*)event.packet->data;
			}*/
		}
	}

	//Client
	if (choice==2) {

		//Initialise the library
		if (enet_initialize () != 0)
		{
			fprintf (stderr, "An error occurred while initializing ENet.\n");
			_getch();
			return EXIT_FAILURE;
		}
		atexit (enet_deinitialize); //deinitialise upon program exit
		
		//Create client
		ENetHost * client;
		ENetPeer * peer;
		ENetAddress address;
		enet_address_set_host(&address, "localhost"); //use localhost
		address.port = 1234;
		client = enet_host_create(NULL, 1, 1, 0, 0);
		if (client == NULL) {
			fprintf (stderr, "An error occurred while trying to create an ENet server host.\n");
			_getch();
			exit (EXIT_FAILURE);
		}

		cout << "Client set up." << endl;

		//Connect to server
		ENetEvent event;
		peer = enet_host_connect(client, &address, 2, 0);
		if (peer == NULL)
		{
			fprintf (stderr, "No available peers for initiating an ENet connection.\n");
			_getch();
			exit (EXIT_FAILURE);
		}
		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service (client, & event, 5000) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT)
		{
			cout << "Connection to server succeeded." << endl;
			ENetPacket * packet;
			string message = "test";
			//getline(cin, message);
			packet = enet_packet_create(message.c_str(), strlen(message.c_str())+1, 1);
			int success = enet_peer_send(peer, 0, packet);
			cout << success << endl;
			_getch();
		}
		else
		{
			enet_peer_reset (peer);
			cout << "Connection to server failed." << endl;
		}
		_getch();
	}
}