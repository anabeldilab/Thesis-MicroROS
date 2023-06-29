#include "esp_action_listener.h"

#define MAXLENGTHMESSAGE 100
#define LENGTHMSGCOMMAND 5
#define ANGLEINCREMENT 10

rcl_subscription_t subscriber;
rcl_publisher_t publisher;
std_msgs__msg__Header msgSubscriber;
std_msgs__msg__Header msgPublisher;

bool b_wifi_disconnect = false;

void subscription_callback(const void * msgin) {

  char read_data[MAXLENGTHMESSAGE + 1];
  char read_command[LENGTHMSGCOMMAND];

  for(int i = 0; i < MAXLENGTHMESSAGE; i++)
    read_data[i] = 0;

  const std_msgs__msg__Header * msg = (const std_msgs__msg__Header *)msgin;

  if (msg->frame_id.size > LENGTHMSGCOMMAND + MAXLENGTHMESSAGE) {
    printf("Error: the message is too long.\n");
    return;
  }

  // Get the command
  memcpy(read_command, msg->frame_id.data, LENGTHMSGCOMMAND);

  // Get the data
  memcpy(read_data, msg->frame_id.data + LENGTHMSGCOMMAND, msg->frame_id.size - LENGTHMSGCOMMAND);

  // Print the command
  msgPublisher.frame_id.data = read_command;
  msgPublisher.frame_id.size = LENGTHMSGCOMMAND;
  msgPublisher.frame_id.capacity = LENGTHMSGCOMMAND;
  RCSOFTCHECK(rcl_publish(&publisher, &msgPublisher, NULL));

  // Print the data
  msgPublisher.frame_id.data = read_data;
  msgPublisher.frame_id.size = msg->frame_id.size - LENGTHMSGCOMMAND;
  msgPublisher.frame_id.capacity = MAXLENGTHMESSAGE;
  RCSOFTCHECK(rcl_publish(&publisher, &msgPublisher, NULL));
  
  if(strcmp(read_command, "ACT_/") == 0) {
		ip4_addr_t ip;
		char ipstr[20];
		char str_log[100];
		if(get_target_ip(&ip)) {
      strcpy(str_log, "Action to station");
      strcpy(ipstr, ip4addr_ntoa(&ip));
      strcat(str_log, ipstr);
		}
		else {
			strcpy(str_log,"Target station not found");
		}
	  publish_header_from_string(str_log, &publisher);		 

    if (strcmp(read_data, "Up") == 0) {
      set_vertical_angle(pan_tilt_state.vertical_servo.angle + ANGLEINCREMENT);
    } else if (strcmp(read_data, "Down") == 0) {
      set_vertical_angle(pan_tilt_state.vertical_servo.angle - ANGLEINCREMENT);
    } else if (strcmp(read_data, "Left") == 0) {
      set_horizontal_angle(pan_tilt_state.horizontal_servo.angle - ANGLEINCREMENT);
    } else if (strcmp(read_data, "Right") == 0) {
      set_horizontal_angle(pan_tilt_state.horizontal_servo.angle + ANGLEINCREMENT);
    }
  }
}

void micro_ros_task(void * arg) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
  rclc_support_t support;

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // create node
  rcl_node_t node;
  RCCHECK(rclc_node_init_default(&node, "action_RosSubscriber", "", &support));

  // create subscriber
  RCCHECK(rclc_subscription_init_default(
    &subscriber,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header),
    "/action"));

  // create publisher
  RCCHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Header),
    "/freertos_header_log"));

  // create executor
  rclc_executor_t executor;
  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

  // add subscriber callback to the executor
  RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msgSubscriber, &subscription_callback, ON_NEW_DATA))

  default_pan_tilt_init();

  // Initialize subscriber message memory
  char string_memory[MAXLENGTHMESSAGE];
  msgSubscriber.frame_id.data = string_memory;
  msgSubscriber.frame_id.size = 0;
  msgSubscriber.frame_id.capacity = LENGTHMSGCOMMAND + MAXLENGTHMESSAGE;

  publish_header_from_string("Starting soft access point", &publisher);
  wifi_init_softap(&publisher, target_mac);

  while(1) {
		rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
    usleep(10000);
  }

  wifi_disconnect(&publisher);

  // free resources
  RCCHECK(rclc_executor_fini(&executor))
  RCCHECK(rcl_subscription_fini(&subscriber, &node));
  RCCHECK(rcl_publisher_fini(&publisher, &node));
  RCCHECK(rcl_node_fini(&node));
  RCCHECK(rclc_support_fini(&support));

  vTaskDelete(NULL);
}

static size_t uart_port = UART_NUM_0;

void app_main(void * arg) {
	//Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);
  // wifi_init_softap(&publisher);
  // wifi_init_sta();

#if defined(RMW_UXRCE_TRANSPORT_CUSTOM)
  rmw_uros_set_custom_transport(
    true,
    (void *) &uart_port,
    esp32_serial_open,
    esp32_serial_close,
    esp32_serial_write,
    esp32_serial_read
  );
#else
#error micro-ROS transports misconfigured
#endif  // RMW_UXRCE_TRANSPORT_CUSTOM

  xTaskCreate(micro_ros_task,
            "uros_task",
            CONFIG_MICRO_ROS_APP_STACK,
            NULL,
            CONFIG_MICRO_ROS_APP_TASK_PRIO,
            NULL);
  
}