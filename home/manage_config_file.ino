void load_wifi_settings(){

  // SPIFFS .open() supports all Stream methods
  //https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html
  File config_file = SPIFFS.open("/home.json", "r");

  // Now we know know what to allocate to the JsonDocument constructor
  size_t size = config_file.size(); 
  std::unique_ptr<char[]> settings_buffer(new char[size]); 
  config_file.readBytes(settings_buffer.get(), size); 

  // Allocating size according to the size of the file above.
  StaticJsonDocument<size> doc; 

  //DeserializationError error = deserializeJson(doc, file);
  JsonObject& json = doc.parseObject(settings_buffer.get());

  if(error)
    Serial.println("File does not exist.")

  strlcpy(relay_creds.host, doc["wifi_credentials"]["SSID"], 
          sizeof(relay_creds.host));
  strlcpy(relay_creds.host, doc["wifi_credentials"]["password"],
          sizeof(relay_creds.password));
  
  config_file.close();
}

