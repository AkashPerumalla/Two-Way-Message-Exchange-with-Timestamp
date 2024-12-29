//NodeMCU B

#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <SoftwareSerial.h>

// Wi-Fi credentials (optional, used if you want to connect Wi-Fi for logging or other purposes)
const char* ssid = "<SSID>";
const char* password = "<PASSWORD>>";

// NTP Client Setup (for IST)
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 19800, 3600000); // NTP server, IST offset (19800 seconds)

// HC-12 communication pins
const int hc12_tx = 5; // D1 (GPIO5)
const int hc12_rx = 4; // D2 (GPIO4)

SoftwareSerial hc12(hc12_rx, hc12_tx); // Create a SoftwareSerial object for HC-12

void setup() {
  Serial.begin(115200);  // Initialize Serial Monitor communication
  hc12.begin(9600); // Initialize HC-12 communication

  // Connect to Wi-Fi (optional, if needed for logging or other purposes)
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize NTP Client
  timeClient.begin();
  Serial.println("NodeMCU B ready to transmit and receive messages.");
}

void loop() {
  // Update time from NTP
  timeClient.update();

  // Check if data is available from HC-12 (i.e., received from NodeMCU A)
  if (hc12.available()) {
    String receivedMessage = hc12.readStringUntil('\n'); // Read incoming message
    String currentTime = timeClient.getFormattedTime(); // Get current time in HH:MM:SS format
    String timestampedMessage = "NodeMCU B [" + currentTime + " IST]: " + receivedMessage;

    // Output the received message with timestamp to Serial Monitor
    Serial.println("Received: " + timestampedMessage);
  }

  // Check if there is a new message from Serial Monitor to send to NodeMCU A
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n'); // Read input from Serial Monitor
    String currentTime = timeClient.getFormattedTime(); // Get current time in HH:MM:SS format
    String timestampedMessage = "NodeMCU B [" + currentTime + " IST]: " + message;

    // Send the timestamped message to NodeMCU A via HC-12
    hc12.println(timestampedMessage);  // Send the message over HC-12

    Serial.println("Sent: " + timestampedMessage);  // Output the sent message to Serial Monitor
  }

  delay(100); // Short delay for stability
}
