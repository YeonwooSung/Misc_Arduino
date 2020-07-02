/**-----------------------------------------------------------------------------

    This is an audio output example with UDA1334 DAC and I2S interface.
   

  ----------------------------------------------------------------------------*/
#include <driver/i2s.h>
#include "sound.h"

#define I2SO_DATA         23     // I2S DATA OUT on GPIO23
#define I2SO_BCK          18     // I2S BCLK on GPIO18
#define I2SO_WSEL         5      // I2S SELECT on GPIO5

#define LED_PIN           2      // Pin number for on-board LED
#define SWITCH_PIN        0      // Pin number for on-board switch

#define VOLUME            0.5    // Between 0 and 1
#define BUFFER_SIZE       512
#define SAMPLE_RATE       16000

static volatile bool buttonPressed = false;


// Function definitions --------------------------------------------------------

void IRAM_ATTR handleInterrupt();

void setupAudioOutput(int sample_rate);

// Function declarations -------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), handleInterrupt, FALLING);
  setupAudioOutput(SAMPLE_RATE);


}

void loop()
{
  esp_err_t audioError;
  size_t bytesLength = 0;
  //short buffer[BUFFER_SIZE] = {0};
  int32_t ibuffer[BUFFER_SIZE] = {0};

  if (buttonPressed)
  {
     digitalWrite(LED_PIN, HIGH);
    //char *payload = "hello";
    //Serial.print("Sending data: ");
    //Serial.println(payload);
    buttonPressed = false;
  }

   
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
      ibuffer[i] = (int32_t) error_sound[i];
    }
    audioError = i2s_write(I2S_NUM_1, ibuffer, BUFFER_SIZE * 4, &bytesLength, portMAX_DELAY);

    digitalWrite(LED_PIN, LOW);
}

void IRAM_ATTR handleInterrupt()
{
  buttonPressed = true;
}


// I2S Audio -------------------------------------------------------------------

void setupAudioOutput(int sample_rate)
{
  /*
     Set up I2S audio for UDA1334 DAC output
  */
  esp_err_t err;
  Serial.println("Initialising audio output driver..");

  const i2s_config_t i2s_config =
  {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = sample_rate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_LSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = true
  };

  const i2s_pin_config_t pin_config =
  {
    .bck_io_num = I2SO_BCK,
    .ws_io_num = I2SO_WSEL,
    .data_out_num = I2SO_DATA,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  err = i2s_driver_install(I2S_NUM_1, &i2s_config, 0, NULL);
  if (err != ESP_OK)
  {
    Serial.printf("Failed installing driver: %d\n", err);
    while (true);
  }

  err = i2s_set_pin(I2S_NUM_1, &pin_config);
  if (err != ESP_OK)
  {
    Serial.printf("Failed setting pin: %d\n", err);
    while (true);
  }

  err = i2s_set_sample_rates(I2S_NUM_1, sample_rate);
  if (err != ESP_OK)
  {
    Serial.printf("Failed to set sample rates: %d\n", err);
    while (true);
  }

  /*
   * Known Issue
   * The output sample rate is not correct if I2S0 is not configured
   * This can be removed if receiving is set up on I2S0.
   */
  i2s_config_t i2s_config_stub = i2s_config;
  i2s_config_stub.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT;
  i2s_driver_install(I2S_NUM_0, &i2s_config_stub, 0, NULL);

  Serial.println("Audio output driver initalised.");
}
