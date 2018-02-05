#include <libconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <wiringPi.h>

/// The maximum temperature allowed before the fan turns on.
const float temp_threshold_default = 40.0f;
/// The <b>General</b> GPIO pin number to trigger the fan on.
/// <a href="https://en.wikipedia.org/wiki/Raspberry_Pi#General_purpose_input-output_(GPIO)_connector">See GPIO pinout</a>
const int pin_default = 1;

struct Config {
    float temp_threshold;
    int   pin;
};

/// Reads the config file from /usr/share/rpifan/config.cfg
/// \return The config
struct Config read_config() {
    struct Config config;
    config_t cfg;
    const char *str;

    openlog("rpifan", LOG_ODELAY, LOG_USER);

    config_init(&cfg);

    // Load default if config file is missing
    if(!config_read_file(&cfg, "/usr/share/rpifan/config.cfg"))
    {
        syslog(LOG_WARNING, "Config file could not be opened, using defaults for all values.");
        config.temp_threshold = temp_threshold_default;
        config.pin = pin_default;
        return config;
    }

    // Read temperature threshold
    if(config_lookup_string(&cfg, "temperature_threshold", &str))
        config.temp_threshold = strtof (str, NULL); // Returns 0 if there is an error
    else {
        syslog(LOG_WARNING, "Config file is missing temp_threshold, using default.");
        config.temp_threshold = temp_threshold_default;
    }

    // Read temperature threshold
    if(config_lookup_string(&cfg, "gpio_pin", &str))
        config.pin = (int) strtol (str, NULL, 10); // Returns 0 if there is an error
    else {
        syslog(LOG_WARNING, "Config file is missing pin, using default.");
        config.pin = pin_default;
    }

    closelog();
    return config;
}

/// Reads the CPU temperature.
/// \return CPU temperature in degrees Celsius, returns -1 if there was an error.
float cpu_temp() {
    FILE *temp_file;
    char temp_str[10];
    float temp;

    temp_file = fopen ("/sys/class/thermal/thermal_zone0/temp", "r");
    if (temp_file == NULL)
        return -1.0f; //print some message
    fgets(temp_str, sizeof(temp_str) - 1, temp_file);

    temp = strtof (temp_str, NULL);
    temp /= 1000;
    fclose (temp_file);
    return temp;
}

void fan_base(int pin) {
    wiringPiSetup();
    pinMode(pin, OUTPUT);
}

/// Turns the fan on.
void fan_on(int pin) {
    fan_base(pin);
    digitalWrite(pin, HIGH);
}

/// Turns the fan off.
void fan_off(int pin) {
    fan_base(pin);
    digitalWrite(pin, LOW);
}

int main() {
    struct Config config = read_config();

    if (cpu_temp() > config.temp_threshold)
        fan_on(config.pin);
    else
        fan_off(config.pin);
    return 0;
}