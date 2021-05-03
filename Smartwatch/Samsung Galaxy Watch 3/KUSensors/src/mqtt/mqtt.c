/*
 * mqtt.c
 *
 *  Created on: Jan 18, 2021
 *      Author: battl
 */

#include <sensors.h>
#include <stdlib.h>
#include "mqtt.h"
#include "thpool.h"
#include <system_info.h>

#define THREAD_NUM	4

MQTTClient client;
MQTTClient_deliveryToken token;
threadpool thpool;

int mqttInit() {
	// 1. Get MQTT connection information(broker address, port, etc.) through REST API

	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;

	if ((rc = MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Can't create MQTTClient object, %d", rc);
	}

	conn_opts.keepAliveInterval = 3600;
	conn_opts.cleansession = 1;

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
		dlog_print(DLOG_ERROR, LOG_TAG, "Client is not connected with MQTT Broker, %d", rc);
		//exit(-1);
	}
	else {
		dlog_print(DLOG_INFO, LOG_TAG, "MQTT Connected");
	}

	thpool = thpool_init(THREAD_NUM);
	return rc;
}

void _mqttPublish(void * msg) {
	char * tizenId;
	int ret;

    ret = system_info_get_platform_string("http://tizen.org/system/tizenid", &tizenId);
    if (ret != SYSTEM_INFO_ERROR_NONE) {
        /* Error handling */
        return;
    }

	char * topicName = tizenId;
	int rc;
	MQTTClient_message pubMsg = MQTTClient_message_initializer;
	pubMsg.payload = msg;
	pubMsg.payloadlen = strlen(msg);
	pubMsg.qos = QOS;
	pubMsg.retained = 0;

	MQTTClient_publishMessage(client, topicName, &pubMsg, &token);
	rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
	free(tizenId); /* Release after use */
}

void mqttPublish(void * msg) {
	thpool_add_work(thpool, _mqttPublish, msg);
}

int mqttSubscribe() {
	//TODO - implements mqtt subscribing codes
	return 0;
}

void mqttExit() {
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
}
