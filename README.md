
# COVID-19 Confirmed Positive Cases Management System using Smartwatch and Hyperledger-fabric Blockchain Network for Data Integrity

Since the pandemic caused by COVID-19 has continued, the need for a system that can collect, safely store and search the movement and health information of confirmed patients has emerged. In particular, it is extremely important to block forgery/modification attempts caused by external intrusion and to keep data integrity. This software meets these needs.


# Architecture

The structure of this software can be described as follows.
![스크린샷 2021-04-29 오후 12.44.04](https://tva1.sinaimg.cn/large/008i3skNgy1gq0h9wtxnuj312l0lctde.jpg)

## Smartwatch

Smartwatches are great devices that can provide information on the movement of users. It can also measure the user's health information (heart rate, blood pressure, blood sugar, step count, etc.) and transmit the data directly to the cloud server via Wi-Fi or cellular networks.
Currently, a watch application using Tizen-based Galaxy Watch 3 is implemented and uses the MQTT protocol.

## MQTT Broker & MQTT to REST Translator
The MQTT protocol is a popular protocol widely used for data transmission of IoT devices. MQTT can be used in high-performance devices such as smartphones as well as small devices without OS. In addition, it is supported by most commercial clouds (ex> Amazon AWS, MS Azure, etc.), so it has an advantage in terms of connectivity.
In this project, Mosquitto is used as MQTT Broker, and Mosquitto Configuration Files are provided for correct use.
In addition, this project supports MQTT to REST Translator to convert MQTT messages into REST API calls.

## Blockchain Network based on Hyperledger-Fabric

Blockchain network is an essential element to protect the security and integrity of data collected from users. In this project, a blockchain network is constructed using Hyperledger-Fabric, and Hyperledger-Composer is used to control this network. Hyperledger-Composer provides a REST API that supports secure addition, inquiry, and deletion of user data.

## Mobile App
Regardless of Android/iOS, data can be visualized using any mobile application that supports the MQTT protocol. Many of these applications have been uploaded to the App Store. The following applications are recommended for this project.


# Setup

## Smartwatch-side

#### Installing Tizen Studio

Please refer to the link below https://developer.tizen.org/ko/development/tizen-studio/download/installing-tizen-studio



#### Issuing a certificate for the operation of the Samsung smartwatch application

Please refer to the link below https://docs.tizen.org/application/vstools/tools/certificate-manager/



#### Installing Samsung Wearable Platform

This project is created based on Samsung Wearable Platform v5.5. Samsung Wearable Platform can be installed using Package Manager in Tizen Studio.

![스크린샷 2021-04-29 오전 10.51.46](https://tva1.sinaimg.cn/large/008i3skNgy1gq0he9expij30km0l20z2.jpg)



#### Connecting Samsung smartwatch through Wi-Fi

In order to connect the Samsung smartwatch and the IDE, the computer on which the IDE is installed and the smartwatch must be connected to the same Wi-Fi network, and you can see that the smartwatch is detected as shown below.

![스크린샷 2021-04-29 오전 11.02.56](https://tva1.sinaimg.cn/large/008i3skNgy1gq0h9j49naj30uw0n1gsm.jpg)



#### Compiling Samsung smartwatch applications

Import the KUSensors in the Smartwatch/Samsung Galaxy Watch 3/ path to Tizen Studio and click the Run button.

![스크린샷 2021-04-29 오후 1.07.15](https://tva1.sinaimg.cn/large/008i3skNgy1gq0hj21icsj31g90u0kcq.jpg)

If it is successful, you will be able to confirm that the tpk file is normally installed on the smartwatch as shown below.

![스크린샷 2021-04-29 오전 11.04.00](https://tva1.sinaimg.cn/large/008i3skNgy1gq0hkonsqsj30yb0b3te3.jpg)



## MQTT Broker-side

#### Installing Mosquitto

Mosquitto is an open source MQTT Broker that anyone can easily use. Installation can be done through the command below. (Ubuntu 20.04)

```
sudo apt-get install mosquitto
```

When Mosquitto is installed, shut down the Mosquitto daemon as follows to use customized configurations for this project, and run it again with the configuration file.

```
sudo service mosquitto stop
cd MQTT Broker/Mosquitto
./run_mosquitto_broker.sh
```

After running Mosquitto, you can monitor messages from the topic you want to receive with the command below.

```
mosquitto_sub -t "Topic" -p 1883
```



#### Running MQTT to REST Translator

The Hyperledger network used in this project provides REST APIs externally for creating, viewing, and deleting Ledgers. Therefore, you need a plug-in that uses the data transmitted from the smartwatch to convert it to REST APIs and delivers it to the Hyperledger network. Since this application is written in Java, you can make it into a Jar-type executable file and run it with MQTT Broker.



## Hyperledger Network-side

#### Installing Hyperledger-Composer & Fabric

Please refer to the link below https://github.com/DM-Kang/KUoslab_Smartwatch/blob/main/HF_custom/README.md



## Result

As shown below, it can be seen that the health data transmitted from the smart watch was transmitted to the Hyperledger-fabric network through the MQTT Broker.

![그림2](https://tva1.sinaimg.cn/large/008i3skNgy1gq0i6k1u56j30wn0mm15d.jpg)

# Future Works






