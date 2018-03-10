mqtt-mh-z19.ino
====
test program for esp-wroom-02 & mh-z19.


Requirements
----

  - ESPr Developer（ESP-WROOM-02 development board)
      - https://www.switch-science.com/catalog/2500/
  - MH-Z19
      - http://www.winsen-sensor.com/products/ndir-co2-sensor/mh-z19.html
  - Arduino Client for MQTT
      - https://github.com/knolleary/pubsubclient/
  - plerup/espsoftwareserial
      - https://github.com/plerup/espsoftwareserial

How to
----

    $ git clone https://github.com/yoggy/mqtt-mh-z19.git
    $ mqtt-mh-z19
    $ cp config.ino.sample config.ino
    $ vi config.ino
    ※ edit mqtt_host, mqtt_username, mqtt_password, mqtt_publish_topic...
    $ open mqtt-mh-z19.ino

Copyright and license
----
Copyright (c) 2018 yoggy

Released under the [MIT license](LICENSE.txt)