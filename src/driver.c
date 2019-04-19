#include<libusb-1.0/libusb.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdio.h>
char *button_map[]={
  "v","c","d","x","Up","Down","Left","Right","Return","space","s","a"
};

static void error(libusb_context* ctx,char* msg){
  fprintf(stderr,"%s\n",msg);
  libusb_exit(ctx);
  exit(1);
}
static void check(uint8_t* input,int threshold,uint8_t* map){
  if(*input>=threshold){
    *input-=threshold;
    *map=0x01;
  }
}
static void tick_inputs(uint8_t* inputs,uint8_t* curr,uint8_t* last){
  inputs[5]-=15;
  memcpy(last,curr,12);
  memset(curr,0,12);
  check(inputs+5,128,curr);
  check(inputs+5,64,curr+1);
  check(inputs+5,32,curr+2);
  check(inputs+5,16,curr+3);
  check(inputs+6,32,curr+9);
  check(inputs+6,16,curr+8);
  check(inputs+6,4,curr+10);
  check(inputs+6,1,curr+11);
  if(inputs[4]) curr[5]=0x01;
  else curr[4]=0x01;
  if(inputs[3]) curr[7]=0x01;
  else curr[6]=0x01;
}
void process_delta(uint8_t* curr,uint8_t* last){
  for(int a=0;a<12;a++){
    if(curr[a] && !last[a]){
      printf("Keydown %s\n",button_map[a]);
    }
    if(!curr[a] && last[a]){
      printf("Keyup %s\n",button_map[a]);
    }
  }
}

int main(int argc,char** argv){

  // LibUSB setup
  struct libusb_device_descriptor desc;
  libusb_device_handle* dev=NULL;
  libusb_device** list;
  libusb_context* ctx;
  libusb_init(&ctx);
  int len=libusb_get_device_list(ctx,&list);
  for(int a=0;a<len;a++){
    libusb_get_device_descriptor(list[a],&desc);
    if(desc.idVendor==0x79 && desc.idProduct==0x11){
      int e=libusb_open(list[a],&dev);
      if(e==LIBUSB_ERROR_ACCESS) error(ctx,"Root permissions required\n");
    }else free(list[a]);
  }
  if(!dev) error(ctx,"Controller not plugged in");
  int i=0;
  if(libusb_kernel_driver_active(dev,i)){
    if(libusb_detach_kernel_driver(dev,i)) error(ctx,"Could not detach kernel driver");
  }
  if(libusb_claim_interface(dev,i)) error(ctx,"Could not claim interface");

  // Begin data interpretation loop
  unsigned char* data=(unsigned char*)malloc(96);
  int transferred;
  int a=libusb_interrupt_transfer(dev,0x81,data,96,&transferred,200);
  uint8_t curr[12],last[12];
  uint8_t input[8];
  memset(last,0,12);
  memset(curr,0,12);
  free(data);
  printf("SNESmap is online\n");
  while(1){
    a=libusb_interrupt_transfer(dev,0x81,input,8,&transferred,200);
    if(a) break;
    tick_inputs(input,curr,last);
    process_delta(curr,last);
  }
  printf("SNESmap terminated\n");
  libusb_close(dev);
  libusb_exit(ctx);
  return 0;
}
