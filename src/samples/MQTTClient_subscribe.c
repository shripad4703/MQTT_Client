/*******************************************************************************
 * Copyright (c) 2012, 2013 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *   http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial contribution
 *******************************************************************************/

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientSub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L
#define KEEP_ALIVE_INTERVAL	20U
#define	TRUE		1U
#define FALSE		0U

const char *client_name = "default_pub"; 	// -c
const char *ip_addr = "127.0.0.1";		// -i
const char *topic = TOPIC;	// -t
int port = 1883;

volatile MQTTClient_deliveryToken deliveredtoken;
void parse_options(int argc, char** argv);
void usage(void);

void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int i;
	char* payloadptr;

	printf("Message arrived\n");
	printf("     topic: %s\n", topicName);
	printf("   message: ");

	payloadptr = message->payload;
	for (i = 0; i < message->payloadlen; i++)
	{
		putchar(*payloadptr++);
	}
	putchar('\n');
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("cause: %s\n", cause);
}

void Set_MQTTClient_ConnectOptions (MQTTClient_connectOptions *conn_opts)
{
	conn_opts->keepAliveInterval = KEEP_ALIVE_INTERVAL;
	conn_opts->cleansession = TRUE;
}

int main(int argc, char* argv[])
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;
	int ch;
	char address[32];

	if (argc > 1)
	{
		parse_options(argc, argv);
	}

	sprintf(address, "tcp://localhost:%d", port);

	printf("Creating MQTT client with name %s\n", CLIENTID);
	MQTTClient_create(&client, address, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

	Set_MQTTClient_ConnectOptions(&conn_opts);
	//conn_opts.keepAliveInterval = 20;
	//conn_opts.cleansession = 1;

	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}
	printf("Subscribing to topic %s\nfor client %s using QoS%d\n Press Q<Enter> to quit\n\n", topic, client_name, QOS_1);

	MQTTClient_subscribe(client, topic, (int)QOS_1);

	do
	{
		ch = getchar();
	} while (ch != 'Q' && ch != 'q');

	MQTTClient_disconnect(client, TIMEOUT);
	MQTTClient_destroy(&client);
	return rc;
}

void usage(void)
{
	printf("MQTT subscriber\n");
	printf("Usage:, where options are:\n");
	printf("  -i <ipaddress> (default is %s)\n", ip_addr);
	printf("  -p <port> (default is %d)\n", port);
	printf("  -c <clientid> (default is %s)\n", client_name);
	printf("  -t <topic> (default is %s)\n", topic);
	exit(EXIT_FAILURE);
}

void parse_options(int argc, char** argv)
{
	int i;
	for (i = 1; i < argc; ++i)
	{
		if (strcmp("-c", argv[i]) == 0)
		{
			printf("client:%s ", argv[++i]);
			client_name = argv[i];
		}

		if (strcmp("-i", argv[i]) == 0)
		{
			printf("ip:%s ", argv[++i]);
			ip_addr = argv[i];
		}

		if (strcmp("-p", argv[i]) == 0)
		{
			printf("port:%s ", argv[++i]);
			port = atoi(argv[i]);
		}

		if(strcmp("-h", argv[i]) == 0)
		{
			usage();
		}

		if (strcmp("-t", argv[i]) == 0)
		{
			printf("topic:%s ", argv[++i]);
			topic = argv[i];
		}
	}
	puts("");
}
