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
#define FEEDBACK_INTERVAL (60 * CLOCK_SECOND)

static struct uip_udp_conn *udp_conn;

PROCESS(saamrp_sink_process, "SAAMRP Sink Process");
AUTOSTART_PROCESSES(&saamrp_sink_process);

PROCESS_THREAD(saamrp_sink_process, ev, data)
{
  static struct etimer feedback_timer;
  static char last_data[100];

  PROCESS_BEGIN();

  udp_conn = udp_new(NULL, UIP_HTONS(UDP_PORT), NULL);
  udp_bind(udp_conn, UIP_HTONS(UDP_PORT));

  etimer_set(&feedback_timer, FEEDBACK_INTERVAL);

  while (1) {
    PROCESS_YIELD();

    if (ev == tcpip_event && uip_newdata()) {
      char *received = (char *)uip_appdata;
      int len = uip_datalen();
      memcpy(last_data, received, len);
      last_data[len] = '\0';

      printf("Sink: Received: %s\n", last_data);
    }

    if (etimer_expired(&feedback_timer)) {
      printf("Sink: Sending feedback or route update based on: %s\n", last_data);
      // You can extend this with actual adaptive logic later
      etimer_reset(&feedback_timer);
    }
  }

  PROCESS_END();
}

