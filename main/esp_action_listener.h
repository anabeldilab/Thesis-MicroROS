#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_tls.h"
#include "driver/uart.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/header.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <rosidl_runtime_c/string_functions.h>

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>
#include "esp32_serial_transport.h"
// WiFi management relate
#include "freertos/event_groups.h"
#include "esp_wifi.h"

#include "nvs_flash.h"

#include "lwip/ip4_addr.h"

#include "esp_http_client.h"

#include "pan_tilt_controller.h"


//#include "UI_conf.h"
#include "wifi_conf.h"

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Aborting.\n",__LINE__,(int)temp_rc);vTaskDelete(NULL);}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){printf("Failed status on line %d: %d. Continuing.\n",__LINE__,(int)temp_rc);}}

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MACSTRLENGTH 18 

extern rcl_publisher_t *log_sap_publisher;
extern uint8_t target_mac[6];

void publish_header_from_string(char *msg, rcl_publisher_t *publisher);
void wifi_event_handler_sap(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data);

void wifi_init_softap(rcl_publisher_t *publisher,uint8_t *client_mac);
void wifi_init_softap_no_ros(uint8_t *client_mac);
void wifi_disconnect(rcl_publisher_t *publisher);
void mac2str(char*, uint8_t*);
bool compare_macs(uint8_t*, uint8_t*);
bool get_target_ip(ip4_addr_t *ip);
char *get_camera_ip(void);
