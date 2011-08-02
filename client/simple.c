#include <stdio.h>
#include <string.h>
#include <enet/enet.h>

int main()
{
  ENetHost *client;
  ENetAddress address;
  ENetEvent event;
  ENetPeer *peer;
  ENetPacket *packet;
  int connected;
  char string[1024];

  /* create client */
  client = enet_host_create(NULL,
                            1,
                            57600 / 8,
                            14400 / 8);
  if (client == NULL) {
    fprintf(stderr, "Can't create client\n");
    exit(1);
  }

  /* connect client */
  enet_address_set_host(&address, "localhost");
  address.port = 12345;

  peer = enet_host_connect(client, &address, 2);

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

  while (enet_host_service(client, &event, 1000) > 0) {
    if (event.type == ENET_EVENT_TYPE_RECEIVE)
      printf("> %s\n", event.packet->data);
  }

  /* disconnect client */
  enet_peer_disconnect(peer, 0);
  while (connected && enet_host_service(client, &event, 3000) > 0) {
      switch (event.type) {
      case ENET_EVENT_TYPE_RECEIVE:
        enet_packet_destroy(event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("Disconnected\n");
        connected = 0;
        break;
      default:
        break;
      }
  }

  if (connected)
    /* force the deconnection */
    enet_peer_reset(peer);

  enet_host_destroy(client);
  return 0;
}
    
