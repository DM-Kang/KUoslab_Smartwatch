/*
 * mqtt.h
 *
 *  Created on: Jan 18, 2021
 *      Author: dmkang
 */

#ifndef MQTT_H_
#define MQTT_H_

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "mqtt/MQTTClient.h"

#define ADDRESS     "tcp://ec2-13-125-65-148.ap-northeast-2.compute.amazonaws.com:1883"
#define CLIENTID    "GalaxyWatch3-1910"
#define TOPIC       "TizenWatch-1910"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

int mqttInit();
void mqttPublish(void * msg);
int mqttSubscribe();
void mqttExit();

#endif /* MQTT_H_ */
