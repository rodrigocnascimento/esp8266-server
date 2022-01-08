#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "";
const char* password = "";
HTTPClient http;

// instantiate thw server on the required port
WiFiServer server(1337);

String header;

// Current time
unsigned long currentTime = millis();

// Previous time
unsigned long previousTime = 0;

// Define timeout in milliseconds
unsigned long timeoutTime = 2000;

void openWiFiConnection() {
  Serial.printf("Connecting on %s", ssid);
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi OK");
  Serial.print("IP addr: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  openWiFiConnection();

  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) {
    Serial.println("Incomming client");

    String currentLine = "";

    previousTime = currentTime;
    
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();
      
      if (client.available()) { // if there's bytes to read from the client
        char c = client.read(); // read a byte, then

        Serial.write(c); // print it out the serial monitor
        header += c;

        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel=\"icon\" href=\"data:,\">");
            client.println("<title>CHAPPIE8266E12v0.1-pre_alpha</title>");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; }</style>");
            client.println("</head><body>");
            client.println("<h1>I'm Chappie Web Server!</h1>");
            client.println("<small>CHAPPIE8266E12v0.1-pre_alpha</small>");
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }

    // Clear the header value
    header = "";

    // Stop the client
    client.stop();
    Serial.println("client disconnected .");
    Serial.println();
  }
}