/* See LICENSE file for copyright and license details. */
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <limits.h>
#include <net/if.h>
#include <netdb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util.h"

static char active_iface[IFNAMSIZ] = "";

void
update_active_interface(void)
{
	FILE *fp;
	char iface[IFNAMSIZ], dest[9];

	fp = fopen("/proc/net/route", "r");
	if (!fp)
		return;

	fgets((char[256]){}, 256, fp); // Skip header
	while (fscanf(fp, "%s %8s", iface, dest) == 2) {
		if (strcmp(dest, "00000000") == 0) {
			strncpy(active_iface, iface, IFNAMSIZ);
			active_iface[IFNAMSIZ - 1] = '\0';
			break;
		}
	}
	fclose(fp);
}

const char *
ipv4_active(void)
{
	static char ip[INET_ADDRSTRLEN];
	struct ifaddrs *ifaddr, *ifa;

	update_active_interface();
	if (active_iface[0] == '\0')
		return NULL;

	if (getifaddrs(&ifaddr) == -1)
		return NULL;

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL)
			continue;
		if (strcmp(ifa->ifa_name, active_iface) != 0)
			continue;
		if (ifa->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in *sa = (struct sockaddr_in *)ifa->ifa_addr;
			inet_ntop(AF_INET, &(sa->sin_addr), ip, INET_ADDRSTRLEN);
			freeifaddrs(ifaddr);
			return ip;
		}
	}

	freeifaddrs(ifaddr);
	return NULL;
}

const char *
netspeed_active_rx(void)
{
  uintmax_t oldrxbytes;
  static uintmax_t rxbytes;
  extern const unsigned int interval;
  char path[PATH_MAX];

  if (active_iface[0] == '\0')
    return "0";

  oldrxbytes = rxbytes;

  if (esnprintf(path, sizeof(path),
                "/sys/class/net/%s/statistics/rx_bytes",
                active_iface) < 0) {
    return NULL;
  }
  if (pscanf(path, "%ju", &rxbytes) != 1) {
    return NULL;
  }
  if (oldrxbytes == 0) {
    return NULL;
  }

  return fmt_human((rxbytes - oldrxbytes) * 1000 / interval,
                   1024);
}

const char *
netspeed_active_tx(void)
{
  uintmax_t oldtxbytes;
  static uintmax_t txbytes;
  extern const unsigned int interval;
  char path[PATH_MAX];

  if (active_iface[0] == '\0')
    return "0";

  oldtxbytes = txbytes;

  if (esnprintf(path, sizeof(path),
                "/sys/class/net/%s/statistics/tx_bytes",
                active_iface) < 0) {
    return NULL;
  }
  if (pscanf(path, "%ju", &txbytes) != 1) {
    return NULL;
  }
  if (oldtxbytes == 0) {
    return NULL;
  }

  return fmt_human((txbytes - oldtxbytes) * 1000 / interval,
                   1024);
}

