// Rispro LPDP IoT Dengue 2nd year
// Mosquito detection node by HAM
// Developed based on blog post:
// https://eloquentarduino.github.io/2020/05/easier-faster-pure-video-esp32-cam-motion-detection/?utm_source=old_version
// modified and adjusted to Mosquito detection purpose

// add Wifi for MQTT
// #include <Arduino.h>
//#include <WiFi.h>
//#include <PubSubClient.h>

// adapted with the node SSID and Password
//const char* ssid = "*********";
//const char* password = "*********";

// MQTT client
// change this based on the node needs
//WiFiClient wifiClient;
//PubSubClient mqttClient(wifiClient); 
//char *mqttServer = "101.128.71.227";
//int mqttPort = 1883;

// using AI Thinker camera model
#define CAMERA_MODEL_AI_THINKER

// Library definition
#include <FS.h>
#include <SPIFFS.h>
#include "EloquentVision.h" //download this from the github (on the post link)

// define the frame parameter
#define FRAME_SIZE FRAMESIZE_QVGA//QVGA
#define SOURCE_WIDTH 320
#define SOURCE_HEIGHT 240
#define CHANNELS 1
#define DEST_WIDTH 32 //32
#define DEST_HEIGHT 24 //24
// important tuning parameter (define sensitivity)
#define BLOCK_VARIATION_THRESHOLD 0.05//0.3
#define MOTION_THRESHOLD 0.02//0.2

// namespace definition
using namespace Eloquent::Vision;
using namespace Eloquent::Vision::IO;
using namespace Eloquent::Vision::ImageProcessing;
using namespace Eloquent::Vision::ImageProcessing::Downscale;
using namespace Eloquent::Vision::ImageProcessing::DownscaleStrategies;

// an easy interface to capture images from the camera
ESP32Camera camera;
// the buffer to store the downscaled version of the image
uint8_t resized[DEST_HEIGHT][DEST_WIDTH];
// the downscaler algorithm
// for more details see https://eloquentarduino.github.io/2020/05/easier-faster-pure-video-esp32-cam-motion-detection
Cross<SOURCE_WIDTH, SOURCE_HEIGHT, DEST_WIDTH, DEST_HEIGHT> crossStrategy;
// the downscaler container
Downscaler<SOURCE_WIDTH, SOURCE_HEIGHT, CHANNELS, DEST_WIDTH, DEST_HEIGHT> downscaler(&crossStrategy);
// the motion detection algorithm
MotionDetection<DEST_WIDTH, DEST_HEIGHT> motion;
JpegWriter<SOURCE_WIDTH, SOURCE_HEIGHT> jpegWriter;


bool debounceMotion(bool touch = false);
void printFilesize(const char *filename);


/**

*/
void setup() {  
  Serial.begin(115200);
  SPIFFS.begin(true);
  camera.begin(FRAME_SIZE, PIXFORMAT_GRAYSCALE);
  motion.setBlockVariationThreshold(BLOCK_VARIATION_THRESHOLD);
  // additional WSP Wifi connect
  //  WiFi.begin(ssid, password);
  //
  //  while (WiFi.status() != WL_CONNECTED) {
  //    delay(500);
  //    Serial.print(".");
  //  }
  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //
  //  mqttClient.setServer(mqttServer, mqttPort);
  
  // additional  (HAM)
  // add trigger pin (to jack stereo -> android)
  // using pin IO12, because we dont use the SD card function
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  //wait 30s, this one is used to get the node stable
  Serial.println("Initializing!");
  delay(30000);
  Serial.println("Ready!");
}

/**

*/
void loop() {
  digitalWrite(12, LOW);
  uint32_t start = millis();
  camera_fb_t *frame = camera.capture();

  // resize image and detect motion
  downscaler.downscale(frame->buf, resized);
  motion.update(resized);
  motion.detect();

  // compute FPS
  Serial.print(1000.0f / (millis() - start));
  Serial.println(" fps");

  // on motion detected, save image to filesystem
  if (motion.ratio() > MOTION_THRESHOLD) {
    Serial.println("Motion detected");

    // save image
    if (debounceMotion()) {
      // take a new pic in the hope it is less affected by the motion noise
      // (you may comment this out if you want)
      delay(500);
      frame = camera.capture();

      // write as jpeg
      File imageFile = SPIFFS.open("/capture.jpg", "wb");
      // you can tweak this value as per your needs
      uint8_t quality = 50; //30

      Serial.println("The image will be saved as /capture.jpg");
      jpegWriter.writeGrayscale(imageFile, frame->buf, quality);
      imageFile.close();
      printFilesize("/capture.jpg");

      debounceMotion(true);
    }
    // give trigger to the jack DC
    //  int test = 1;
    //  char data[40];
    //  sprintf(data, "%f", test);
    //  mqttClient.publish("/detect10", data); 
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    // wait for 5 minutes to anticipate bounce/multiple detection
    delay(3000000);
  }
}

/**
   Debounce repeated motion detections
   @return
*/
bool debounceMotion(bool touch) {
  static uint32_t lastMotion = 0;

  // update last tick
  if (lastMotion == 0 || touch)
    lastMotion = millis();

  // debounce
  if (millis() - lastMotion > 5000) {
    lastMotion = millis();

    return true;
  }

  return false;
}


/**
   Print file size (for debug)
   @param filename
*/
void printFilesize(const char *filename) {
  File file = SPIFFS.open(filename, "r");

  Serial.print(filename);
  Serial.print(" size is ");
  Serial.print(file.size() / 1000);
  Serial.println(" kb");

  file.close();
}
