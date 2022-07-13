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
  int s = ConexaoRawSocket("lo");
  char* buf = "1234567890123";

  write(s, buf, strlen(buf) + 1);
  close(s);
  printf("mandei\n");
}
