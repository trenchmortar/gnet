/* SDR Example
 * Copyright (C) 2000  David Helder
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <glib.h>
#include <gnet.h>

#define MAXLINE 4096



int
main(int argc, char** argv)
{
  gchar buf[MAXLINE + 1];
  gint n;

  struct sap_packet {
    guint32 sap_header;
    guint32 sap_src;
    char sap_data[1];
  } * sapptr;

  GMcastSocket* ms;
  GInetAddr* ia;
  gint rv;

  gnet_init ();

  /* Create a multicast socket */
  ms = gnet_mcast_socket_new_with_port (9875);
  g_assert(ms != NULL);

  /* Make sure it derives properly from GUdpSocket */
  gnet_udp_socket_ref ((GUdpSocket *) ms);
  gnet_udp_socket_unref ((GUdpSocket *) ms);

  /* Let's check this as well then, just for fun */
  gnet_mcast_socket_ref (ms);
  gnet_mcast_socket_unref (ms);

  /* Get address of our group */
  ia = gnet_inetaddr_new ("sap.mcast.net", 9875);
  g_assert(ia != NULL);

  /* Join the group */
  rv = gnet_mcast_socket_join_group (ms, ia);
  g_assert(rv == 0);

  printf("Joined %s:%d...\n", gnet_inetaddr_get_name(ia), gnet_inetaddr_get_port(ia));

  /* Print some information about our multicast socket */
  g_print ("My addresss: %s\n", gnet_inetaddr_get_host_name());
  g_print ("Loopback: %d\n", gnet_mcast_socket_is_loopback (ms));
  g_print ("Mcast TTL: %d\n", gnet_mcast_socket_get_ttl(ms));

  /* Turn off loopback */
  rv = gnet_mcast_socket_set_loopback (ms, 0);
  g_assert (rv == 0);

  for(;;)
    {
      g_print ("Waiting for packet...\n");

      /* Receive packet */
      n = gnet_mcast_socket_receive (ms, buf, MAXLINE, NULL);
      buf[n] = 0;

      sapptr = (struct sap_packet*) buf;

      n -= 2 * sizeof(guint32);
      if (n <= 0)
	{
	  g_warning ("SAP packet too small");
	  continue;
	}

      g_print ("%s\n", sapptr->sap_data);

    }
}
