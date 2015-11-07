// enetTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <conio.h>
#include <enet\enet.h>
#include <string.h>
#include <sstream>

bool useServer = true;
ENetHost * host; //This machine
ENetPeer * peer; //Remote machine
ENetEvent  event; //Current event thread


//Functions
void connectServer();
void sendData(std::string);
int receiveData();

int _tmain(int argc, _TCHAR* argv[])
{
	//Initialise the library
	if (enet_initialize () != 0)
    {
        fprintf (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit (enet_deinitialize); //deinitialise upon program exit

	//Create server
	if (useServer) {
		ENetAddress address;
		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */
		address.host = ENET_HOST_ANY;
		/* Bind the server to port 1234. */
		address.port = 1234;
		host = enet_host_create (& address /* the address to bind the server host to */, 
									 32      /* allow up to 32 clients and/or outgoing connections */,
									  2      /* allow up to 2 channels to be used, 0 and 1 */,
									  0      /* assume any amount of incoming bandwidth */,
									  0      /* assume any amount of outgoing bandwidth */);
		if (host == NULL)
		{
			fprintf (stderr, 
					 "An error occurred while trying to create an ENet server host.\n");
			exit (EXIT_FAILURE);
		}

		std::cout << "This machine is the server.\nWaiting for client handshake...\n";
	}

	//Create client
	else {
		host = enet_host_create (NULL /* create a client host */,
					1 /* only allow 1 outgoing connection */,
					2 /* allow up 2 channels to be used, 0 and 1 */,
					0	/* 57600 / 8 /* 56K modem with 56 Kbps downstream bandwidth */,
					0	/* 14400 / 8 /* 56K modem with 14 Kbps upstream bandwidth */);
		if (host == NULL)
		{
			fprintf (stderr, 
					 "An error occurred while trying to create an ENet client host.\n");
			exit (EXIT_FAILURE);
		}

		std::cout << "This machine is the client\n";

		//Connect to the server machine
		connectServer();

		//Send some data
		sendData("Hello there");

		std::cout << "Sent handshake\n";
	}
	 
	//Program loop
	while(true) {
		//std::cout << "waiting for data...\n";
		//_getch();
		int success = receiveData();
		if (success) {
			std::cout << "\nRemote: " << event.packet->data;
			std::cout << "\nLocal: ";
			std::string message;
			std::getline(std::cin,message);
			sendData(message);
		}
	}

	//Exiting program
	enet_host_destroy(host);
	
	std::cout << "Exiting...";
	_getch();
	return 0;
}

//Connect to server
void connectServer() {
	ENetAddress address;
	ENetEvent event;
	//ENetPeer *peer;
	/* Connect to some.server.net:1234. */
	enet_address_set_host (& address, "192.168.8.102");
	address.port = 1234;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect (host, & address, 2, 0);    
	if (peer == NULL)
	{
	   fprintf (stderr, 
				"No available peers for initiating an ENet connection.\n");
	   exit (EXIT_FAILURE);
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service (host, & event, 7000) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts ("Connection to some.server.net:1234 succeeded.");
		//
	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
		enet_peer_reset (peer);
		puts ("Connection to some.server.net:1234 failed.");
	}
}

//Sending data
void sendData(std::string message) {
	/* Create a reliable packet of size 7 containing "packet\0" */
	ENetPacket * packet = enet_packet_create (message.c_str(), strlen (message.c_str()) + 1, ENET_PACKET_FLAG_RELIABLE);
	/* Extend the packet so and append the string "foo", so it now */
	/* contains "packetfoo\0"                                      */
	//enet_packet_resize (packet, strlen ("packetfoo") + 1);
	//strcpy (& packet -> data [strlen ("packet")], "foo");
	/* Send the packet to the peer over channel id 0. */
	/* One could also broadcast the packet by         */
	/* enet_host_broadcast (host, 0, packet);         */
	enet_peer_send (peer, 0, packet);
	/* One could just use enet_host_service() instead. */
	enet_host_flush (host);
}

//Receiving data
int receiveData() {
	//ENetEvent event;
	// Non-blocking event poll
	while (enet_host_service (host, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf ("A new client connected from %x:%u.\n", 
					event.peer -> address.host,
					event.peer -> address.port);
			/* Store any relevant client information here. */
			event.peer -> data = "Client information";
			//return 0;
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf ("A packet of length %u containing %s was received from %s on channel %u.\n",
					event.packet -> dataLength,
					event.packet -> data,
					event.peer -> data,
					event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy (event.packet);
			return 1;
			//break;
       
		case ENET_EVENT_TYPE_DISCONNECT:
			printf ("%s disconnected.\n", event.peer -> data);
			/* Reset the peer's client information. */
			event.peer -> data = NULL;
		}
		return 0;
	}
}
