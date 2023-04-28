#include <ArduinoJson.h>

void determine_memory() {
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    total = fs_info.totalBytes;
    used = fs_info.usedBytes;
    free_space = fs_info.totalBytes - fs_info.usedBytes;
}

void save_settings() {
    logging("Save setting :: Saving config file");
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["white_hat"] = white_hat;
    json["ssid"] = ssid;
    json["password"] = password;
    json["channel"] = channel;
    json["hidden"] = hidden;
    json["local_ip"] = local_ip_str;
    Serial.println(local_ip_str);
    json["local_mac"] = local_mac_default;
    json["gateway"] = gateway_str;
    Serial.println(gateway_str);
    json["subnet"] = subnet_str;
    Serial.println(subnet_str);
    json["update_username"] = update_username;
    json["update_password"] = update_password;
    json["ftp_username"] = ftp_username;
    json["ftp_password"] = ftp_password;
    json["ftp_enabled"] = ftp_enabled;
    json["esp_portal_enabled"] = esp_portal_enabled;
    
    File file = SPIFFS.open("/json/settings.json", "w");
    json.printTo(file);
    file.close();
    logging("Save settings :: config file saved");
}

void create_default_settings() {
    logging("Create default settings :: Creating config file");
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["white_hat"] = white_hat_default;
    json["ssid"] = ssid_default;
    json["password"] = password_default;
    json["channel"] = channel_default;
    json["hidden"] = hidden_default;
    json["local_ip"] = local_ip_default;
    json["local_mac"] = local_mac_default;
    json["gateway"] = gateway_default;
    json["subnet"] = subnet_default;
    json["update_username"] = update_username_default;
    json["update_password"] = update_password_default;
    json["ftp_username"] = ftp_username_default;
    json["ftp_password"] = ftp_password_default;
    json["ftp_enabled"] = ftp_enabled_default;
    json["esp_portal_enabled"] = esp_portal_enabled_default;

    File file = SPIFFS.open("/json/settings.json", "w");
    json.printTo(file);
    file.close();
    logging("Create default settings :: Config file created");
}

void create_attack_settings() {
    logging("Create attack settings :: Creating config file");
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["white_hat"] = white_hat_attack;
    json["ssid"] = ssid_attack;
    json["password"] = password_attack;
    json["channel"] = channel_attack;
    json["hidden"] = hidden_attack;
    json["local_ip"] = local_ip_attack;
    json["local_mac"] = local_mac_attack;
    json["gateway"] = gateway_attack;
    json["subnet"] = subnet_attack;
    json["update_username"] = update_username_attack;
    json["update_password"] = update_password_attack;
    json["ftp_username"] = ftp_username_attack;
    json["ftp_password"] = ftp_password_attack;
    json["ftp_enabled"] = ftp_enabled_attack;
    json["esp_portal_enabled"] = esp_portal_enabled_attack;

    File file = SPIFFS.open("/json/settings.json", "w");
    json.printTo(file);
    file.close();
    logging("Create attack settings :: Config file created");
}

void load_settings() {
    logging("Load settings :: Opening config file");

    File file = SPIFFS.open("/json/settings.json", "r");
    if(!file) {
        logging("Load settings :: No config file... Calling create_default_settings()");
        create_default_settings();
        logging("Load settings :: Calling load_defaults()");
        File file = SPIFFS.open("/json/settings.json", "r");
    }

    logging("Load settings :: Reading config file");
    size_t size = file.size();
    std::unique_ptr<char[]> buf(new char[size]);
    file.readBytes(buf.get(), size);

    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());
    white_hat = json["white_hat"];
    strcpy(ssid, (const char*)json["ssid"]);
    strcpy(password, (const char*)json["password"]);
    channel = json["channel"];

    hidden = json["hidden"];

    strcpy(local_ip_str, (const char*) json["local_ip"]);
    strcpy(local_mac, (const char*) json["local_mac"]);
    strcpy(gateway_str, (const char*) json["gateway"]);
    strcpy(subnet_str, (const char*) json["subnet"]);

    strcpy(update_username, (const char*) json["update_username"]);
    strcpy(update_password, (const char*) json["update_password"]);

    strcpy(ftp_username, (const char*) json["ftp_username"]);
    strcpy(ftp_password, (const char*) json["ftp_password"]);
    ftp_enabled = json["ftp_enabled"];
    esp_portal_enabled = json["esp_portal_enabled"];

    file.close();
    logging("Load settings :: Config file Done");
}

void create_system() {
    logging("Create system :: Creating index file");
    StaticJsonBuffer<500> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    JsonArray& arr = jsonBuffer.createArray();
    json["local_ip"] = local_ip_str;
    json["local_mac"] = local_mac;
    json["channel"] = channel;
    determine_memory();
    json["total"] = total;
    json["used"] = used;
    json["free_space"] = free_space;

    int i = 0;
    String str = "";
    String str_file_name = "";
    int int_file_name;
    num_stations_connected = 0;
    Dir dir = SPIFFS.openDir("/connected/");
    
    while(dir.next()) {
        num_stations_connected++;
        str += dir.fileName();
        str = get_file_name(str, '/', 2);
        arr.add(str);
        i++;
    }

    json["list_of_connect_stations"] = arr;
    json["num_stations_connected"] = num_stations_connected;

    File file = SPIFFS.open("/json/system.json", "w");
    json.printTo(file);
    file.close();

    logging("Create system :: index file Done");
}