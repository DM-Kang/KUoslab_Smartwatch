package kr.ac.korea.oslab.kuhealthapi;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;

import org.json.simple.JSONObject;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class GetMqttInfo {
	private static int deviceCnt = 0;
	private final int portNum = 1883;

	@SuppressWarnings("unchecked")
	@GetMapping("/KUHealth/GetMqttInfo")
	public String GetMqttInfo_func() {
		// TODO Implementation of Device Authentication Logic

		/*
		 * try { Runtime rt = Runtime.getRuntime(); Process proc =
		 * rt.exec("sudo mosquitto -c /etc/");
		 * 
		 * InputStream is = proc.getInputStream(); InputStreamReader isr = new
		 * InputStreamReader(is); BufferedReader br = new BufferedReader(isr);
		 * 
		 * String line; while((line=br.readLine())!= null){ System.out.println(line);
		 * System.out.flush(); } } catch (IOException e) { e.printStackTrace(); }
		 */
		
		// getting the AWS Public DNS of the instance 
		String publicDNS = "tcp://" + http_get("http://169.254.169.254/latest/meta-data/public-hostname");
		String portNumStr = "" + portNum;
	
		JSONObject mainJson = new JSONObject();
		mainJson.put("mqtt_uri", publicDNS);
		mainJson.put("mqtt_port", portNumStr);
		mainJson.put("uri_with_port", publicDNS + ":" + portNumStr);

		return mainJson.toString().replace("\\", "");
	}

	public String http_get(String strUrl) {
		try {
			URL url = new URL(strUrl);
			HttpURLConnection con = (HttpURLConnection) url.openConnection(); 
			con.setConnectTimeout(5000); //서버에 연결되는 Timeout 시간 설정
			con.setReadTimeout(5000); // InputStream 읽어 오는 Timeout 시간 설정
			//con.addRequestProperty("x-api-key", RestTestCommon.API_KEY); //key값 설정

			con.setRequestMethod("GET");

			// URLConnection에 대한 doOutput 필드값을 지정된 값으로 설정한다. URL 연결은 입출력에 사용될 수 있다. 
			// URL 연결을 출력용으로 사용하려는 경우 DoOutput 플래그를 true로 설정하고, 그렇지 않은 경우는 false로 설정해야 한다. 
			// 기본값은 false이다.
			con.setDoOutput(false); 

			StringBuilder sb = new StringBuilder();
			if (con.getResponseCode() == HttpURLConnection.HTTP_OK) {
				BufferedReader br = new BufferedReader(
						new InputStreamReader(con.getInputStream(), "utf-8"));
				String line;
				while ((line = br.readLine()) != null) {
					sb.append(line);
				}
				br.close();
				//System.out.println("" + sb.toString());
				return sb.toString();
			} else {
				//System.out.println(con.getResponseMessage());
				return "-1";
			}

		} catch (Exception e) {
			//System.err.println(e.toString());
			return "-1";
		}
	}
}
