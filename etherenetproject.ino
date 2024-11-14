#include <SPI.h>
#include <Ethernet.h>

// MAC address for your Ethernet shield (you can generate one online if needed)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// IP address for the Arduino (make sure it's within your network's range)
IPAddress ip(172, 16, 57, 27);

// Create an Ethernet server on port 80
EthernetServer server(80);

// Variable to store the state of the LED
int ledState = LOW;

void setup() {
  // Initialize pin 13 (built-in LED) as output
  pinMode(13, OUTPUT);
  
  // Start the Ethernet connection and the server
  Ethernet.begin(mac, ip);
  server.begin();
  
  Serial.begin(9600);  // For debugging
  Serial.print("Server is at ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // Listen for incoming clients
  EthernetClient client = server.available();
  
  if (client) {
    Serial.println("New client connected");
    boolean currentLineIsBlank = true;
    String request = "";
    
    // Read the HTTP request from the client
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // If we've reached the end of the request (a blank line), process it
        if (c == '\n' && currentLineIsBlank) {
          // Check what the client requested (on or off)
          if (request.indexOf("GET /LED=ON") != -1) {
            ledState = HIGH;  // Turn the LED on
          } 
          else if (request.indexOf("GET /LED=OFF") != -1) {
            ledState = LOW;   // Turn the LED off
          }
          digitalWrite(13, ledState);  // Update LED based on request

          // Send an HTTP response back to the client
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<html>");
          client.println("<head><title>Arduino LED Control</title></head>");
          client.println("<body>");
          client.println("<h1>Control LED from Web Server</h1>");
          client.println("<p><a href=\"/LED=ON\">Turn LED ON</a></p>");
          client.println("<p><a href=\"/LED=OFF\">Turn LED OFF</a></p>");
          client.println("</body>");
          client.println("</html>");
          
          // Break out of the loop
          break;
        }
        
        // Keep track of blank lines to detect the end of the request
        if (c == '\n') {
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    // Close the connection after sending the response
    client.stop();
    Serial.println("Client disconnected");
  }
}
