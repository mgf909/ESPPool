
void reconnect() {
	// Loop until we're reconnected
	while (!client.connected())
	{
		Serial.print("Attempting MQTT connection...");
		// Create a random client ID
		String clientId = "ESP8266Client-";
		clientId += String(random(0xffff), HEX);
		// Attempt to connect
		//if you MQTT broker has clientID,username and password
		//please change following line to    if (client.connect(clientId,userName,passWord))
		if (client.connect(clientId.c_str()))
		{
			Serial.println("connected");
			//once connected to MQTT broker, subscribe command if any
			client.subscribe("poolCommand");
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			// Wait 6 seconds before retrying
			delay(6000);
		}
	}
} //end reconnect()



void callback(char* topic, byte* payload, unsigned int length)
{
	Serial.print("Command is : [");
	Serial.print(topic);
	int p = (char)payload[0] - '0';
	//	int chk = DHT.read11(DHT11_PIN);
	// if MQTT comes a 0 message, show humidity
	if (p == 0)
	{
		Serial.println("to show humidity!]");
		Serial.print(" Humidity is: ");
		//	Serial.print(DHT.humidity, 1);
		Serial.println('%');
	}
	// if MQTT comes a 1 message, show temperature
	if (p == 1)
	{
		// digitalWrite(BUILTIN_LED, HIGH);
		Serial.println(" is to show temperature!] ");
		//	int chk = DHT.read11(DHT11_PIN);
		Serial.print(" Temp is: ");
		//	Serial.print(DHT.temperature, 1);
		Serial.println(' C');
	}
	Serial.println();
} //end callback