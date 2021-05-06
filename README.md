
# COVID-19 Confirmed Positive Cases Management System using Smartwatch and Hyperledger-fabric Blockchain Network for Data Integrity

Since the pandemic caused by COVID-19 has continued, the need for a system that can collect, safely store and search the movement and health information of confirmed patients has emerged. In particular, it is extremely important to block forgery/modification attempts caused by external intrusion and to keep data integrity. This software meets these needs.


# Architecture

The structure of this software can be described as follows.
![스크린샷 2021-04-29 오후 12.44.04](https://tva1.sinaimg.cn/large/008i3skNgy1gq0h9wtxnuj312l0lctde.jpg)

## Smartwatch

Smartwatches are great devices that can provide information on the movement of users. It can also measure the user's health information (heart rate, blood pressure, blood sugar, step count, etc.) and transmit the data directly to the cloud server via Wi-Fi or cellular networks.
Currently, a watch application using Tizen-based Samsung Galaxy Watch 3 is implemented and uses the MQTT protocol.

## MQTT Broker, REST API Server & MQTT to REST Translator
The MQTT protocol is a popular protocol widely used for data transmission of IoT devices. MQTT can be used in high-performance devices such as smartphones as well as small devices without OS. In addition, it is supported by most commercial Clouds (ex> Amazon AWS, MS Azure, etc.), so it has an advantage in terms of connectivity.
In this project, Mosquitto is used as MQTT Broker, and Mosquitto Configuration Files are provided for correct use.
In addition, this project provides REST API Server & MQTT to REST Translator to support functionalities such as secure connection between smartwatches and the Cloud and converting MQTT messages into REST API calls.

## Blockchain Network based on Hyperledger-Fabric

Blockchain network is an essential element to protect the security and integrity of data collected from users. In this project, a blockchain network is constructed using Hyperledger-Fabric, and Hyperledger-Composer is used to control this network. Hyperledger-Composer provides REST APIs that support secure addition, inquiry, and deletion of user data.

## Mobile App
Regardless of Android/iOS, data can be visualized using any mobile application that supports the MQTT protocol. Many of these applications have been uploaded to the App Store. The following applications are recommended for this project.

- IoT MQTT Panel: https://play.google.com/store/apps/details?id=snr.lab.iotmqttpanel.prod
- MQTT Dashboard: https://play.google.com/store/apps/details?id=com.lapetov.mqtt


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

***<u>I recommend using a commercial cloud server such as Amazon AWS to take advantage of the free DDNS service to improve remote connectivity.</u>***

Mosquitto is an open source MQTT Broker that anyone can easily use. Installation can be done through the command below. (Ubuntu 20.04)

```
sudo apt-get install mosquitto mosquitto-clients
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



#### Running REST API Server

REST API Server is a component that the smartwatch accesses when communicating with the cloud for the first time. It checks whether the smartwatch requesting connection is a valid device, and returns necessary information for MQTT connection. Also, The Hyperledger network used in this project provides REST APIs externally for creating, querying, and deleting Ledgers. Therefore, you need a plug-in that uses the data transmitted from the smartwatch to convert it to REST APIs and delivers it to the Hyperledger network. This software was developed based on Spring Boot Framework, and can be easily compiled through Maven. 

The execution command of the JAR file is as follows.

```
cd REST_API_Server/bin
java -jar ku-health-api-0.0.1-SNAPSHOT.jar
```

![스크린샷 2021-05-04 오후 9.20.22](https://tva1.sinaimg.cn/large/008i3skNgy1gq8mg3xmx2j30lf0ook1j.jpg)

![스크린샷 2021-05-04 오후 9.24.32](https://tva1.sinaimg.cn/large/008i3skNgy1gq8mgf8nhuj30cg0c6q5m.jpg)

## Hyperledger Network-side

#### (Optional) Running MQTT to REST Translator

*(The functions of MQTT to REST Translator are currently integrated in REST API Server. But if access is difficult due to problems such as Public Domain from MQTT Broker-side to Hyperledger Network-side, run this software on Hyperledger Network-side to connect with 2 components above. Since this application is written in Java, you can make it into a Jar-type executable file and run it on Hyperledger Network-side.)*

The execution command of the JAR file is as follows.

```
cd MQTT_REST_Translator/target
java -jar MQTT_REST_Translator.jar
```



#### Installing & Running Hyperledger-Composer & Fabric

***<u>To install this software, your computer must have at least 8 GB of storage (cannot be installed on instances provided through AWS Free-tier)</u>***

- Linux(Ubuntu 16.04 or higher)

- Docker

  https://docs.docker.com/install/linux/docker-ce/ubuntu/ 
  
- NVM

  ```
  cd $HOME
  sudo apt-get install build-essential libssl-dev
  curl https://raw.githubusercontent.com/creationix/nvm/v0.25.0/install.sh | bash 
  source ~/.profile
  ```

- NODEJS & NPM

  ```
   nvm install v8.7
  ```

- MongoDB

  - https://docs.mongodb.com/manual/tutorial/install-mongodb-on-ubuntu/

- Golang(1.13)

  - https://golang.org/doc/install
  - When installing Golang, set the location (GOPATH) to save source and binary files to $HOME/go, $HOME/go/bin

- Hyperledger Fabric (version 1.1) - Component and binary file installation to operate fabric network

  ```
   mkdir -p $GOPATH/src/github.com
   cd $GOPATH/src/github.com 
   git clone https://github.com/hyperledger/fabric-samples.git
   cd fabric-samples
   curl -sSL https://bit.ly/2ysbOFE | bash -s  1.1.0
  ```

- Hyperledger Composer (version 0.19) - Provides REST server that can execute chaincode by interlocking with Fabric

  ```
   npm install -g composer-cli@0.19
   npm install -g composer-rest-server@0.19
   npm install -g generator-hyperledger-composer@0.19
   npm install -g composer-playground@0.19
  ```

- Running the Hyperledger Fabric network

  ```
   cd HF_custom/hyperledger/hyperledger-fabric
   ./2_startFabric.sh
  ```

  ![results1](https://tva1.sinaimg.cn/large/008i3skNgy1gq0jahphloj31xo0n5qni.jpg)

- Running Hyperledger Composer REST Server & MongoDB

  ```
   cd ../hyperledger-composer
   ./run-oslab.sh
   cd ../../app/bin
   ./db.sh
  ```

  - Checking the execution result (Issuing an Admin Card and checking the chaincode installation)

    ![results2](https://tva1.sinaimg.cn/large/008i3skNgy1gq0jdt4yjoj30uu0q2dja.jpg)

  - Checking Composer's REST server execution (http://localhost:5000)
  
    ![results3](https://tva1.sinaimg.cn/large/008i3skNgy1gq0jeq2tdkj31jk0u04qp.jpg)



## Result

As shown below, it can be seen that the health data transmitted from the smart watch was transmitted to the Hyperledger-fabric network through the MQTT Broker.

![그림2](https://tva1.sinaimg.cn/large/008i3skNgy1gq0i6k1u56j30wn0mm15d.jpg)







