// Rispro LPDP IoT Dengue 2nd year
// Mosquito detection node by HAM
// Developed based on blog post:
// https://eloquentarduino.github.io/2020/05/easier-faster-pure-video-esp32-cam-motion-detection/?utm_source=old_version
// modified and adjusted to Mosquito detection purpose
// Major update : 290322 --> update code to disable detection when detected something (activate debounce flag)
//                       --> update code to ignore every detection in first 10s, to avoid any false detection

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
#define MOTION_THRESHOLD 0.03//0.2
// hold time when node detect something
#define debounce_time 30000
float motion_thresh; //implemented variable
static uint32_t starting_, anticipate_;
bool debounce_flag;

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
  motion_thresh = MOTION_THRESHOLD;
  // additional  (HAM)
  // add trigger pin (to jack stereo -> android)
  // using pin IO12, because we dont use the SD card function
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  //wait 30s, this one is used to get the node stable
  Serial.println("Initializing!");
  delay(3000);
  Serial.println("Ready!");
  //  record the starting time and debounce anticipation flag
  starting_ = millis();
  anticipate_ = 0;
  debounce_flag = false;
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

  //  add function to compensate multiple detection by modifying motion threshold value
  if (debounce_flag)
  {
    motion_thresh = 100;
    Serial.println("debounce active!");
    if (millis() - anticipate_ > debounce_time) //stop debounce routine
    {
      debounce_flag = false;
      motion_thresh = MOTION_THRESHOLD;
    }
  }


  // on motion detected, save image to filesystem
  if (motion.ratio() > motion_thresh & millis() - starting_ > 10000 )  //add requirement to ignore first 10 seconds reading
  { 
    Serial.println("Motion detected");
    // give trigger to the jack DC
    digitalWrite(12, HIGH);
    delay(100);
    digitalWrite(12, LOW);
    // enable the debounce flag and record the time
    debounce_flag = true;
    anticipate_ = millis();
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