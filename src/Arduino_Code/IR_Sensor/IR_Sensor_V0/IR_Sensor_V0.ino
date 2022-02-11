// IoT Dengue 2
// Maintained by HAM
// Mosquito Classification Supporting Device
// Mosquito Recognition: To know whether the mosquito is entering the IoT Chamber or not
//                        This code aims to improve the previous recording method (every 3 hours)
//                        The code proposed a new method by detecting a mosquito that enters the chamber
//                        by using 4-pairs of IR-LED - Photodiode
// Code V0 : IR LED -> Chamber -> Photodiode -> ADS1115 -> Arduino Nano (ADC + Moving Average)

// Connection :
/*
  ADS1115 - Arduino Nano
  VDD - 5V
  GND - GND
  SCL - A5
  SDA - A4
*/

#include <Adafruit_ADS1X15.h>
#include <FIR.h>
//#include <MovingAverage.h>
//#include "MegunoLink.h"
//#include "Filter.h"
Adafruit_ADS1115 ads; //16-bit version

// set filter FIR
FIR<float, 16> fir;
FIR<float, 16> fir2;
FIR<float, 16> fir3;
FIR<float, 16> fir4;
//

int16_t adc0, adc1, adc2, adc3;
float filterVal = 0.8;
int smooth1, smooth2, smooth3, smooth4;
int avgval;


// Create a new exponential filter with a weight of 10 and initial value of 0.
//ExponentialFilter<long> ADC1(100, 0);
//ExponentialFilter<long> ADC2(100, 0);
//ExponentialFilter<long> ADC3(100, 0);
//ExponentialFilter<long> ADC4(100, 0);

//MovingAverage<unsigned> test1(10, 2);
//MovingAverage<unsigned> test2(10, 2);
//MovingAverage<unsigned> test3(10, 2);
//MovingAverage<unsigned> test4(10, 2);

//unsigned delta_x = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  if (!ads.begin()) {
    Serial.println("Failed to initialize ADS.");
    while (1);
  }

  float coef[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir.setFilterCoeffs(coef);
  float coef2[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir2.setFilterCoeffs(coef2);
  float coef3[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir3.setFilterCoeffs(coef3);
  float coef4[16] = { 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1.};
  fir4.setFilterCoeffs(coef4);
  //  Serial.print("Gain set: ");
  //  Serial.println(fir.getGain());
  //
  //  Serial.println("START!");
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  // 1. use the ADC data only!
  //  Serial.print("Sensor_1:"); Serial.print(analogRead(3));
  //  Serial.print("\t");
  //  Serial.print("Sensor_2:"); Serial.print(analogRead(2));
  //  Serial.print("\t");
  //  Serial.print("Sensor_3:"); Serial.print(analogRead(1));
  //  Serial.print("\t");
  //  Serial.print("Sensor_4:"); Serial.println(analogRead(0));
  // 2. Using ADS1115 16 bit ADC
  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  //    Serial.print("Sensor_1:"); Serial.print(adc3);
  //    Serial.print("\t");
  //    Serial.print("Sensor_2:"); Serial.print(adc2);
  //    Serial.print("\t");
  //    Serial.print("Sensor_3:"); Serial.print(adc1);
  //    Serial.print("\t");
  //    Serial.print("Sensor_4:"); Serial.println(adc0);
  // 3. Using Exponential Filter
  //  int RawValue = analogRead(0);
  //    ADCFilter.Filter(RawValue);
  //  adc0 = ads.readADC_SingleEnded(0);
  //  adc1 = ads.readADC_SingleEnded(1);
  //  adc2 = ads.readADC_SingleEnded(2);
  //  adc3 = ads.readADC_SingleEnded(3);
  //
  //  ADC1.Filter(adc0);
  //  ADC2.Filter(adc1);
  //  ADC3.Filter(adc2);
  //  ADC4.Filter(adc3);
  //
  //  Serial.print("Sensor_1:"); Serial.print(ADC1.Current());
  //  Serial.print("\t");
  //  Serial.print("Sensor_2:"); Serial.print(ADC2.Current());
  //  Serial.print("\t");
  //  Serial.print("Sensor_3:"); Serial.print(ADC3.Current());
  //  Serial.print("\t");
  //  Serial.print("Sensor_4:"); Serial.println(ADC4.Current());
  //  4. Using Moving Average Filter
  //  adc0 = ads.readADC_SingleEnded(0);
  //  adc1 = ads.readADC_SingleEnded(1);
  //  adc2 = ads.readADC_SingleEnded(2);
  //  adc3 = ads.readADC_SingleEnded(3);
  //
  //  test1.push(adc0);
  //  test2.push(adc1);
  //  test3.push(adc2);
  //  test4.push(adc3);
  //
  //  Serial.print("Sensor_1:"); Serial.print(test1.get());
  //  Serial.print("\t");
  //  Serial.print("Sensor_2:"); Serial.print(test2.get());
  //  Serial.print("\t");
  //  Serial.print("Sensor_3:"); Serial.print(test3.get());
  //  Serial.print("\t");
  //  Serial.print("Sensor_4:"); Serial.println(test4.get());
  //  5. Using low pass filter
  //  adc0 = ads.readADC_SingleEnded(0);
  //  adc1 = ads.readADC_SingleEnded(1);
  //  adc2 = ads.readADC_SingleEnded(2);
  //  adc3 = ads.readADC_SingleEnded(3);
  //
  //  smooth1 =  smooth(adc0, filterVal, smooth1);
  //  smooth2 =  smooth(adc1, filterVal, smooth2);
  //  smooth3 =  smooth(adc2, filterVal, smooth3);
  //  smooth4 =  smooth(adc3, filterVal, smooth4);
  //
  //  Serial.print("Sensor_1:"); Serial.print(smooth1);
  //  Serial.print("\t");
  //  Serial.print("Sensor_2:"); Serial.print(smooth2);
  //  Serial.print("\t");
  //  Serial.print("Sensor_3:"); Serial.print(smooth3);
  //  Serial.print("\t");
  //  Serial.print("Sensor_4:"); Serial.println(smooth4);
  //  delay(200);
  //  6. Using FIR Moving average
  Serial.print("Sensor_1: ");
  Serial.println(fir.processReading(adc0));
  //  Serial.print("\t");
  //  Serial.print("Sensor_2: ");
  //  Serial.print(fir2.processReading(adc1));
  //  Serial.print("\t");
  //  Serial.print("Sensor_3: ");
  //  Serial.print(fir3.processReading(adc2));
  //  Serial.print("\t");
  //  Serial.print("Sensor_4: ");
  //  Serial.println(fir4.processReading(adc3));
  //  avgval = (fir.processReading(adc0) + fir2.processReading(adc1) + fir3.processReading(adc2) + fir4.processReading(adc3)) / 4;
  //  avgval = (adc0 + adc1 + adc2 + adc3)/4;
  //  Serial.println(fir.processReading(avgval));
  //  Serial.println(avgval);
  //  Serial.print("\t");
}

int smooth(int data, float filterVal, float smoothedVal) {


  if (filterVal > 1) {     // check to make sure param's are within range
    filterVal = .99;
  }
  else if (filterVal <= 0) {
    filterVal = 0;
  }

  smoothedVal = (data * (1 - filterVal)) + (smoothedVal  *  filterVal);

  return (int)smoothedVal;
}
