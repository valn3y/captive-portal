int station_connected = 0;
int station_phished = 0;
int led_state = 0;

int white_hat;
char ssid[32];
char password[64];
int channel;
int hidden;

char local_ip_str[16];
char local_mac[20];
char gateway_str[16];
char subnet_str[16];

char update_username[32];
char update_password[64];

char ftp_username[32];
char ftp_password[64];
int ftp_enabled;
int esp_portal_enabled;

String total;
String used;
String free_space;

int num_stations_connected;
int num_stations_have_connected;
int list_of_connect_stations[8];

char station_ip[50];
char station_mac[32];
int station_id = 0;

const char *white_hat_default = "0";
const char *esp_portal_enabled_default = "1";

const char *ssid_default = "esp-phishing";
const char *password_default = "espphishing";
const char *channel_default = "11";
const char *hidden_default = "1";

const char *local_ip_default = "192.168.4.22";
const char *local_mac_default = "00:E0:4C:26:AF:C7";
const char *gateway_default = "192.168.4.9";
const char *subnet_default = "255.255.255.0";

const char *update_username_default = "esp-phishing";
const char *update_password_default = "esphishing";

const char *ftp_username_default = "esp-phishing";
const char *ftp_password_default = "esphishing";
const char *ftp_enabled_default = "1";

const char *white_hat_attack = "0";
const char *ssid_attack = "esp-phishing";
const char *password_attack = "senha";
const char *channel_attack = "11";
const char *hidden_attack = "1";
const char *local_ip_attack = "192.168.15.1";
const char *local_mac_attack = "00:E0:4C:26:AF:C7";
const char *gateway_attack = "192.168.15.1";
const char *subnet_attack = "255.255.255.0";
const char *update_username_attack = "esp-phishing";
const char *update_password_attack = "esphishing";
const char *ftp_username_attack = "esp-phishing";
const char *ftp_password_attack = "esphishing";
const char *ftp_enabled_attack = "1";
const char *esp_portal_enabled_attack = "1";