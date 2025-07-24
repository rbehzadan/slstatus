/* slstatus configuration */
/* See LICENSE file for copyright and license details. */

/* interval between updates (in ms) */
const unsigned int interval = 1000;

/* text to show if no value can be retrieved */
static const char unknown_str[] = "n/a";

/* maximum output string length */
#define MAXLEN 2048

/* output formats */
static const struct arg args[] = {
	/* function, format, argument */
  { cpu_perc, "%2s%% ", ""},
  { load_avg, "%5s | ", ""},
  { ram_perc, "%s%% | ", ""},
  { disk_free, "%s |", "/" },
  { netspeed_active_tx, "%6s↑ ", ""},
  { netspeed_active_rx, "%6s↓ | ", ""},
  { ipv4_active, "%s | ", ""},
  { battery_state, "%s ", "BAT0"},
  { battery_perc, "%2s%% | ", "BAT0"},
  { datetime, "%s", "%a %b%d %H:%M:%S"},
};
