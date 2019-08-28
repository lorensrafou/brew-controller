# brew-controller
ESP32 Based Homebrew Controller

See mawelsh.com for details.

Based around a single heating element eBIAB system with recirculation and two temperature sensors.  One sensor will be located below the bag near the heat element (to ensure the water temp below the grain doesn't overheat whil recirc is running) and the other will be in the mash.  The projuct supports a basic command line interface for configuration (assigning sensors and WiFi network) and a basic rest interface for viewing current brew status, setting the desired mash temp and controlling mashing/boiling state.  This is just my first commit... more to come. 
