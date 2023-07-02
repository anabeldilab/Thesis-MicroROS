#include "esp_action_listener.h"

//static uint8_t target_mac[6]={188,221,194,36,109,200};

void wifi_disconnect(rcl_publisher_t *publisher){
  publish_header_from_string("Stopping WiFi in base station", publisher);
  ESP_ERROR_CHECK(esp_wifi_disconnect());
  //ESP_ERROR_CHECK(esp_wifi_stop());
  //ESP_ERROR_CHECK(esp_wifi_deinit());
  publish_header_from_string("WiFi Stopping process finished", publisher);
}


bool compare_macs(uint8_t *mac1, uint8_t *mac2){
  bool result = true;
  for(int i = 0; i < 6; i++)
    if(mac1[i] != mac2[i]) {
      result = false;
      break;
    }
  return result;
}

/*
bool get_target_ip(ip4_addr_t *ip){

wifi_sta_list_t wifi_sta_list;
tcpip_adapter_sta_list_t adapter_sta_list;
memset(&wifi_sta_list, 0, sizeof(wifi_sta_list));
memset(&adapter_sta_list, 0, sizeof(adapter_sta_list));
esp_wifi_ap_get_sta_list(&wifi_sta_list);
tcpip_adapter_get_sta_list(&wifi_sta_list, &adapter_sta_list);
for (int i = 0; i < adapter_sta_list.num; i++) {
  // printing the station information

   tcpip_adapter_sta_info_t station = adapter_sta_list.sta[i];
   bool found=true;
   for(int j=0;j<6;j++)
     if(target_mac[j]!=station.mac[j]){
       found=false;
       break;
     }
   if(found){
  ip->addr=station.ip.addr;
  return true;
   }

}
return false;
}
*/



void mac2str(char *dest, uint8_t* mac){
//dest requires 18 bytes
char temp[3];
memset(dest,0,MACSTRLENGTH);
int pos=0;
for(int i=0;i<6;i++){
  itoa(mac[i],temp,16);
  strcpy(dest+pos,temp);
  pos=strlen(dest);
  if(i!=5)
    dest[pos++]='-';
}

}

