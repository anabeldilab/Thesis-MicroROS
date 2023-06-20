#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>

#include <std_msgs/msg/string.h>

// ######################### MICRO-ROS TASK  #########################

#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "esp_log.h"
#include "esp_system.h"
#include "driver/uart.h"

#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>


// ######################### MICRO-ROS TASK  #########################

#include "pan_tilt_controller.h"
#include "esp32_serial_transport.h"


#define MAXLENGTHMESSAGE 200
#define LENGTHMSGCOMMAND 5

rcl_subscription_t subscriber;
rcl_publisher_t publisher;
std_msgs__msg__String msgSubscriber;
std_msgs__msg__String msgPublisher;

void subscription_callback(const void * msgin) {
	const std_msgs__msg__String * msg = (const std_msgs__msg__String *)msgin;

	char read_command[LENGTHMSGCOMMAND];

	for(int i=0;i<LENGTHMSGCOMMAND;i++)
		read_command[i]=msg->data.data[i];

	// Ponemos el mensaje en la direccion de memoria del mensaje del publisher
	msgPublisher.data.data = read_command;
	msgPublisher.data.size = LENGTHMSGCOMMAND;
	msgPublisher.data.capacity = LENGTHMSGCOMMAND;

	if (strcmp(msg->data.data, "Up") == 0) {
		set_vertical_angle(pan_tilt_state.vertical_servo.angle + 10);
		RCSOFTCHECK(rcl_publish(&publisher, &msgPublisher, NULL));
	} else if (strcmp(msg->data.data, "Down") == 0) {
		set_vertical_angle(pan_tilt_state.vertical_servo.angle - 10);
		RCSOFTCHECK(rcl_publish(&publisher, &msgPublisher, NULL));
	} else if (strcmp(msg->data.data, "Left") == 0) {
		set_horizontal_angle(pan_tilt_state.horizontal_servo.angle - 10);
		RCSOFTCHECK(rcl_publish(&publisher, &msgPublisher, NULL));
	} else if (strcmp(msg->data.data, "Right") == 0) {
		set_horizontal_angle(pan_tilt_state.horizontal_servo.angle + 10);
		RCSOFTCHECK(rcl_publish(&publisher, &msgPublisher, NULL));
	}

	RCSOFTCHECK(rcl_publish(&publisher, &msgPublisher, NULL));
}

void micro_ros_task(void * arg) {
  rcl_allocator_t allocator = rcl_get_default_allocator();
	rclc_support_t support;

	RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

	// create node
	rcl_node_t node;
	RCCHECK(rclc_node_init_default(&node, "dir_goal_RosSubscriber", "", &support));

	// create subscriber
	RCCHECK(rclc_subscription_init_default(
		&subscriber,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
		"/dir_goal"));

	// create publisher
	RCCHECK(rclc_publisher_init_default(
		&publisher,
		&node,
		ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, String),
		"/debug_topic"));

	// create executor
	rclc_executor_t executor;
	RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));

	// add subscriber callback to the executor
	RCCHECK(rclc_executor_add_subscription(&executor, &subscriber, &msgSubscriber, &subscription_callback, ON_NEW_DATA))

	pan_tilt_init();

	// Initialize subscriber message memory
	char string_subscriber_memory[MAXLENGTHMESSAGE];
	msgSubscriber.data.data = &string_subscriber_memory[0];
	msgSubscriber.data.size = 0;
	msgSubscriber.data.capacity = MAXLENGTHMESSAGE;

	char string_publisher_memory[MAXLENGTHMESSAGE];
	msgPublisher.data.data = &string_publisher_memory[0];
	msgPublisher.data.size = (unsigned)20;
	msgPublisher.data.capacity = MAXLENGTHMESSAGE;

	while(1) {
		rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
		usleep(100000);
	}

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