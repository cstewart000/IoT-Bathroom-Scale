#include <HX711.h>
#include <ESP8266WiFi.h>

// Hardware configuration
#define DOUT D3  // Pin connected to HX711 data output pin
#define CLK  D6  // Pin connected to HX711 clk pin


// Network configuration
const char* ssid     = "HUAWEI-2XCNAA";   //replace with your own SSID
const char* password = "95749389";    //replace with your own password
const int httpPort = 80;


// Service
const char* host = "api.pushingbox.com";
String url = "/pushingbox?";
const char* dev_id = "v158F5B9B79D68EA";


#define NUM_MEASUREMENTS 20 // Number of measurements
#define THRESHOLD 0      // Measures only if the weight is greater than 2 kg. Convert this value to pounds if you're not using SI units.
#define THRESHOLD1 0.5  // Restart averaging if the weight changes more than 0.5 kg.
#define calibration_factor 1

//Initialise objects
HX711 scale(DOUT, CLK);

// TODO 
WiFiClient client;


float weight = 0.0;
float prev_weight = 0.0;


void setup() {
  // put your setup code here, to run once:
  // start the serial connection
  Serial.begin(115200);
  scale.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0
  initialiseWifiAndPrintSerial();

}

void loop() {
  // put your main code here, to run repeatedly:
  //WiFiClient client;
  float wt = getWeightFromScale();
  Serial.println(wt);

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }


  //TODO refactor
  url += "devid=";
  url += "dev_id";
  url += "&data=" + String(wt);

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
    Serial.print("Data Sent!");
  }

  Serial.println();
  Serial.println("closing connection");
}

void initialiseWifiAndPrintSerial() {

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

float getWeightFromScale() {
  float weight = scale.get_units();
  Serial.println(weight);
  float avgweight = 0;

  /*
    if (weight > THRESHOLD) { // Takes measures if the weight is greater than the threshold
    float weight0 = scale.get_units();
    for (int i = 0; i < NUM_MEASUREMENTS; i++) {  // Takes several measurements
    delay(100);
    weight = scale.get_units();
    avgweight = avgweight + weight;
    if (abs(weight - weight0) > THRESHOLD1) {
      avgweight = 0;
      i = 0;
    }
    weight0 = weight;
    }
    avgweight = avgweight / NUM_MEASUREMENTS; // Calculate average weight

    Serial.print("Measured weight: ");
    Serial.print(avgweight, 1);
    Serial.println(" kg");


    }*/

  return weight;
}

