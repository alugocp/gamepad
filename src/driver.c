#include<libusb-1.0/libusb.h>
#include<stdlib.h>
#include<stdio.h>

static void error(libusb_context* ctx,char* msg){
  fprintf(stderr,"%s\n",msg);
  libusb_exit(ctx);
  exit(1);
}

int main(int argc,char** argv){
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
  printf("Now able to access controller\n");
  unsigned char* data=(unsigned char*)malloc(96);
  int transferred;
  int a=libusb_interrupt_transfer(dev,0x81,data,96,&transferred,200);
  unsigned char input[8];
  printf("%i\n",a);
  free(data);
  while(1){
    a=libusb_interrupt_transfer(dev,0x81,input,8,&transferred,200);
    if(a==LIBUSB_ERROR_NO_DEVICE) break;
    printf("\r%u,%u,%u,%u,%u,%u,%u,%u",input[0],input[1],input[2],input[3],input[4],input[5],input[6],input[7]);
  }
  libusb_close(dev);
  libusb_exit(ctx);
  return 0;
}
