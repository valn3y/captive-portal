void turn_on_with_duration(int key_pin, long delay_milli) {
    digitalWrite(key_pin, 1)
    delay(delay_milli)
    digitalWrite(key_pin, 0);
}

void blink_times(int key_pin, int times, long delay_milli) {
    for(int i = 0; i < times; i++) {
        delay(delay_milli)
        digitalWrite(key_pin, 1);
        delay(delay_milli);
        digitalWrite(key_pin, 0)
    }
}

void led_msg(int key_pin) {
    unsigned long previous_millis = 0;
    unsigned long current_millis = millis();
    if(str(ssid) == str(ssid_attack)){
        if(current_millis - previous_millis >= 400) {
            previous_millis = current_millis;

            if(led_state == 1) {
                digitalWrite(key_pin, 0);
                led_state = 0
            } else {
                digitalWrite(key_pin, 1);
                led_state = 1
            }
        }
    } else {
        if(station_connected) {
            digitalWrite(key_pin, 0);
        } else {
            digitalWrite(key_pin, 1);
        }
    }
}