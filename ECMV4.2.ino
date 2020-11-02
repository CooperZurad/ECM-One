// Adafruit INA260 Library - Version: Latest 
#include <Adafruit_INA260.h>

class Actuator
{
	//class member variables
	int motorPin;
	long OnTime;
	long OffTime;
	
	// maintain current state 
	int motorState; //to Enable
	unsigned long previousMillis; //last time updated
	int AutoDownSteps =0;
	
	// constructor
  Adafruit_INA260 ina260Auto = Adafruit_INA260();
	
	// constructor- creates an Act
	public:
	Actuator(int pin, long on, long off)
	{
		motorPin = pin;
		pinMode(motorPin, OUTPUT);
		OnTime = on;
		OffTime = off;
		motorState = 0;
		previousMillis = 0;
	}
	
	void Update(unsigned long currentMillis)
	{
		if (currentMillis -previousMillis >= OffTime)
		{
			previousMillis = currentMillis;
			if (motorState == HIGH)
			{
				digitalWrite(motorPin, HIGH);
				delayMicroseconds(OnTime);
				digitalWrite(motorPin, LOW);
        Serial.println("AUTO STEP  ");
				AutoDownSteps = AutoDownSteps +1;
        //Serial.print("AutoDownSteps= ");
        Serial.println(ReportAutoDown());
        //int AutoCurrent = ina260Auto.readCurrent();
        //Serial.print("AutoCurrent ");
        //Serial.println(AutoCurrent);
			}
		}	
	}
	
	
	
	/*
	void AdjustOn(long amount)
	{
		OnTime += amount;
	}
	
	*/
	
	void AdjustOff(long amount)
	{
		OffTime = amount;
	}
	
	void AdjustShort()	
	{
		OffTime = 250;
	}	
	
	void SetMotorState(int state)
	{
		motorState = state;
	}
	
	int ReportAutoDown()
	{
		return AutoDownSteps;
	}

  void ResetAutoDown()
  {
    AutoDownSteps = 0;
    Serial.println("AutoDownSteps reset to 0");
  }
	
		
};


 // Constructors

Actuator act_one(7, 10, 100);


Adafruit_INA260 ina260 = Adafruit_INA260();

 
// global variables



int const Size = 10;
int CurrentValue[Size];
int TotalCurrentValue = 0;
int AverageCurrentValue = 0;
int AverageCurrent =0;
int CurrentLimit = 9000;
int ProbeCurrent = 0;

int UpSteps =0;


// Output Pins   

int Electrode = 5; //PWM capable pin, to gate of MOSFET for Electrode
int Enable = 6;  // to stepper motor Driver
int Step = 7; // to stepper motor Driver 
int Dir = 8; // to stepper motor Driver
int Buzzer = 12; //Power through Transistor 2n2222 for buzzer

// Switch or Button Inputs

int dirPin= 2;
int movePin = 3; 
int automaticPin = 4;

// Analog Inputs 

int analogPin0 = A0; // for pot to set stepper descent rate



// States
int movebuttonState = 0;
int dirbuttonState = 0;
int pumpbuttonState = 0;
int automaticbuttonState = 0;
int probeState = 0;

// Other Values
int descentspeedValue = 0; //for stepper in Manuel Mode
int Descentspeed = 15;
int ManualSteps = 0;

int currentValue = 0;// when using analog Current Sensor 
int Current = 0;
int BusVoltage = 0; 
int Power = 0; 
long AutoDescentDelay = 5000;
int ElectrodeDutyCycle = 255;
int ElectrodeProbeDutyCycle = 10;


