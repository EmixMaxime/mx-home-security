#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_camera.h"
#include <Preferences.h>
#include <ArduinoJson.h>

// Allocate a temporary JsonDocument
// Use https://arduinojson.org/v6/assistant to compute the capacity.

Preferences preferences;

WiFiClient espClient;
PubSubClient client(espClient);

const uint16_t size = 1024;

#define MQTT_BROKER_PORT 8883
#define MQTT_BROKER_IP 192, 168, 1, 14

IPAddress broker_ip(MQTT_BROKER_IP);

const char *ssid = "Livebox-FFD0";
const char *password = "wKzooRJtrJiE3X9kVP";
const char *mqtt_server = "192.168.1.14";
const char *HostName = "ESP_RFID_2";
const char *mqttUser = "mx";
const char *mqttPassword = "coucou";

#define TOPIC_PUBLISH_PICTURE "test/picture"
#define TOPIC_RECV_REGISTERED "registered/alarm"

// <!> will add /<device_id> at the end.
#define TOPIC_CAMERA_MANAGER "status/camera_manager/"


#define CAMERA_MODEL_AI_THINKER
#include "pins.h"

void initMqttCamera(String device_id);

const int send_picture_period_ms = 1000;
const int send_ping_period_ms = 60000;
unsigned long time_now_send_picture = 0;
unsigned long time_now_send_ping = 0;
String device_id = "";
bool run_camera = false;
uint16_t bufferSize = client.getBufferSize();


void setup_wifi()
{
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  // WiFi.hostname(HostName);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Mac address: ");
  Serial.println(WiFi.macAddress());
}

void reconnect() {
  bool connected = false;
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");

    if (device_id.equals("")) {
      Serial.println("connect without will message because device_id is unkown.");
      connected = client.connect(HostName, mqttUser, mqttPassword);
    } else {
      Serial.println("connect with will message to sync status.");
      char will_topic[100];
      const char* id = device_id.c_str();

      snprintf(will_topic, sizeof(will_topic), "status/camera_manager/%s", id);

      // \x00 = binary of false
      // @todo: will message does not work.
      connected = client.connect(HostName, mqttUser, mqttPassword, (const char *) will_topic, 1, true, "\x00");
      // device id is known, set will message.
    }

    if (connected) {
      Serial.println("connected");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

esp_err_t camera_init() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer   = LEDC_TIMER_0;
  config.pin_d0       = 5;
  config.pin_d1       = 18;
  config.pin_d2       = 19;
  config.pin_d3       = 21;
  config.pin_d4       = 36;
  config.pin_d5       = 39;
  config.pin_d6       = 34;
  config.pin_d7       = 35;
  config.pin_xclk     = 0;
  config.pin_pclk     = 22;
  config.pin_vsync    = 25;
  config.pin_href     = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn     = 32;
  config.pin_reset    = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size   = FRAMESIZE_VGA; // QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.jpeg_quality = 10;           
  config.fb_count     = 1;

  // config.ledc_channel = LEDC_CHANNEL_0;
  // config.ledc_timer = LEDC_TIMER_0;
  // config.pin_d0 = Y2_GPIO_NUM;
  // config.pin_d1 = Y3_GPIO_NUM;
  // config.pin_d2 = Y4_GPIO_NUM;
  // config.pin_d3 = Y5_GPIO_NUM;
  // config.pin_d4 = Y6_GPIO_NUM;
  // config.pin_d5 = Y7_GPIO_NUM;
  // config.pin_d6 = Y8_GPIO_NUM;
  // config.pin_d7 = Y9_GPIO_NUM;
  // config.pin_xclk = XCLK_GPIO_NUM;
  // config.pin_pclk = PCLK_GPIO_NUM;
  // config.pin_vsync = VSYNC_GPIO_NUM;
  // config.pin_href = HREF_GPIO_NUM;
  // config.pin_sscb_sda = SIOD_GPIO_NUM;
  // config.pin_sscb_scl = SIOC_GPIO_NUM;
  // config.pin_pwdn = PWDN_GPIO_NUM;
  // config.pin_reset = RESET_GPIO_NUM;
  // config.xclk_freq_hz = 20000000;
  // config.pixel_format = PIXFORMAT_JPEG;

  // config.fb_count = 1;
  // config.jpeg_quality = 10;
  // config.frame_size = FRAMESIZE_VGA;

  esp_err_t err = esp_camera_init(&config);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Camera Init Failed");
    return err;
  }

  return ESP_OK;
}

