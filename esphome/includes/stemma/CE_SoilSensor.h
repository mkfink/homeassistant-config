#include "esphome.h"
#include "Adafruit_seesaw.h"

class CE_SoilSensor : public PollingComponent, public Sensor {
 public:
  // constructor
  Adafruit_seesaw ss;

  Sensor *TemperatureSensor = new Sensor();
  Sensor *MoistureSensor = new Sensor();

  CE_SoilSensor() : PollingComponent(6000) { }

  void setup() override {
    // This will be called by App.setup()
    ss.begin(0x36);
  }

  void update() override {
    if (float temp = ss.getTemp()) TemperatureSensor->publish_state(temp);
    if (uint16_t capread = ss.touchRead(0)) {
      float moist = capread;//((capread - 337.0)/1586.0)*100.0; 1020 -> 0%
      MoistureSensor->publish_state(moist);
    }
  }
};