void setup() { 



  //For Interrupt Routine
	OCR0A = 0xAF;
	TIMSK0 |= _BV(OCIE0A);
  
  Serial.begin(115200);

  Serial.println("Adafruit INA260 Test");

  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip but will proceed as if did.");
    delay(1000);
  }
  Serial.println("Found INA260 chip");
  
  
  // initialize the digital pins
  
  pinMode (Buzzer, OUTPUT);
  pinMode (Electrode, OUTPUT);
  pinMode (Step, OUTPUT);
  pinMode (Dir, OUTPUT);
  pinMode (Enable, OUTPUT);
  pinMode (movePin, INPUT_PULLUP);
  pinMode (dirPin, INPUT_PULLUP);
  pinMode (automaticPin, INPUT_PULLUP);
    
  // initial conditions of the machine
  
  digitalWrite(Dir, LOW); // going up
  enable();  //enable stepper
  electon(); //turn Electrode on, but there is also an external switch

  // instructions
  Serial.println("type '0' to see list of commands"); 
}

SIGNAL(TIMER0_COMPA_vect) {
	unsigned long currentMillis = millis();
	act_one.Update(currentMillis);
	 
}

// the loop routine runs over and over again forever:
void loop() {

if (Serial.available()) {

    String command = Serial.readString();
    Serial.print("mini_michael sent ");
    Serial.println(command);
    int DD = command.toInt();
    if (DD == 0) {menu();}
    if (DD == 1) {electon();}
    if (DD == 2) {electoff();}
    if (DD == 3) {enable();}
    if (DD == 4) {disable();}
    if (DD == 5) {buzz_on();}
    if (DD == 6) {buzz_off();}
    if (DD == 7) {ver();}
    if (DD == 8) {report();}
    if (DD == 9) {restart();}
    if (DD == 10) {probeState = 1;} 
    if (DD == 11) {probeState = 0; Serial.println("Probe stopped");} 
    if (DD == 12) {resetautodownsteps();}
    
   
    //Serial.println(DD);
    //Serial.print("converted to long is ");
    //Serial.println(long(DD));
    
    if (long(DD) > 100) {AutoDescentDelay = long(DD);
            Serial.print("AutoDescentDelay is now ");
            Serial.println(AutoDescentDelay);}
            
  }                    											


	
	
	
	automaticbuttonState = digitalRead(automaticPin);
	if (automaticbuttonState == HIGH) {
	
		act_one.SetMotorState(0); //turn off interrupt control of motor
		
		dirbuttonState = digitalRead(dirPin);
		if (dirbuttonState == LOW) {
		//Serial.println("DOWN");
		down(); //DOWN
		}
	
		else {
		up(); //UP
    //Serial.println("UP");
		}
		
		movebuttonState = digitalRead(movePin);
		if (movebuttonState == LOW){
	
		Serial.println("STEP");
		digitalWrite(Step, HIGH);
		delayMicroseconds(10);
		digitalWrite(Step, LOW);
		delay(Descentspeed);
    //Serial.println(Descentspeed);
    ManualSteps = ManualSteps +1;
    Serial.print("Manual Steps ");
    Serial.println(ManualSteps);
    
		}
	
		else {
	
		//Serial.println("StepperOFF");
		delay(1);
		}
	}
	
	else {
	  
	  //Serial.println("Automatic");

    up();
    enable();
		act_one.SetMotorState(1); // turn on interrupt control of motor
		act_one.AdjustOff(AutoDescentDelay);
		
		
		if(Current > CurrentLimit){
			act_one.SetMotorState(0); // turn off interrupt
					
			
			Serial.println("BUZZ! ");
			
      			electoff();
      			disable();
      			buzz_on();
            delay(5000);
            buzz_off();
      			report();
			
	
		}
		
		/*
		else {
		digitalWrite(Dir, LOW);
		act_one.SetMotorState(1); // turn interrupt back on
		}
		*/	
	} 
	
	
	
	
	
	descentspeedValue = analogRead(analogPin0);
	Descentspeed = map(descentspeedValue, 1, 1023, 5, 1000);
  	
	Current = ina260.readCurrent();
  //Serial.println("Current");
  //Serial.println(Current); 
  //BusVoltage = ina260.readBusVoltage(); 
  //Power = ina260.readPower();
  
  if (probeState == 1) {probe();}
    
	
		 
	
}



void up() {
		
	digitalWrite(Dir, LOW);
	   
	   
}  


