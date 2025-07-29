#include "contiki.h"



#include "contiki-net.h"
#include "uip.h"
#include "uip-ds6.h"
#include "uip-udp-packet.h"
#include "dev/leds.h"
#include "sys/etimer.h"
#include <stdio.h>
#include <string.h>


#define UDP_PORT 8765
#define SEND_INTERVAL (30 * CLOCK_SECOND)

static struct uip_udp_conn *udp_conn;

PROCESS(saamrp_node_process, "SAAMRP Node Process");
AUTOSTART_PROCESSES(&saamrp_node_process);

PROCESS_THREAD(saamrp_node_process, ev, data)
{
  static struct etimer periodic_timer;
  static uint16_t residual_energy = 100; // Simulated energy level

  PROCESS_BEGIN();

  uip_ipaddr_t sink_ipaddr;
  uip_ip6addr(&sink_ipaddr, 0xfe80,0,0,0,0x0212,0x7401,1,1); // Set sink IP manually

  udp_conn = udp_new(&sink_ipaddr, UIP_HTONS(UDP_PORT), NULL);
  udp_bind(udp_conn, UIP_HTONS(UDP_PORT));

  etimer_set(&periodic_timer, SEND_INTERVAL);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    char buf[50];
    residual_energy -= 1; // simulate drain
    sprintf(buf, "Energy:%u", residual_energy);

    uip_udp_packet_send(udp_conn, buf, strlen(buf));
    printf("Node: Sent %s to sink\n", buf);

    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}

