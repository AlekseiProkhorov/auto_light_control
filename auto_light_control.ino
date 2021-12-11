#define RELAY_PIN 8
#define AUTO_INDICATOR_PIN 7
#define informer_interval 5000

#include <SoftwareSerial.h>
#include "TimerOne.h"

SoftwareSerial remote_serial(3, 2);
int light_analog_value = 0;
String light_state = "";
String current_status = "";
String command = "";
bool automatic_control = 1;
unsigned long previousMillis = 0;

void light_level_control() {
    light_analog_value = analogRead(A0);
    light_analog_value = map(light_analog_value, 0, 1000, 0, 100);

    if (automatic_control) {
        digitalWrite(AUTO_INDICATOR_PIN, HIGH);
        if (light_analog_value < 50) {
            light_state = "ON";
            digitalWrite(RELAY_PIN, HIGH);
        } else {
            digitalWrite(RELAY_PIN, LOW);
            light_state = "OFF";
        }
    } else {
        digitalWrite(AUTO_INDICATOR_PIN, LOW);
    }
}

void current_status_informer() {
    if (millis() - previousMillis >= informer_interval) {
        previousMillis = millis();
        current_status = "Current light level: " + String(light_analog_value) + "%\n" + "Ext. light state: " + String((digitalRead(RELAY_PIN)) ? "ON" : "OFF") + "\n" + "Auto light control: " + String((automatic_control) ? "ON" : "OFF") + "\n";
        remote_serial.println(current_status);
        Serial.println(current_status);
    }
}

void check_if_new_command() {
    if (remote_serial.available()) {
        command = remote_serial.readStringUntil("\n");
        remote_command_handle();
    }
}

void remote_command_handle() {
    if (command == "AUTO:OFF\n") {
        automatic_control = 0;
        remote_serial.println("Automatic light controll is turned off\n");
    } else if (command == "AUTO:ON\n") {
        automatic_control = 1;
        remote_serial.println("Automatic light controll is turned on\n");
    } else if (command == "LIGHT:ON\n") {
        if (automatic_control == 1) {
            remote_serial.println("You should turn off auto light\n");
        } else {
            digitalWrite(RELAY_PIN, HIGH);
            remote_serial.println("External light is manually turned on\n");
        }
    } else if (command == "LIGHT:OFF\n") {
        if (automatic_control == 1) {
            remote_serial.println("You should turn off auto light\n");
        } else {
            digitalWrite(RELAY_PIN, LOW);
            remote_serial.println("External light is manually turned off\n");
        }
    } else {
        remote_serial.println("This command is incorrect: " + command + "\n" + "Please try again.");
    }
}

void setup() {
    Serial.begin(9600);
    remote_serial.begin(9600);

    pinMode(RELAY_PIN, OUTPUT);
    pinMode(AUTO_INDICATOR_PIN, OUTPUT);

    Timer1.initialize(1000000);
    Timer1.attachInterrupt(light_level_control);
}

void loop() {
    check_if_new_command();
    current_status_informer();
}