void mqttCallback(String topic, byte* message, unsigned int length) {
  String payload;
  for (int i = 0; i < length; i++) {
    payload += (char)message[i];
  }

  Serial.println("mqtt callback called with topic: ");
  Serial.println(topic);

  if (topic.equals(TOPIC_RECV_REGISTERED)) {
    StaticJsonDocument<2048> CONFIG;

    deserializeJson(CONFIG, payload);

    Serial.print("\n Got the device_id from bobby core");
    Serial.println(payload);
    const char* device_id = CONFIG["device_id"];
    Serial.println(device_id);

    preferences.putString("device_id", device_id);
    initMqttCamera(device_id);
  }
  else if (topic.equals(TOPIC_CAMERA_MANAGER + device_id)) {
    StaticJsonDocument<1048> PAYLOAD_JSON;
    deserializeJson(PAYLOAD_JSON, payload);
    Serial.println("handling camera status");

    const bool status = PAYLOAD_JSON["status"];
    Serial.println(payload);
    Serial.println("status: ");
    Serial.println(status);
    run_camera = status;
  }
}

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  while (!Serial); // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  camera_init();
  delay(500);
  setup_wifi();
  client.setServer(broker_ip, MQTT_BROKER_PORT);
  client.setCallback(mqttCallback);

  // RW-mode (second parameter has to be false).
  preferences.begin("bobby-cam", false);

  device_id = preferences.getString("device_id", "");

  reconnect();

  if (device_id.equals("")) {
    StaticJsonDocument<1024> payload;

    payload["type"] = "esp32cam";
    payload["id"] = WiFi.macAddress();

    char buffer[1024];
    serializeJson(payload, buffer);

    Serial.println("No device_id! Get one from the core app.");
    client.subscribe(TOPIC_RECV_REGISTERED);
    client.publish("discover/alarm", buffer, false);
  } else {
    Serial.print("\n setup known device_id: ");
    Serial.println(device_id);
    initMqttCamera(device_id);
  }
}

esp_err_t process_image(camera_fb_t *fb, uint16_t *mqttBufferSize) {
  if (fb->len > *mqttBufferSize) {
    size_t newSize = fb->len + ((20/100) * fb->len);
    bool is_realloc = client.setBufferSize(newSize);
    Serial.printf("\nRealloc buffer size from %zu to %zu\n", fb->len, newSize);

    if (!is_realloc) {
      ESP_LOGE(TAG, "Realloc buffer failed.");
      // don't go any further, mqtt publish won't work.
      return ESP_ERR_NO_MEM;
    } else {
      *mqttBufferSize = client.getBufferSize();
    }
  }

  char buff[100];
  const char* id = device_id.c_str();
  snprintf(buff, sizeof(buff), "ia/picture/%s", id);

  client.publish(buff, fb->buf, fb->len, false);
  return ESP_OK;
}

esp_err_t camera_capture() {
  //acquire a frame
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGE(TAG, "Camera Capture Failed");
    return ESP_FAIL;
  }

  Serial.printf("\nSize of picture: %zu\n", fb->len);

  esp_err_t processed = process_image(fb, &bufferSize);
  if (processed!= ESP_OK) {
    return processed;
  }

  // return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);
  return ESP_OK;
}

void mqttLoop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}

void initMqttCamera(String device_id) {
  char buff[100];
  const char* id = device_id.c_str();

  snprintf(buff, sizeof(buff), "status/camera_manager/%s", id);
  // subscribe to mqtt camera topics to up/off.
  client.subscribe(buff, 1);

  // I need to know if we had to ask bobby core the device_id because will is not set (because device id was unkown when mqtt started)
  // if so, I need to disconnect and reconnect the mqtt client :)
  snprintf(buff, sizeof(buff), "connected/camera_manager/%s", id);
  client.publish((const char *) buff, "\x01");
}

void camera_loop() {
  if (!run_camera) return;

  // if status given by core is true.
  if(millis() - time_now_send_picture > send_picture_period_ms) {
    time_now_send_picture = millis();
    camera_capture();
  }

  // ping is handled by object detection (listen_frame) service.
  // if(millis() - time_now_send_ping > send_ping_period_ms) {
  //   time_now_send_ping = millis();
  //   camera_ping();
  // }
}

void loop() {
  mqttLoop();
  camera_loop();
}
