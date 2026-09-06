/* Example
   web
   server
   for
   controlling
   peripherals

*/

extern "C" {
#include "driver/ledc.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
}

#include <memory>
#include <string>

#include "esp32/gpio_esp32.h"
#include "esp32/http_server_esp32.h"
#include "esp32/pwm_esp32.h"
#include "hal/gpio/gpio_interface.h"
#include "hal/motor/motor_interface.h"
#include "hal/motor/motor_l298n.h"
#include "hal/pwm/pwm_interface.h"
#include "web/device_web_app.h"
#include "web/peripherals/led_web.h"
#include "web/peripherals/motor_web.h"
#include "web/peripherals/pwm_web.h"

/*
 The WIFI
 name is
 stored in
 KConfig.projbuild
 but true
 password
 is hidden
 in my
 .gitignore'd
 "wifi_password.h".
 Create
 the file
 "wifi_password.h"
 and add:
 #undef
 CONFIG_WIFI_PASSWORD
 #define
 CONFIG_WIFI_PASSWORD
 <your_password_here>
*/
#include "wifi_password.h"

static const char *THREAD_TAG = "WEB_SERVER";

using namespace rover::hal;
using namespace rover::web;

// ----
// Hardware
static GpioInterface *green_led;
static GpioInterface *red_led;
static HttpServerInterface *debug_server;

// Temporary:
// L298N
// motor
// as two
// GPIOs
// and a
// PWM
static GpioInterface *motor_forward;
static GpioInterface *motor_reverse;
static PwmInterface *motor_speed;

static GpioInterface *motor2_forward;
static GpioInterface *motor2_reverse;
static PwmInterface *motor2_speed;
static MotorInterface *motor2;

// ----
// Web
// Peripheral
// Integrations
static rover::web::PeripheralInterface *green_led_web;
static rover::web::PeripheralInterface *red_led_web;

static rover::web::PeripheralInterface *motor_forward_web;
static rover::web::PeripheralInterface *motor_reverse_web;
static rover::web::PeripheralInterface *motor_speed_web;

static rover::web::PeripheralInterface *motor2_web;

// --- Web
// application
static rover::web::DeviceWebApp *app;

extern "C" {
void app_main()
{
    esp_log_level_set(THREAD_TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_event_loop_create_default();

    // ----
    // Hardware
    // initializations
    // ----
    debug_server = new HttpServerEsp32(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    green_led = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_26);
    red_led = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_22);

    /* ---
     * Front
     * left
     * ----*/
    motor2_forward = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_5);
    motor2_reverse = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_18);
    motor2_speed = new PwmEsp32(GPIO_NUM_19, LEDC_CHANNEL_1, LEDC_TIMER_1);
    motor2 = new MotorL298N(*motor2_speed, *motor2_forward, *motor2_reverse);

    /* ---
     * Back
     * right
     * ----*/
    // motor_forward
    // =
    // new
    // GpioEsp32(GpioDirection::OUTPUT,
    // GPIO_NUM_27);
    // motor_reverse
    // =
    // new
    // GpioEsp32(GpioDirection::OUTPUT,
    // GPIO_NUM_14);
    // motor_speed
    // =
    // new
    // PwmEsp32(GPIO_NUM_12,
    // LEDC_CHANNEL_0,
    // LEDC_TIMER_0);

    // /*
    // ---
    // Front
    // right
    // (Broken)
    // ----*/
    // motor_forward
    // =
    // new
    // GpioEsp32(GpioDirection::OUTPUT,
    // GPIO_NUM_4);
    // //
    // GPIO
    // 2
    // is
    // also
    // Blue
    // on-board
    // LED
    // motor_reverse
    // =
    // new
    // GpioEsp32(GpioDirection::OUTPUT,
    // GPIO_NUM_2);
    // motor_speed
    // =
    // new
    // PwmEsp32(GPIO_NUM_15,
    // LEDC_CHANNEL_0,
    // LEDC_TIMER_0);

    /* ---
     * Back
     * left
     * ----*/
    motor_forward = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_33);
    motor_reverse = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_25);
    motor_speed = new PwmEsp32(GPIO_NUM_32, LEDC_CHANNEL_0, LEDC_TIMER_0);

    // ----
    // Html
    // rendering
    // application
    // ----
    app = new rover::web::DeviceWebApp(*debug_server);

    // ---
    // Add
    // peripherals
    // to
    // webpage
    // ----
    green_led_web = new rover::web::LedWeb("green_led", green_led);
    red_led_web = new rover::web::LedWeb("red_led", red_led);

    motor_forward_web = new rover::web::LedWeb("m_forward", motor_forward);
    motor_reverse_web = new rover::web::LedWeb("m_reverse", motor_reverse);
    motor_speed_web = new rover::web::PwmWeb("m_speed", motor_speed);

    motor2_web = new rover::web::MotorWeb("front_left", motor2);

    app->add_peripheral(green_led_web);
    app->add_peripheral(red_led_web);

    app->add_peripheral(motor_forward_web);
    app->add_peripheral(motor_reverse_web);
    app->add_peripheral(motor_speed_web);

    app->add_peripheral(motor2_web);
}
}
