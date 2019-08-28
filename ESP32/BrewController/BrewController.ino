// Include the libraries we need
#include <EEPROM.h>
#include <ESPmDNS.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <SimpleCLI.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

//WiFi Definitons


struct BrewControllerConfiguration {
  char ssid[24];
  char password[32];
  DeviceAddress heaterSensor;
  DeviceAddress mashSensor;
};

// Pin Definitions
#define HEATING_POS 17
#define HEATING_NEG 16
#define RECIRC_POS 19
#define RECIRC_NEG 18

// Create CLI Object
SimpleCLI cli;

// Commands
Command setTemp;
Command getTemp;
Command setWifi;
Command getSensors;
Command setHeaterSensor;
Command setMashSensor;

// Data wire is plugged into port 15 on the ESP32
#define ONE_WIRE_BUS 15

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress heaterThermometer;
DeviceAddress mashThermometer;

DeviceAddress unassignedSensors[8];

float MashTemp = 153;
float temp = 32;
float heaterTemp = 32;
bool heating = false;
bool recirculating = false;
time_t startMash = 0;
time_t startBoil = 0;

enum State {
  Heating,
  Holding,
  Boiling
};

State currentState = Heating;

WebServer server(80);

/*
 * Setup function. Here we do the basics
 */
void setup(void)
{
  // start serial port
  Serial.begin(9600);

  EEPROM.begin(sizeof(BrewControllerConfiguration));

  BrewControllerConfiguration StartupConfiguration;
  EEPROM.get(0, StartupConfiguration);

  printConfiguration(StartupConfiguration);

  startWifi(StartupConfiguration.ssid, StartupConfiguration.password, false);

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  int numSensors = sensors.getDeviceCount();

  int unassignedCount = 0;
  // Discover available sensors and check them against the assignments.
  for (int i = 0; i < numSensors; i++){
    DeviceAddress currentSensor;

    if (!sensors.getAddress(currentSensor, i)) {
      Serial.print("Found a sensor on the bus but was unable to retrieve an address for sensor:"); 
      Serial.println(i);
    }

    if(!addressMatch(currentSensor, StartupConfiguration.heaterSensor) && !addressMatch(currentSensor, StartupConfiguration.mashSensor)){
      Serial.print("Discovered unassigned sensor: ");
      printAddress(currentSensor);
      Serial.println();
      memcpy(&unassignedSensors[unassignedCount], &currentSensor, 8);
      unassignedCount++;
    }
    else if (currentSensor == StartupConfiguration.heaterSensor)
    {
      // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(heaterThermometer, 9);
    } else if (currentSensor == StartupConfiguration.mashSensor){
      // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
      sensors.setResolution(heaterThermometer, 9);
    }
  }

  memcpy(mashThermometer, StartupConfiguration.mashSensor, 8);
  memcpy(heaterThermometer, StartupConfiguration.heaterSensor, 8);

  setTemp = cli.addSingleArgCmd("settemp", setTempCallback);
  getTemp = cli.addCmd("gettemp", getTempCallback);
  setWifi = cli.addCmd("setwifi", setWifiCallback);
  setWifi.addArg("ssid");
  setWifi.addArg("password");
  getSensors = cli.addSingleArgCmd("getsensors", getSensorsCallback);
  setMashSensor = cli.addSingleArgCmd("setmashsensor", setMashSensorCallback);
  setHeaterSensor = cli.addSingleArgCmd("setHeaterSensor", setHeaterSensorCallback);

  pinMode(HEATING_POS, OUTPUT);
  digitalWrite(HEATING_POS, LOW);
  
  pinMode(HEATING_NEG, OUTPUT);
  digitalWrite(HEATING_NEG, HIGH);

  pinMode(RECIRC_POS, OUTPUT);
  digitalWrite(RECIRC_POS, LOW);
  
  pinMode(RECIRC_NEG, OUTPUT);
  digitalWrite(RECIRC_NEG, HIGH);
}

