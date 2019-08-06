#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#include "WIFI_info.h" //Our HTML webpage contents

#ifndef STASSID
#define STASSID ""
#define STAPSK  ""
#endif

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK  "pass123"
#endif

#define  version_info 4

const char* ssid = STASSID;
const char* password = STAPSK;

const char* ap_ssid = APSSID;
const char* ap_password = APPSK;

bool storedConfigurationExists = false;

ESP8266WebServer server(80);

//Check if header is present and correct
bool is_authenticated() {
  Serial.println("Enter is_authenticated");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentication Successful");
      return true;
    }
  }
  Serial.println("Authentication Failed");
  return false;
}

String getContentType(String filename) {
    if (server.hasArg("download")) return "application/octet-stream";
    else if (filename.endsWith(".html")) return "text/html";
    else if (filename.endsWith(".css")) return "text/css";
    else if (filename.endsWith(".js")) return "application/javascript";
    else if (filename.endsWith(".png")) return "image/png";
    else if (filename.endsWith(".gif")) return "image/gif";
    else if (filename.endsWith(".jpg")) return "image/jpeg";
    else if (filename.endsWith(".svg")) return "image/svg+xml";
    else if (filename.endsWith(".ico")) return "image/x-icon";
    else if (filename.endsWith(".pdf")) return "application/x-pdf";
    else if (filename.endsWith(".zip")) return "application/x-zip";
    else if (filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
}

bool handleFileRead(String path) {
    String contentType = getContentType(path);
    String pathWithGz = path + ".gz";
    Serial.print("handle file ");
    Serial.print(path);

    if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
        if (SPIFFS.exists(pathWithGz)) {
            path += ".gz";
            Serial.print(".gz");
        }
        File file = SPIFFS.open(path, "r");
        size_t sent = server.streamFile(file, contentType);
        file.close();
        Serial.println(" success");
        return true;
    }
    Serial.println(" failed");
    return false;
}

//login page, also called for disconnect
void handleLogin() {
  String msg;
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
  }
  if (server.hasArg("DISCONNECT")) {
    Serial.println("Disconnection");
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=0");
    server.send(301);
    return;
  }
  if (server.hasArg("USERNAME") && server.hasArg("PASSWORD")) {
    if (server.arg("USERNAME") == "admin" &&  server.arg("PASSWORD") == "admin") {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;
    }
    msg = "Wrong username/password! try again.";
    Serial.println("Log in Failed");
  }
  // String content = "<html><body><form action='/login' method='POST'>To log in, please use : admin/admin<br>";
  // content += "User:<input type='text' name='USERNAME' placeholder='user name'><br>";
  // content += "Password:<input type='password' name='PASSWORD' placeholder='password'><br>";
  // content += "<input type='submit' name='SUBMIT' value='Submit'></form>" + msg + "<br>";
  // content += "You also can go <a href='/inline'>here</a></body></html>";

/*
  String content = LOGIN_page;

  server.send(200, "text/html", content);
*/

    if ( ! handleFileRead("/login.html")) {
        server.send(404, "text/plain", "FileNotFound");
        return;
    }

}

//root page can be accessed only if authentication is ok
void handleRoot() {
  Serial.println("Enter handleRoot");
  String header;
  if (!is_authenticated()) {
    server.sendHeader("Location", "/login");
    server.sendHeader("Cache-Control", "no-cache");
    server.send(301);
    Serial.println("redirect to /login");
    return;
  }
  // String content = "<html><body><H2>hello, you successfully connected to esp8266!</H2><br>";
  // if (server.hasHeader("User-Agent")) {
  //   content += "the user agent used is : " + server.header("User-Agent") + "<br><br>";
  // }
  // content += "You can access this page until you <a href=\"/login?DISCONNECT=YES\">disconnect</a></body></html>";

    if ( ! handleFileRead("/main.html")) {
        server.send(404, "text/plain", "FileNotFound");
        return;
    }

}

//no need authentication
void handleStateSwitch() {

    if ( server.method() == HTTP_POST ) {
        handleSwitchPost();
    }

  String message = "switch \n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  Serial.println("handleSwitch " + message);

  server.send(200, "text/plain", message);
}

/**
 * Process modification state for switch
 * */
void handleSwitchPost() {

    String state = "";
    String message = "";
    
    for (uint8_t i = 0; i < server.args(); i++) {
        if ( server.argName(i) == "state") {
            state = server.arg(i);
        }
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    if ( state != "" ) {
        message += "\n state assigned to " + state;

        if ( state == "on" ) {
            digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
        } else if ( state == "off" ) {
            digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off (Note that LOW is the voltage level
        }
    }

  Serial.println("handleSwitchPost " + message);
    
}


void handleNotFound() {

    if ( handleFileRead(server.uri())) {
        return;
    }

  
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

void readStoredConfiguration() {

}

void activateAccessPoint() {
    Serial.println();
    Serial.print("Configuring access point...");
/* You can remove the password parameter if you want the AP to be open. */
    WiFi.softAP(ap_ssid, ap_password);

    IPAddress myIP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(myIP);
}

void initFileSystem() {
  // Инициализация FFS
 SPIFFS.begin();
 {
 Dir dir = SPIFFS.openDir("/");
  Serial.println("files");
 while (dir.next()) {
 String fileName = dir.fileName();
 size_t fileSize = dir.fileSize();
  Serial.print(fileName);
  Serial.print(" ");
  Serial.println(fileSize);
 }
 }
}

void setup(void) {

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output

  Serial.begin(9600);
  Serial.println();
  Serial.print("setup. Version ");
  Serial.println(version_info);

  initFileSystem();


  readStoredConfiguration();

  WiFi.persistent(false);

  if ( ! storedConfigurationExists ) {

      activateAccessPoint();

  } else {

      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      Serial.println("");

      // Wait for connection
      int waitCount = 20;
      while ( WiFi.status() != WL_CONNECTED ) {
          delay(500);
          Serial.print(".");
          waitCount--;

          if ( waitCount <= 0 ) {
              activateAccessPoint();
              break;
          }
      }
  }

  if ( WiFi.status() != WL_CONNECTED ) {
      Serial.println("");
      Serial.print("Accesspoint started ");
      Serial.println(ap_ssid);
      Serial.print("Accesspoint password ");
      Serial.println(ap_password);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
  } else {
      Serial.println("");
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
  }

  server.on("/", handleRoot);
  server.on("/login", handleLogin);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works without need of authentication");
  });

//  server.on("/css/bootstrap.min.css", []() {
//      server.send(200, "text/plain", RES_bootstrap_min_css);
//  });
//
//  server.on("/js/bootstrap.min.js", []() {
//      server.send(200, "text/plain", RES_bootstrap_min_js);
//  });
//
//  server.on("/js/jquery.min.js", []() {
//      server.send(200, "text/plain", REST_jquery_min_js);
//  });
//
  server.on("/api/v1/state/switch", handleStateSwitch);

  server.onNotFound(handleNotFound);
  //here the list of headers to be recorded
  const char * headerkeys[] = {"User-Agent", "Cookie"} ;
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);
  //ask server to track these headers
  server.collectHeaders(headerkeys, headerkeyssize);
  server.begin();
  Serial.println("HTTP server started");
}


void loop(void) {
  server.handleClient();
}
