#include "esp_action_listener.h"

//static const char *TAG_sap = "wifi softAP";
//static uint8_t *sap_target_mac;
static ip4_addr_t sap_ip_target;
static bool sap_ip_target_assigned = false;
uint8_t target_mac[6] = {120, 33, 132, 155, 205, 152};


void wifi_event_handler_sap(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data) {
	char logstr[100];

  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
    char macstr[MACSTRLENGTH];
    mac2str(macstr, event->mac);
    strcpy(logstr, "Station connected ");
    strcat(logstr, macstr);
    publish_header_from_string(logstr, log_sap_publisher);
    // ESP_LOGI(TAG_sap, "station %s join, AID=%d",
    // MAC2STR(event->mac), event->aid);
  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
    publish_header_from_string("Station disconnected", log_sap_publisher);
    if(compare_macs(target_mac, event->mac)) {
      publish_header_from_string("Target station disconnected", log_sap_publisher);
      sap_ip_target_assigned = false;
    }
    // ESP_LOGI(TAG_sap, "station %s leave, AID=%d", MAC2STR(event->mac), event->aid);
  } else if (event_id == IP_EVENT_AP_STAIPASSIGNED) {
    ip_event_ap_staipassigned_t *event = (ip_event_ap_staipassigned_t *)event_data;
    if(compare_macs(target_mac, event->mac)) {
      char ipstr[20];
      sap_ip_target.addr = event->ip.addr;
      sap_ip_target_assigned = true;
      strcpy(logstr,"Target station has ip ");
      strcpy(ipstr, ip4addr_ntoa(&sap_ip_target));
      strcat(logstr, ipstr);
      publish_header_from_string(logstr, log_sap_publisher);
    } else {
	     char ipstr[20];
	     ip4_addr_t ip;
	     ip.addr=event->ip.addr;
	     strcpy(logstr,"Ip assigned to a non-target station ");
	     strcpy(ipstr,ip4addr_ntoa(&ip));
	     strcat(logstr,ipstr);
	     publish_header_from_string(logstr,log_sap_publisher);
    }
  }
}

char *get_camera_ip(void) {
  char *ipstr = NULL;
  if(sap_ip_target_assigned) {
    ipstr = ip4addr_ntoa(&sap_ip_target);
  }
  return ipstr;
}

bool get_target_ip(ip4_addr_t *ip) {
	if(!sap_ip_target_assigned)
		return false;
	ip->addr = sap_ip_target.addr;
	return true;
}

void wifi_init_softap(rcl_publisher_t *publisher, uint8_t *client_mac) {
    // Init sap publisher
    log_sap_publisher = publisher; 
    publish_header_from_string("Starting Soft Access Point", publisher);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_err_t ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
      char error_msg[128];
      snprintf(error_msg, sizeof(error_msg), "Error en esp_wifi_init: %d", ret);
      publish_header_from_string(error_msg, publisher);
      return;
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler_sap,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
			    				                                      IP_EVENT_AP_STAIPASSIGNED,
                                                        &wifi_event_handler_sap,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = SAP_ESP_WIFI_SSID,
            .ssid_len = strlen(SAP_ESP_WIFI_SSID),
            .channel = SAP_ESP_WIFI_CHANNEL,
            .password = SAP_ESP_WIFI_PASS,
            .max_connection = SAP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(SAP_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    publish_header_from_string("Wifi init softap finished", publisher);
    //ESP_LOGI(TAG_sap, "wifi_init_softap finished. SSID:%s password:%s channel:%d", SAP_ESP_WIFI_SSID, SAP_ESP_WIFI_PASS, SAP_ESP_WIFI_CHANNEL);
}


void wifi_init_softap_no_ros(uint8_t *client_mac) {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    esp_err_t ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
      char error_msg[128];
      snprintf(error_msg, sizeof(error_msg), "Error en esp_wifi_init: %d", ret);
      return;
    }

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler_sap,
                                                        NULL,
                                                        NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
			    				                                      IP_EVENT_AP_STAIPASSIGNED,
                                                        &wifi_event_handler_sap,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = SAP_ESP_WIFI_SSID,
            .ssid_len = strlen(SAP_ESP_WIFI_SSID),
            .channel = SAP_ESP_WIFI_CHANNEL,
            .password = SAP_ESP_WIFI_PASS,
            .max_connection = SAP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(SAP_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    //ESP_LOGI(TAG_sap, "wifi_init_softap finished. SSID:%s password:%s channel:%d", SAP_ESP_WIFI_SSID, SAP_ESP_WIFI_PASS, SAP_ESP_WIFI_CHANNEL);
}
