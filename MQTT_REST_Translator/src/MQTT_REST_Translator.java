import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.URL;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Date;

import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

/**
 * @author dmkang@os.korea.ac.kr
 *
 */

public class MQTT_REST_Translator {
	public static void main(String[] args){
		String MqttServer 	= "tcp://ec2-13-125-65-148.ap-northeast-2.compute.amazonaws.com:1883";
		String client_id 	= "MQTT_REST_Translator_" + System.currentTimeMillis();
		String username 	= null;   
		String passwd 		= null;   
		String topic 		= "#"; // all topics in the MQTT network
		
		MQTT mqtt = new MQTT(MqttServer, client_id, username, passwd);
		mqtt.init(topic);

		sleep(1000);

		mqtt.subscribe(topic, 1);
	}

	public static void sleep(int time){
		try {
			Thread.sleep(time);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	@SuppressWarnings("unchecked")
	public static void postwork(String topic, MqttMessage mqttMessage) throws NoSuchAlgorithmException {
		String strUrl = "http://192.168.1.12:5000/api/org.oslab.ac.kr.COVIDAsset";
		JSONParser parser = new JSONParser();
		try {
			JSONObject mainJson = new JSONObject();
			JSONObject mqttMsgJson = (JSONObject) parser.parse(new String(mqttMessage.getPayload(), "UTF-8"));
			
			String cid = sha256("110, Sejong-daero, Jung-gu, Seoul, Republic of Korea" + mqttMsgJson.toJSONString()); // SHA256(the address of Seoul City Hall + Health data from SmartWatch) 
			String pid = sha256(topic);
			JSONObject covid = new JSONObject();
			covid.put("$class", "org.oslab.ac.kr.COVID");
			JSONObject patientId = new JSONObject();
			patientId.put("$class", "org.oslab.ac.kr.Patients");
			patientId.put("PatientId", pid);
			covid.put("patientId", patientId);
			covid.put("name", topic);
			covid.put("birthDate", "1988.01.29");
			covid.put("infection", "Y");
			Date date_now = new Date(System.currentTimeMillis());
			covid.put("date", date_now.toString());
			covid.put("travelRoute", "110, Sejong-daero, Jung-gu, Seoul, Republic of Korea");
			covid.put("note", mqttMsgJson.toJSONString());
			
			mainJson.put("$class", "org.oslab.ac.kr.COVIDAsset");
			mainJson.put("COVIDId", cid);
			mainJson.put("covid", covid);
			
			post(strUrl, mainJson.toJSONString());
		} catch (UnsupportedEncodingException e1) {
			e1.printStackTrace();
		} catch (ParseException e1) {
			e1.printStackTrace();
		}
	}
	
	public static void post(String strUrl, String jsonMessage){
		try {
			URL url = new URL(strUrl);
			HttpURLConnection con = (HttpURLConnection) url.openConnection();
			con.setConnectTimeout(5000); //������ ����Ǵ� Timeout �ð� ����
			con.setReadTimeout(5000); // InputStream �о� ���� Timeout �ð� ����

			con.setRequestMethod("POST");

			//json���� message�� �����ϰ��� �� �� 
			con.setRequestProperty("Content-Type", "application/json");
			con.setDoInput(true);
			con.setDoOutput(true);
			con.setUseCaches(false);
			con.setDefaultUseCaches(false);

			OutputStreamWriter wr = new OutputStreamWriter(con.getOutputStream());
			wr.write(jsonMessage); //json ������ message ���� 
			wr.flush();

			StringBuilder sb = new StringBuilder();
			if (con.getResponseCode() == HttpURLConnection.HTTP_OK) {
				BufferedReader br = new BufferedReader(new InputStreamReader(con.getInputStream(), "utf-8"));
				String line;
				while ((line = br.readLine()) != null) {
					sb.append(line).append("\n");
				}
				br.close();
				System.out.println("" + sb.toString());
			} else {
				System.out.println(con.getResponseMessage());
			}
		} catch (Exception e){
			System.err.println(e.toString());
		}
	}
	
    /**
     * SHA-256���� �ؽ��ϴ� �޼ҵ�
     * 
     * @param bytes
     * @return
     * @throws NoSuchAlgorithmException 
     */
    public static String sha256(String msg) throws NoSuchAlgorithmException {
        MessageDigest md = MessageDigest.getInstance("SHA-256");
        md.update(msg.getBytes());
        
        return bytesToHex(md.digest());
    }
 
 
    /**
     * ����Ʈ�� �������� ��ȯ�Ѵ�
     * 
     * @param bytes
     * @return
     */
    public static String bytesToHex(byte[] bytes) {
        StringBuilder builder = new StringBuilder();
        for (byte b: bytes) {
          builder.append(String.format("%02x", b));
        }
        return builder.toString();
    }
}