//Setup WiFi
void startWifi(char* ssid, char* password, bool restart) {
  if(restart){
    server.stop();
    WiFi.disconnect();
    //Wait for disconnect
    while (WiFi.status() == WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  int connectionTimeout = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if(connectionTimeout++ > 10)
    return;
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/setpoint/{}", ServerSetMashTemp);

  server.on("/state/{}", ServerSetBrewState);

  server.on("/temp", ServerGetTemp);

  server.on("/status", ServerGetStatus);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void ServerSetMashTemp(){
  String MashTempString = server.pathArg(0);
  setMashTemp(MashTempString);

  server.send(200, "text/plain", "Temperature set to: " + MashTempString);
}

void ServerGetTemp(){
  server.send(200, "text/plain", 
    "Current Mash Temperature: " + String(temp, 3) + "\n" +
    "Current Heater Termperature:" + String(heaterTemp, 3)
  );
}

void ServerGetStatus(){
  server.send(200, "text/plain", 
    "Current Mash Temperature: " + String(temp, 3) + "\n" +
    "Current Heater Temperature: " + String(heaterTemp, 3) + "\n" +
    "SetPoint " + String(MashTemp, 3) + "\n" + 
    "Recirculating: " + String(recirculating) + "\n" +
    "Heating: " + String(heating) + "\n"
  );
}

void ServerSetBrewState(){
  String State = server.pathArg(0);

  if(State == "Boil"){
    currentState = Boiling;
  } else {
    currentState = Heating;
  }

}

void handleRoot() {
  server.send(200, "text/plain", "Hello from BrewController!");
}

void handleNotFound() {
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
}

// function to print the temperature for a device
float getTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    return 0;
  }
  float tempF = DallasTemperature::toFahrenheit(tempC);

  return tempF;
}

/*
 * Main function. It will request the tempC from the sensors and display on Serial.
 */
int printCounter = 0;
void loop(void)
{ 
  server.handleClient();

  sensors.requestTemperatures(); // Send the command to get temperatures
  float tmpTemp = getTemperature(mashThermometer); // Use a simple function to grab the data
  float tmpHeaterTemp = getTemperature(heaterThermometer);

  //deal with errors
  if(tmpTemp != 0){
    temp = tmpTemp;
  }

  if(tmpHeaterTemp != 0){
    heaterTemp = tmpHeaterTemp;
  }

  switch (currentState){
    case Heating:
      if(temp < MashTemp){
        if(!heating){
          Serial.println("Enabling heaters below temp");  
        }
        enableHeater(true);
        enableRecirc(true);
      } else {
        if(heating){
          Serial.println("Reached temp, beginning to hold.");
        }
        enableHeater(false);
        enableRecirc(false);
        currentState = Holding;
      }
      break;
    case Holding:
      if(temp < MashTemp-2){
        if(!recirculating){
          enableRecirc(true);
        }

        if(!heating && (heaterTemp < (MashTemp+2))){
          Serial.println("Temperature is low.  Starting heater with 30 second delay");
          enableHeater(true);
        } else if(heating && (heaterTemp > (MashTemp+2))) {
          Serial.println(" Heater temp is:" + String(heaterTemp, 3) + ". Disabling heater.");
          enableHeater(false);
        }
      } else if ( temp > (MashTemp + 3) ){
        if(heating){
          Serial.println("Overheated, disabling heater");
          enableHeater(false);
        }
        if(!recirculating){
          Serial.println("Enabling recirc to cool");
          enableRecirc(true);
        }
      } else if ( (temp > (MashTemp + 1)) && !(temp > (MashTemp + 3))){
        if(heating){
          Serial.println("Warmed back up, disabling recirc and heater");
          enableHeater(false);
          enableRecirc(false);
        } else if(recirculating) {
          Serial.println("temp is:" + String(temp, 3) + "Back to the sweetspot, disabling recirc");
          enableRecirc(false);
        }
      }
      break;
    case Boiling:
      if(!heating){
        Serial.println("Heater currently disabled.  Enabling to boil.");
        enableHeater(true);
      }
      if(recirculating){
        enableRecirc(false);
      }
      // if((temp > 210) && (startBoil == 0)){
      //   startBoil = gettime
      // }
  }

  // Check if user typed something into the serial monitor
  if (Serial.available()) {
    // Read out string from the serial monitor
    String input = Serial.readStringUntil('\n');

    // Echo the user input
    Serial.print("# ");
    Serial.println(input);
    
    // Parse the user input into the CLI
    cli.parse(input);
  }
  
  if (cli.errored()) {
    CommandError cmdError = cli.getError();
    
    Serial.print("ERROR: ");
    Serial.println(cmdError.toString());
    
    if (cmdError.hasCommand()) {
        Serial.print("Did you mean \"");
        Serial.print(cmdError.getCommand().toString());
        Serial.println("\"?");
    }
  }
}

// Callback for setting temperature serial command
void setTempCallback(cmd* c) {
  Command cmd(c);

  Argument arg = cmd.getArgument(0);

  String MashTempStr = arg.getValue();

  setMashTemp(MashTempStr);
}

void setMashTemp(String tempStr){
  MashTemp = tempStr.toFloat();

  Serial.print("Setting MashTemperature to: ");
  Serial.println(MashTemp);
}

void getTempCallback(cmd* c) {
  Serial.print("Temperature: ");
  Serial.println(temp);
}

// Callback for setting WiFi Configuration
void setWifiCallback(cmd* c) {
  Command cmd(c);

  Argument ssidArg = cmd.getArgument(0);
  String ssidStr = ssidArg.getValue();
  char ssid[32];
  ssidStr.toCharArray(ssid, 32);
  Argument passwordArg = cmd.getArgument(1);
  String passwordStr = passwordArg.getValue();
  char password[32];
  passwordStr.toCharArray(password, 32);

  BrewControllerConfiguration StartupConfiguration;
  EEPROM.readBytes(0, &StartupConfiguration, sizeof(BrewControllerConfiguration));

  strcpy(StartupConfiguration.ssid, ssid);
  strcpy(StartupConfiguration.password, password);

  Serial.println("Putting configuration in EEPROM: ");
  Serial.println(sizeof(BrewControllerConfiguration));
  printConfiguration(StartupConfiguration);

  EEPROM.writeBytes(0, &StartupConfiguration, sizeof(BrewControllerConfiguration));
  EEPROM.commit();

  startWifi(ssid, password, true);
}

// Callback for getting unassigned sensors state.
void getSensorsCallback(cmd* c) {
  for(int i = 0; i < 8; i++){
    float temperature = getTemperature(unassignedSensors[i]);
    Serial.print(i);
    Serial.print(" Sensor: ");
    printAddress(unassignedSensors[i]);
    Serial.print(" Temperature: ");
    Serial.println(temperature);
    // if(!addressNull(unassignedSensors[i])){

    // }
  }
}

void setMashSensorCallback(cmd* c){
  Command cmd(c);

  Argument sensorIndexArg = cmd.getArgument(0);
  String sensorIndexStr = sensorIndexArg.getValue();
  int sensorIndex = sensorIndexStr.toInt();

  //Get the current Saved config
  BrewControllerConfiguration StartupConfiguration;
  EEPROM.readBytes(0, &StartupConfiguration, sizeof(BrewControllerConfiguration));

  memcpy(StartupConfiguration.mashSensor, unassignedSensors[sensorIndex], 8);
  memcpy(mashThermometer, unassignedSensors[sensorIndex], 8);

  Serial.println("Putting configuration in EEPROM: ");
  Serial.println(sizeof(BrewControllerConfiguration));
  printConfiguration(StartupConfiguration);


  //Write the new sesnor config to the EEPROM 
  EEPROM.writeBytes(0, &StartupConfiguration, sizeof(BrewControllerConfiguration));
  EEPROM.commit();
}

void setHeaterSensorCallback(cmd* c){
  Command cmd(c);

  Argument sensorIndexArg = cmd.getArgument(0);
  String sensorIndexStr = sensorIndexArg.getValue();
  int sensorIndex = sensorIndexStr.toInt();

  //Get the current Saved config
  BrewControllerConfiguration StartupConfiguration;
  EEPROM.readBytes(0, &StartupConfiguration, sizeof(BrewControllerConfiguration));

  memcpy(StartupConfiguration.heaterSensor, unassignedSensors[sensorIndex], 8);
  memcpy(heaterThermometer, unassignedSensors[sensorIndex], 8);

  Serial.println("Putting configuration in EEPROM: ");
  Serial.println(sizeof(BrewControllerConfiguration));
  printConfiguration(StartupConfiguration);


  //Write the new sesnor config to the EEPROM 
  EEPROM.writeBytes(0, &StartupConfiguration, sizeof(BrewControllerConfiguration));
  EEPROM.commit();
}

bool addressMatch(uint8_t * address, uint8_t *_address){
  for(int i =0; i < 8; i ++){
    if(*(address+i) != *(_address+i)){
      return false;
    }
  }

  return true;
}

bool addressNull(uint8_t *address){
  bool isNull = true;
  for(int i=0; i < 8; i++){
    if(*(address+i) != 0){
      isNull = false;
    }
  }

  return isNull;
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

void enableHeater(boolean enable){
  if(enable != heating){
    Serial.print("Setting heater to: ");
    Serial.println(enable);
    if(enable){
      digitalWrite(HEATING_POS, HIGH);
      digitalWrite(HEATING_NEG, LOW);
      heating = true;
    } else {
      digitalWrite(HEATING_POS, LOW);
      digitalWrite(HEATING_NEG, HIGH);
      heating = false;
    }
  }
}

void enableRecirc(boolean enable){
  if(enable != recirculating)
  {
    Serial.print("Setting recirc to: ");
    Serial.println(enable);
    if(enable){
      digitalWrite(RECIRC_POS, HIGH);
      digitalWrite(RECIRC_NEG, LOW);
      recirculating = true;
    } else {
      digitalWrite(RECIRC_POS, LOW);
      digitalWrite(RECIRC_NEG, HIGH);
      recirculating = false;
    }
  }
}


void printConfiguration(BrewControllerConfiguration config){
  Serial.println("Current Configuration: ");
  Serial.print("WiFi SSID: ");
  Serial.println(config.ssid);
  Serial.print("WiFi Password: ");
  Serial.println(config.password);
  Serial.print("Mash TempSensor ID: ");
  printAddress(config.mashSensor);
  Serial.println();
  Serial.print("Heater TempSensor ID: ");
  printAddress(config.heaterSensor);
  Serial.println();
}