#include <Ticker.h>

// define pins
#define sigB 13    // pin 13 to output watchdog
#define LED 14     // pin 14 to indicate error
#define button1 33 // pin 33 to check button press
#define aIN 12     // pin 12 to read analogue signal
#define pIN 25     // pin 25 to ready digital signal

// define rates of task in ms
// calculated period from frequency
#define t1 4      // SignalB repeats every 1300+2500+8 = 3808 microseconds ~4 ms
#define t2 200    // 5Hz
#define t3 1000   // 1Hz
#define t4 42     // 24Hz
#define t5 42     // 24Hz
#define t6 100    // 10Hz
#define t7 333    // 3Hz
#define t8 333    // 3Hz
#define t9 5000   // 0.3Hz

// define variables
bool b1 = false;
float freqIN = 0;
float analogueIN = 0;
float analogues[4];
float average_analogueIN;
float average_analogues[4];
int error_code;
int tick;       // tick counter

Ticker ticker;  // initialise ticker for cyclic executive

// define pin functionalities
// arrays
// inital tick value
// baud rate
// enable ticker to run every ms
void setup() {
  Serial.begin(57600);
  tick = 0;
  analogues[0] = 0;
  analogues[1] = 0;
  analogues[2] = 0;
  analogues[3] = 0;
  pinMode(sigB,OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(pIN, INPUT);
  pinMode(aIN, INPUT);
  
  ticker.attach_ms(1, count);
  Serial.print("\nSwitch \tInput \tFrequency \n");
}

// executes tasks based off of the frequency that they need executing
// ticker increments tick every ms, if statement executes task when
// the rate is divisible and has a set remainder of 0 (or 21, 167
// so they operate at the correct frequency)
void count()
{
  tick++;
  if((tick%t1) == 0) task1();
  if((tick%t2) == 0) task2();
  if((tick%t3) == 0) task3();
  if((tick%t4) == 0) task4();
  if((tick%t5) == 21) task5();
  if((tick%t6) == 0) task6();
  if((tick%t7) == 0) task7();
  if((tick%t8) == 167) task8();
  if((tick%t9) == 0) task9();
}

// 50us pulse
void task1()  // SignalB repeats every 1300+2500+8 = 3808 microseconds ~4 ms
{
  digitalWrite(sigB, HIGH);
  delayMicroseconds(50);
  digitalWrite(sigB, LOW);
}

// Read digital value from PIN button1
void task2()
{
  b1 = digitalRead(button1);
}

// measure square wave input
// calculate frequency from pulse length (period)
void task3()
{
  float peak;
  peak = pulseIn(pIN, LOW);
  freqIN = 1000000 / peak*2;
}

// store analogue values into arrays
// use for loop to cycle through
void task4()
{
  for(int i=1; i<4; i++)
  {
    analogues[i-1] = analogues[i];
  }
  analogues[3] = analogRead(aIN);
}

// calculates average analogue readings from previous tasks
void task5()
{
  average_analogueIN = 0;
  // loop to find some of values
  for(int i=0; i<4; i++)
  {
    average_analogueIN += analogues[i];
  }
  average_analogueIN = average_analogueIN /4; // divide by total of readings to obtain average reading
}

// this line of code waits for one clock cycle, 1000 times
// ESP32 has an internal 8MHz oscillator, thus, 1000 clock cycles = 1000/8000000 = 0.000125s
void task6()
{
  for(int i=0; i<1000; i++)
  {
    __asm__ __volatile__ ("nop");
  }
}

// define a value that when crossed enables error value
void task7()
{
  if((average_analogueIN * 3.3 / 4095) > (3.3/2))
  {
    error_code = 1;
  } else {
    error_code = 0;
  }
}

// visualize error code through an LED
void task8()
{
  digitalWrite(LED, error_code);
}

// print values to serial monitor
void task9()
{
  Serial.print(b1);
  Serial.print(", \t");
  Serial.print(freqIN);
  Serial.print(", \t");
  Serial.print((average_analogueIN * 3.3 / 4095));
  Serial.print("\n");
}

void loop() {}
