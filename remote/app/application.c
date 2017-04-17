#include <application.h>
#include <bcl.h>

#define PREFIX_TALK_REMOTE "climate-station-001-remote"
#define DEBUG false
#define MEASUREMENT_DELAY 60000

// LED instance
bc_led_t led;

// Button instance
bc_button_t button;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *event_param)
{
    (void) self;
    (void) event_param;

    if (event == BC_BUTTON_EVENT_PRESS)
    {
        bc_led_pulse(&led, 100);
        static uint16_t event_counter = 0;
        event_counter++;
        if (DEBUG) {
            usb_talk_publish_push_button(PREFIX_TALK_REMOTE, &event_counter);
        } else { 
            bc_radio_pub_push_button(&event_counter);
        }
    }
    else if (event == BC_BUTTON_EVENT_HOLD)
    {
        bc_radio_enroll_to_gateway();
        bc_led_pulse(&led, 1000);
    }
}

void application_init(void) {
    usb_talk_init();

    // Initialize LED
    bc_led_init(&led, BC_GPIO_LED, false, false);

    // Initialize button
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    // Initialize radio
    bc_radio_init();

    // Initialize climate module
    bc_module_climate_init();
    bc_module_climate_set_update_interval_thermometer(MEASUREMENT_DELAY);
    bc_module_climate_set_update_interval_lux_meter(MEASUREMENT_DELAY);
    bc_module_climate_set_update_interval_hygrometer(MEASUREMENT_DELAY);
    bc_module_climate_set_update_interval_barometer(MEASUREMENT_DELAY);
    bc_module_climate_set_event_handler(climate_event_handler, NULL);

    // Initialize CO2 module
    bc_module_co2_init();
    bc_module_co2_set_update_interval(MEASUREMENT_DELAY);
    bc_module_co2_set_event_handler(co2_event_handler, NULL);
}

void climate_event_handler(bc_module_climate_event_t event, void *event_param)
{
    (void) event_param;
    float value;
    float meter, pascal;

    static uint8_t i2c_thermometer = 0x48;
    static uint8_t i2c_lux_meter = 0x44;
    static uint8_t i2c_hygrometer = 0x40;
    static uint8_t i2c_barometer = 0x60;

    switch (event)
    {
        case BC_MODULE_CLIMATE_EVENT_UPDATE_THERMOMETER:
            if (bc_module_climate_get_temperature_celsius(&value))
            {
                if (DEBUG) {
                    usb_talk_publish_thermometer(PREFIX_TALK_REMOTE, &i2c_thermometer, &value);
                } else { 
                    bc_radio_pub_thermometer(i2c_thermometer, &value);
                }
            }
            break;

        case BC_MODULE_CLIMATE_EVENT_UPDATE_LUX_METER:
            if (bc_module_climate_get_luminosity_lux(&value))
            {
                if (DEBUG) {
                    usb_talk_publish_lux_meter(PREFIX_TALK_REMOTE, &i2c_lux_meter, &value);
                } else {
                    bc_radio_pub_luminosity(i2c_lux_meter, &value);
                }
            }
            break;

        case BC_MODULE_CLIMATE_EVENT_UPDATE_HYGROMETER:
            if (bc_module_climate_get_humidity_percentage(&value))
            {
                if (DEBUG) {
                    usb_talk_publish_humidity_sensor(PREFIX_TALK_REMOTE, &i2c_hygrometer, &value);
                } else {
                    bc_radio_pub_humidity(i2c_hygrometer, &value);
                }
            }
            break;
        
        case BC_MODULE_CLIMATE_EVENT_UPDATE_BAROMETER:
            if (bc_module_climate_get_altitude_meter(&meter) && bc_module_climate_get_pressure_pascal(&pascal))
            {
                if (DEBUG) {
                    usb_talk_publish_barometer(PREFIX_TALK_REMOTE, &i2c_barometer, &pascal, &meter);
                } else {
                    bc_radio_pub_barometer(i2c_barometer, &pascal, &meter);
                }
            }
            break;        
        case BC_MODULE_CLIMATE_EVENT_ERROR_HYGROMETER:
        case BC_MODULE_CLIMATE_EVENT_ERROR_BAROMETER:
        case BC_MODULE_CLIMATE_EVENT_ERROR_LUX_METER:
        case BC_MODULE_CLIMATE_EVENT_ERROR_THERMOMETER:
        default:
            break;
    }
}

void co2_event_handler(bc_module_co2_event_t event, void *event_param) 
{
    (void) event_param;
    int16_t value;
    static uint8_t i2c_co2 = 0x38;

    switch (event)
    {
        case BC_MODULE_CO2_EVENT_UPDATE:
            if (bc_module_co2_get_concentration(&value)) 
            {
                if (DEBUG) {
                    usb_talk_publish_co2_concentation(PREFIX_TALK_REMOTE, &i2c_co2, &value);
                } else {
                    bc_radio_pub_co2(i2c_co2, &value);
                }
            }
            break;
        
        case BC_MODULE_CO2_EVENT_ERROR:
        default:
            break;
    }


}
