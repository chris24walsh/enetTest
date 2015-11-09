// enetTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <enet\enet.h>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	
	cout << "Enter 1 to host, or 2 to join: ";
	int choice;
	cin >> choice;

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
		ENetHost * client;
		ENetAddress address;
		address.host = ENET_HOST_ANY; //use localhost
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
			if (enet_host_service(server, &event, 0) && event.type == ENET_EVENT_TYPE_CONNECT) { //Non-blocking check for connection
				cout << "Client has connected." << endl;
				_getch();
			}
		}
	}

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
		ENetHost * server;
		ENetHost * client;
		ENetPeer * peer;
		ENetAddress address;
		address.host = ENET_HOST_ANY; //use localhost
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