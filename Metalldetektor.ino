/* Arduino Nano # Atmega 328
 * 
 */

//constants
//------------ pins
const int genPin = 8; //gen pin
const int integratePin = 7;
const int protectPin = 6;

const int adjustLedLow = 5;
const int adjustLedHigh = 4;
const int adjustBtnPin = 3;

const int signalAPin = 0;
const int referenceAPin = 1;

const int beepPin = 11; //beep pin

const int INTEGRATE_TIME_MS = 70;
const int PROTECT_TIME_MS = 85; // time for wait before measure signal (it is for cut jump signal after inductance power off)
const int INDUCTANCE_POWER_ON = 200; 


void setup()   {
  Serial.begin(115200);
  pinMode(genPin, OUTPUT);//gen pin -> out
  pinMode(integratePin, OUTPUT);
  pinMode(protectPin, OUTPUT);

  pinMode(adjustBtnPin, INPUT);
  pinMode(adjustLedLow, OUTPUT);
  pinMode(adjustLedHigh, OUTPUT);

  pinMode(beepPin, OUTPUT);
  
  digitalWrite(adjustLedLow, LOW);
  digitalWrite(adjustLedHigh, LOW);
}


void loop() 
{
    while(true)
    {     
      //for safe
      digitalWrite(protectPin, LOW);

      //not integrate signal
      digitalWrite(integratePin, HIGH);
    
        //pulse
        digitalWrite(genPin, LOW);
        delayMicroseconds(INDUCTANCE_POWER_ON); // 200 works fine
        digitalWrite(genPin,HIGH);

        // wait (protection)
        delayMicroseconds(PROTECT_TIME_MS); //30  (100 new) 100 works fine
        digitalWrite(protectPin, HIGH);

        //integrate
        digitalWrite(integratePin, LOW);
        delayMicroseconds(INTEGRATE_TIME_MS); //60 (60 new) 70 -works fine

        //measure
        digitalWrite(integratePin, HIGH);

        // cheeck referense voltage
        int reference = analogRead(referenceAPin);

        // if settings button pressed - perform adjustment
        if(digitalRead(adjustBtnPin) == LOW)
        {
          analogWrite(beepPin, 0);
          //noTone(beepPin);
          
          long val = analogRead(signalAPin);
          val += analogRead(signalAPin);
          val += analogRead(signalAPin);
          val += analogRead(signalAPin);
          val += analogRead(signalAPin);
          val += analogRead(signalAPin);
          
          val /= 6;
        
          Serial.print("val");Serial.println(val);
          Serial.print("ref");Serial.println(reference);
        
          long highEdge = val + 3;
          long lowEdge = val - 3;
                   
          if(highEdge > reference)
          {
            digitalWrite(adjustLedHigh, HIGH);
            digitalWrite(adjustLedLow, LOW);
          } else if(lowEdge < reference) {
            digitalWrite(adjustLedLow, HIGH);
            digitalWrite(adjustLedHigh, LOW);
          } else {
            digitalWrite(adjustLedLow, LOW);
            digitalWrite(adjustLedHigh, LOW);
          }
          
        } else {
          digitalWrite(adjustLedLow, LOW);
          digitalWrite(adjustLedHigh, LOW);
          
          int count = 0;
        
          while(count < 100)
          {
            int val = analogRead(signalAPin);
            if(val > reference - 10)
            {
              break;
            }
            count++;
          }
          
        
          if(count > 3 ) {
            long vol = 240 * 100 / count;
            analogWrite(beepPin, vol);
            long freq = 100 + (700L * 100 / count);
            //tone(beepPin, freq);
            Serial.println(count);
            
          } else {
            //noTone(beepPin);
            analogWrite(beepPin, 0);
          }
        }
    
        delay(5);
    }
}