void down() {
		
	digitalWrite(Dir, HIGH);
	   	   
}  


void enable() {
	digitalWrite(Enable, LOW);
		
}	

void disable() {
	digitalWrite(Enable, HIGH);
}

void electon() {
	//digitalWrite(Electrode, HIGH);
	Serial.println("Electrode On");
	analogWrite(Electrode,ElectrodeDutyCycle);	
}

void electoff() {
	//digitalWrite(Electrode, LOW);
	Serial.println("Electrode Off");
	analogWrite(Electrode,0);
	
}

void increase() {
  AutoDescentDelay = AutoDescentDelay + 1000;
  Serial.print("AutoDescentDelay= ");
  Serial.println(AutoDescentDelay);
}

void decrease() {
  AutoDescentDelay = AutoDescentDelay -1000;
  Serial.print("AutoDescentDelay= ");
  Serial.println(AutoDescentDelay);
  
}

void buzz_on() {
  Serial.println("BUZZ!");
  digitalWrite(Buzzer, HIGH);
  
}


void buzz_off() {

  Serial.println("BUZZ OFF!");
  digitalWrite(Buzzer, LOW);
  
}

void restart() {
  //buzz_off();
  electon();
  enable();
}

void ver() {
  Serial.println("ECMV4.2 10/22/20");
}

void report() {
	//Serial.print("UpSteps= ");
	//Serial.println(UpSteps);
	//Serial.print("AutoDownSteps= ");
	//Serial.println(act_one.ReportAutoDown());
	Serial.print("AutoDescentDelay =");
  Serial.println(AutoDescentDelay);
  Serial.print("ElectrodeDutyCycle =");
  Serial.println(ElectrodeDutyCycle);
  Serial.print("Instant Current = ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.print("   ");
  Serial.print(ina260.readCurrent(),0);
  Serial.println("   ");
  //int Distance = (act_one.AutoReportDown() - UpSteps);
  //Serial.print("Distance =");
  //Serial.print(Distance*0.01);
  //Serial.println(" mm");  

	
}	

void probe() {
      
      Serial.println("PROBE");
      Serial.println("TURN ELECTRODE ON!");
      analogWrite(Electrode,ElectrodeProbeDutyCycle);  
      digitalWrite(Dir, LOW);// stepper going down
      digitalWrite(Step, HIGH);
      delayMicroseconds(10);
      digitalWrite(Step, LOW);
      delay(1000);
      Current = ina260.readCurrent();
      if (Current > 5) {
            probeState = 0;
            electoff();
            Serial.println("Probe Current Limit");
            Serial.println(Current);
            Serial.println("Probe stopped automatically");
            digitalWrite(Dir, HIGH);//stepper to go up
            for (int i = 0; i < 3; i++){ 
                  digitalWrite(Step, HIGH);
                  delayMicroseconds(10);
                  digitalWrite(Step, LOW);
                  delay(500);
                  }
            }
       Serial.println("Remember to call 'electon' before beginning to cut");
                 
}

/*
void no_probe() {
    probeState = 0;
    Serial.println("Probing stopped");
}    
*/


void menu() {
  Serial.println("'0' -shows menu");
  Serial.println("'1'- turns Electrode on");
  Serial.println("'2' - turns Electrode off");
  Serial.println("'3' - enables stepper motor");
  Serial.println("'4' - disables stepper motor");
  Serial.println("'5' - turns buzzer on");
  Serial.println("'6' - turns buzzer off");
  Serial.println("'7' - shows version of code");
  Serial.println("'8' - shows values for Steps, AutoDescentDelay, Current");
  Serial.println("'9' - issues restart commands to turn on Electrode, enable stepper, turn buzzer off");
  Serial.println("'10' - start probe");
  Serial.println("'11' - stop probe");
  Serial.println("'12' - reset AutoDownSteps"); 
  Serial.println();
  Serial.println("  Enter any number greater than 100 to change AutoDescentDelay to that number.");
}

  void resetautodownsteps() {
    act_one.ResetAutoDown();
  }
