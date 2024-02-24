
#include "Particle.h"
#include "Logging.h"


SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);


unsigned long last_time;


void setup() {
  
  logging_remote(LOG_ON, LOG_ON_DEBUG);

  last_time = millis();
}


void loop() {

  unsigned long time = millis();

  if(time > last_time + 10000) {

    Log.info("%d", (int) Time.now());

    if(time > last_time + 30000) {
      Log.warn("Too long!");
    }
    
    last_time = time;
  }
}