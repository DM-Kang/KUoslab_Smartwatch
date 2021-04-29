/**
 * 
 */

/**
 * @author dmkang@os.korea.ac.kr
 *
 */
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttAsyncClient;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.MqttPersistenceException;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class MQTT implements MqttCallback{
	private static String Broker;
	private static String Client_ID;
	private static String UserName;
	private static String Passwd;
	private static MqttAsyncClient Client;
	private static MqttMessage message;
	private static MemoryPersistence persistence;
	private static MqttConnectOptions connOpts;

	public MQTT(String broker, String client_id,String username, String passwd){
		MQTT.Broker = broker;
		MQTT.Client_ID = client_id;
		MQTT.UserName = username;
		MQTT.Passwd = passwd;
	}

	public void init(String topic){
		MQTT.persistence = new MemoryPersistence();
		try {
			Client = new MqttAsyncClient(MQTT.Broker, MQTT.Client_ID, MQTT.persistence);
			connOpts = new MqttConnectOptions();
			if(Client_ID!=null && Passwd != null) {
				connOpts.setUserName(MQTT.UserName);
				connOpts.setPassword(MQTT.Passwd.toCharArray());
			}
			connOpts.setCleanSession(true);
			System.out.println("Connecting to broker: "+MQTT.Broker);

			Client.connect(connOpts);
			Client.setCallback(this);

			System.out.println("Connected");

			message = new MqttMessage();
		} catch(MqttException me) {
			System.out.println("reason "+me.getReasonCode());
			System.out.println("msg "+me.getMessage());
			System.out.println("loc "+me.getLocalizedMessage());
			System.out.println("cause "+me.getCause());
			System.out.println("excep "+me);
			me.printStackTrace();
		}

		try {
			Thread.sleep(3000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void disconnect(){
		try {
			Client.disconnect();
			Client.close();
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	public void publish(String topic, String msg, int qos){
		message.setQos(qos);
		message.setPayload(msg.getBytes());

		try {
			Client.publish(topic, message);
		} catch (MqttPersistenceException e) {
			e.printStackTrace();
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	public void subscribe(String topic, int qos){
		try {
			Client.subscribe(topic,qos);
		} catch (MqttException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
		System.out.println("Message arrived : " + new String(mqttMessage.getPayload(), "UTF-8"));
		MQTT_REST_Translator.postwork(topic, mqttMessage);
	}

	@Override
	public void connectionLost(Throwable cause) {
		System.out.println("Lost Connection." + cause.getCause());
	}

	@Override
	public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {
		System.out.println("Message with " + iMqttDeliveryToken + " delivered.");
	}
}