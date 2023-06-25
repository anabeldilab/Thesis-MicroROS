#include <string.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <std_msgs/msg/header.h>

#include <rclc/rclc.h>

#include "pan_tilt_controller.h"

void publish_header_from_string(char *msg, rcl_publisher_t *publisher){
	std_msgs__msg__Header local_msg_log;
	int len = strlen(msg);
	local_msg_log.frame_id.data = msg;
	local_msg_log.frame_id.size = len;
	local_msg_log.frame_id.capacity = len;
	RCSOFTCHECK(rcl_publish(publisher, &local_msg_log, NULL));
}