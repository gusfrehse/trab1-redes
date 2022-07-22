#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "ConexaoRawSocket.h"

int main() {
  char curr_byte;

  int s = ConexaoRawSocket("lo");
  while (read(s, &curr_byte, 1) == 1) {
    if (curr_byte == 0b01111110) {
      printf("Recebi byte de identificação!\n");
    }

    printf("recebi: 0x%x\n", curr_byte);
  }
  close(s);


  printf("sucesso\n");

}
