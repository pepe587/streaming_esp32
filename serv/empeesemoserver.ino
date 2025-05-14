#include "WiFi.h"
#include <TFT_eSPI.h>
#include <Button.h>
#include <Bonezegei_XPT2046v2.h>
#include <Bonezegei_Utility.h>
#include <JPEGDecoder.h>
#include <stdio.h>
#include <unistd.h>
#define TS_CS 21

WiFiUDP               udp;
TFT_eSPI              tft;
TFT_eSPI_Button       button;
Bonezegei_XPT2046v2   touch(TS_CS);
Bonezegei_Input       *input = &touch;


void showJPEG(TFT_eSPI *tft, uint8_t *jpegData, size_t jpegSize) {
   JPEGDecoder JpegDec;
   
   // Decodificar el archivo JPEG
    JpegDec.decodeArray(jpegData, jpegSize);

    // Crear un buffer para la imagen decodificada
    uint16_t *pImage = (uint16_t *)malloc(JpegDec.width * JpegDec.height * sizeof(uint16_t));
    if (pImage == NULL) {
        Serial.println("No se pudo asignar memoria para la imagen");
        return;
    }

    // Leer la imagen decodificada en el buffer
    for (uint16_t y = 0; y < JpegDec.height; y++) {
        for (uint16_t x = 0; x < JpegDec.width; x+=3) {
            JpegDec.read();
            uint16_t color = tft->color565(JpegDec.pImage[(y * JpegDec.width * 3) + x], JpegDec.pImage[(y * JpegDec.width * 3) + x + 1], JpegDec.pImage[(y * JpegDec.width * 3)] + x + 2);
            pImage[y * JpegDec.width + x] = color;
        }
    }

    // Mostrar la imagen en la pantalla TFT
    tft->pushImage(0, 0, JpegDec.width, JpegDec.height, pImage);

    // Liberar la memoria asignada para la imagen
    free(pImage);
}


void setup()
{


  IPAddress local_IP(192, 168, 4, 1); // Dirección IP estática que deseas asignar
  IPAddress gateway(192, 168, 4, 1);    // Dirección IP del gateway
  IPAddress subnet(255, 255, 255, 0);   // Máscara de subred
  Serial.begin(115200);
  WiFi.config(local_IP, gateway, subnet);
  WiFi.softAP("hola", "queepico");
  IPAddress IP = WiFi.softAPIP();
  Serial.printf("AP IP address: ");
  Serial.println(IP);
  udp.begin(8080);
  tft.init();
  tft.begin();
  tft.fillScreen(0xffffffff);
  input->begin();
  input->setRotation(1);
  button.initButton(&tft, 100, 100, 100, 100, 0xff000008, 0x00000000, 0xffffffff, "->", 5);
  }

void loop() {
  static char *buffer = (char *)malloc(96 *96 *8);
  static char *image = (char *)malloc(1500 * sizeof(char));
  static int packetSize;
  static unsigned long total_len;
  static int len;
  static int b = 0;

  if (b == 0)
  {
    *buffer = 0;
    b++;
  }
  if (input->getPress())
  {
      button.drawButton();
  }
  packetSize = udp.parsePacket();
  if (packetSize) {
    len = udp.read(image, packetSize);
    if (len > 0) {
      image[len] = '\0'; // Null-terminate the buffer to treat it as a string
      if (len == 1460)
      {
        buffer = strcat((char *)buffer, (const char *)image);
        total_len += len;
      }
      else
      {
        buffer = strcat((char *)buffer, (const char *)image);
        total_len += len;
        showJPEG(&tft, (uint8_t *)buffer, total_len);
        buffer = (char *)memset((void *)buffer, 0, 96*96*8);
        total_len = 0;
      }
    }
  }
}