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
  char buf2[11];

  int s = ConexaoRawSocket("lo");
  read(s, buf2, 10);
  close(s);

  buf2[10] = '\0';
  printf("recebi: '%s'\n", buf2);

  printf("sucesso\n");

}
