ESP32 Relay Light Server and IR Client
------------------------------------------

Another Arduino project to automate the _mundane_ task of flipping light switches
around the house. This project uses an ESP32 to host a web server that
controls a [quad relay](https://www.sparkfun.com/products/15102) from SparkFun Electronics. 
The server utilizes [ESPAsync](https://github.com/me-no-dev/ESPAsyncWebServer) and [SPIFFS](https://github.com/pellepl/spiffs)
to host the web pages files directly on the ESP32. 
In addition there is a seperate ESP32 client attached to two IR trip wires that
communicate to the server when a person is entering or leaving a room. It
manages the number of people in the room, only to signal when there are
**no** people left. There is of course a light switch for those that will
remain down stairs and more features to be built as the project grows wings. 

To Do
-------------------------------------
- [ ] Add websockets for dynamic updating
- [ ] Update the feel of the webpage
- [ ] Add function to search for WiFi if saved WiFi settings don't work on
bootup
- [ ] Add JSON files for data storage

