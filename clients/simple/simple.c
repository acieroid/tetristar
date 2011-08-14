#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <enet/enet.h>
#include <signal.h>

#ifdef ENET_VERSION
  /* See server/network.h for an explanation of this */
  #define HAS_RECENT_ENET
#endif

void sigchld_handler(int signum)
{
  printf("Exiting client\n");
  exit(0);
}

int main()
{
  ENetHost *client;
  ENetAddress address;
  ENetEvent event;
  ENetPeer *peer;
  ENetPacket *packet;
  int connected;
  char string[1024];
  pid_t pid;

  signal(SIGCHLD, sigchld_handler);

  /* create client */
  client = enet_host_create(NULL, 1,
#if HAS_RECENT_ENET
                            2,
#endif
                            0, 0);
  if (client == NULL) {
    fprintf(stderr, "Can't create client\n");
    exit(1);
  }

  /* connect client */
  enet_address_set_host(&address, "localhost");
  address.port = 12345;

  peer = enet_host_connect(client, &address, 2
#if HAS_RECENT_ENET
                           , 0
#endif
                          );

  if (peer == NULL) {
    fprintf(stderr, "Can't create peer\n");
    exit(1);
  }

  if (enet_host_service(client, &event, 5000) > 0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    printf("Connected\n");
    connected = 1;
  }
  else {
    fprintf(stderr, "Connection failed\n");
    exit(1);
  }

  /* fork */
  pid = fork();
  if (pid == 0) {
    /* read and send packets */
    while (1) {
      if (fgets(string, 1024, stdin) == NULL)
        /* disconnect at EOF */
        break;

      string[strlen(string)-1] = '\0'; /* drop the \n */
      packet = enet_packet_create(string, strlen(string) + 1,
                                  ENET_PACKET_FLAG_RELIABLE);
      enet_peer_send(peer, 0, packet);
      enet_host_flush(client);
    }
    /* disconnect client */
    enet_peer_disconnect(peer, 0);
    enet_host_destroy(client);
  }
  else {
    while (1) {
      enet_host_service(client, &event, 1000);
      if (event.type == ENET_EVENT_TYPE_RECEIVE)
        printf("> %s\n", event.packet->data);
      if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
        printf("Exiting client\n");
        exit(0);
        break;
      }
    }
  }
  return 0;
}
