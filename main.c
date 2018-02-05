#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>

/// The maximum temperature allowed before the fan turns on.
float temp_limit = 40.0f;
/// The <b>General</b> GPIO pin number to trigger the fan on.
/// <a href="https://en.wikipedia.org/wiki/Raspberry_Pi#General_purpose_input-output_(GPIO)_connector">See GPIO pinout</a>
int pin = 1;

void fan_on();

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

void fan_base() {
    wiringPiSetup();
    pinMode(pin, OUTPUT);
}

/// Turns the fan on.
void fan_on() {
    fan_base();
    digitalWrite(pin, HIGH);
}

/// Turns the fan off.
void fan_off() {
    fan_base();
    digitalWrite(pin, LOW);
}

int main() {
    if (cpu_temp() > temp_limit)
        fan_on();
    else
        fan_off();
    return 0;
}