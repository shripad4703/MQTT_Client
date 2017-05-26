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
#define CLIENTID    "ExampleClientPub"
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

void parse_options(int argc, char** argv);
void Set_MQTTClient_ConnectOptions (MQTTClient_connectOptions *conn_opts);
void usage(void);


void Set_MQTTClient_ConnectOptions (MQTTClient_connectOptions *conn_opts)
{
	conn_opts->keepAliveInterval = KEEP_ALIVE_INTERVAL;
	conn_opts->cleansession = TRUE;
}

void Configure_PubMsg(MQTTClient_message *pubmsg)
{
	pubmsg->qos = (int)QOS_1;
	pubmsg->retained = TRUE;
}

int main(int argc, char* argv[])
{
	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_deliveryToken token;
	int rc;
	int ch;
	char _payload[128];

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

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(EXIT_FAILURE);
	}

	Configure_PubMsg(&pubmsg);
	//pubmsg.qos = QOS;
	//pubmsg.retained = 1;

	do
	{
		sprintf(_payload, "Detected Temp. %d", (rand() % 40));

		pubmsg.payload = _payload;
		pubmsg.payloadlen = strlen(_payload);

		MQTTClient_publishMessage(client, topic, &pubmsg, &token);

		printf("Waiting for up to %d seconds for publication of %s\n on topic %s for client with ClientID: %s\n",
				(int) (TIMEOUT / 1000), _payload, topic, client_name);

		rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);

		printf("Message with delivery token %d delivered\n", token);

		sleep(1);

	}while(1);

	MQTTClient_disconnect(client, TIMEOUT);

	MQTTClient_destroy(&client);

	return rc;
}

void usage(void)
{
	printf("MQTT Publisher\n");
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
