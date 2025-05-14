#include <WiFi.h>
#include <WiFiUdp.h>
#include "esp_camera.h"
#define CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM  32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM  0
#define SIOD_GPIO_NUM  26
#define SIOC_GPIO_NUM  27

#define Y9_GPIO_NUM    35
#define Y8_GPIO_NUM    34
#define Y7_GPIO_NUM    39
#define Y6_GPIO_NUM    36
#define Y5_GPIO_NUM    21
#define Y4_GPIO_NUM    19
#define Y3_GPIO_NUM    18
#define Y2_GPIO_NUM    5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM  23
#define PCLK_GPIO_NUM  22


IPAddress ip(192, 168, 4, 1);

// Configuración de la cámara
void initCamera() {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.xclk_freq_hz = 32000000;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_96X96; //FRAMESIZE_SVGA; // 800x600
  config.jpeg_quality = 12;
  config.fb_count = 3;
  config.fb_location = CAMERA_FB_IN_PSRAM;

  // Inicializa la cámara
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    while (1); // Detén la ejecución si la cámara falla
  }
}

// Configuración de la conexión WiFi
void initWiFi() {
  WiFi.begin("hola", "queepico");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(100);
  }
  Serial.println("Connected to WiFi");
}

void setup() {
  Serial.begin(115200);
  initCamera();
  initWiFi();
}


void loop()
{
  static camera_fb_t *fb;
  static WiFiUDP udp;
  //static unsigned long long last_sec= 0;
  //static short frames = 0;

  fb = esp_camera_fb_get();
  if (fb)
  {
    udp.beginPacket(ip, 8080);
    udp.write(fb->buf, fb->len);
    udp.endPacket();
    esp_camera_fb_return(fb);
  }
  //frames++;
 // if (fb && fb->timestamp.tv_sec > last_sec)
 // {
 //     Serial.printf("frames = %d\n", frames);
 //     last_sec = fb->timestamp.tv_sec;
 //     frames = 0;
  //}
  //if (fb)
  //  esp_camera_fb_return(fb);
}

