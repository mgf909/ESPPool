
#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define TESTBED 1 //Set to 1 for benchtest hardware - different id, parameters and Temp sensor addresses.

#define ONE_WIRE_BUS 21  // DS18B20 on arduino pin2 corresponds to D4 on physical board
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature dallasSensors(&oneWire);

// Update these with values suitable for your network.
char* ssid = "test2";
const char* password = "not1welcome";
const char* mqtt_server = "hal.lions.home";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
//float temp;

//set Testing vs Production variables and settings
#if TESTBED == 1
#define SKETCH_NAME "ESPPool Controller"
#define SKETCH_VERSION "2017.10.05t1"
int desiredPoolTemp = 31; //set initial desired/max pool temperature - this can be overwritten by the controller
int differential = 2; // Set the differential to switch on the pumps. In production this should be about 15, on testbed 2
int acidpumpOnTime = 5; //default minutes on the hour to run the acid feeder for - this can be overwritten by the controller
int mqttReportFreq = 3000; //how many millis to send data to MQTT
#define SummerPumpRunTime 25000  // 28800 seconds = 8 hours, 36000 = 10 HOURS
#define WinterPumpRunTime 14400 // 4 Hours only during winter. - Winter could also be enabled when pooltemp drops to 18deg
						 // Set Temp Sensor addresses - i use the ds18b example sketch to identify the addresses.
DeviceAddress roofTsensor = {
	0x28, 0x14, 0xFE, 0x1A, 0x04, 0x0, 0x0, 0x20 };
	//0x28, 0x61, 0x17, 0x1B, 0x04, 0x00, 0x00, 0xF5 }; real testbed
DeviceAddress poolTsensor = {
	0x28, 0xCF, 0x38, 0x1B, 0x04, 0x0, 0x0, 0x2D };
	//0x28, 0xEB, 0x68, 0xE3, 0x02, 0x00, 0x00, 0x6E }; real testbed
#define pooltemp_topic "testpool/pooltemp"
#define rooftemp_topic "testpool/rooftemp"


#endif

#if TESTBED == 0
#define SKETCH_NAME "Pool Controller"
#define SKETCH_VERSION "2016.10.29 r1"
int desiredPoolTemp = 27; //set initial desired/max pool temperature - this can be overwritten by the controller
int differential = 15; // Set the differential to switch on the pumps. In Production this should be about 15, on testbed 0
int acidpumpOnTime = 05; //how many minutes on the hour to run the acid feeder for.
int mqttReportFreq = 60; //how many cycles to ask vera for data
#define SummerPumpRunTime 28800  // 28800 seconds = 8 hours, 36000 = 10 HOURS
#define WinterPumpRunTime 14400 // 4 Hours only during winter. - Winter could also be enabled when pooltemp drops to 18deg
						 // Set Temp Sensor addresses
DeviceAddress roofTsensor = {
	0x28, 0x76, 0x30, 0xE3, 0x02, 0x00, 0x00, 0x3E };
DeviceAddress poolTsensor = {
	0x28, 0xBE, 0xFF, 0x0C, 0x02, 0x00, 0x00, 0x9E };
#endif



void setup_wifi() {
	delay(100);
	// We start by connecting to a WiFi network
	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}
	randomSeed(micros());
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
}





void setup() {
	Serial.begin(115200);
	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);

	// Start up the Dallas Temp library and set the resolution to 9 bit
	dallasSensors.begin();
	dallasSensors.setResolution(poolTsensor, 12);
	dallasSensors.setResolution(roofTsensor, 12);
	

	
}





void loop() {
	if (!client.connected()) {
		reconnect();
	}
	client.loop();

	long now = millis();
		
	

	
	
	
	// Publish metrics to MQTT periodically
	if (now - lastMsg > mqttReportFreq) {
		lastMsg = now;
		

//	msg = msg + " C ";
//		char message[58];
//		msg.toCharArray(message, 58);
//		Serial.println(message);

// Get temperature values
  dallasSensors.requestTemperatures();
  float poolTemp = dallasSensors.getTempC(poolTsensor); //should only really do this when the pump is runnign as the pipes can be different temp to the pool. Would need to set inital value and turn on pump if its set..
  float roofTemp = dallasSensors.getTempC(roofTsensor);


Serial.print ("Pool Temp is: ");
Serial.println (poolTemp);
Serial.print ("Roof Temp is: ");
Serial.println (roofTemp);

		//publish sensor data to MQTT broker
	//	client.publish(pooltemp_topic, String(poolTemp).c_str(), true);
	//	client.publish(rooftemp_topic, String(roofTemp).c_str(), true);
		
	}
}
