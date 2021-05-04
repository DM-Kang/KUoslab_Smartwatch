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

#define REST_API_ADDR 	"http://ec2-13-125-65-148.ap-northeast-2.compute.amazonaws.com:8080/KUHealth/GetMqttInfo"
#define MQTT_ADDRESS    "tcp://ec2-13-125-65-148.ap-northeast-2.compute.amazonaws.com:1883"
#define CLIENTID    	"Galaxy_Watch_3"
#define QOS         	1
#define TIMEOUT     	10000L

#ifdef __cplusplus
extern "C" {
#endif

int mqttInit();
void mqttPublish(void * msg);
int mqttSubscribe();
void mqttExit();

#ifdef __cplusplus
}
#endif

#endif /* MQTT_H_ */
