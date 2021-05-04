# Throttle-sketches #
Arduino sketches to create a model train multi-channel throttle with an Arduino Uno R3 and an Adafruit Motor Shield v2.3.

## ThrottleTest ##
This sketch is a quick test of the Motor Shield as a throttle for a KATO N-gauge PORTRAM model, based on the trams of the Toyama Light Rail system in Toyama, Japan.  The sketch is basically the DCMotorTest example sketch from the Motor Shield's library, but with serial output disabled and timings altered.  It ramps up the output voltage from 0 to full-on (12 volts) over about 5 seconds, holds full power for 3 seconds, ramps back down to 0, pauses 3 more seconds, then repeats the process with the opposite polarity.  So the tram accelerates from stop to full speed, cruises, decelerates to stop, pauses, then reverses direction.  This sequence repeats indefinitely.

The KATO PORTRAM models run at a fairly accurate scale speed of 60 km/hr on a full 12 volt supply.  A more realistic acceleration/deceleration time however is closer to 15 seconds.  I could fine-tune this sketch, but this version was used to make a YouTube video of the test run, so the sketch will remain as is.
