/**
 * igmp-querier.c - IGMP Querier for Multicast DNS
 *
 * Copyright (c) 2014, Daniel Lorch <dlorch@gmail.com>
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 * Many thanks go to Marc Cullen <mcbugs000@gmail.com>
 *   A detailled analysis of the mDNS issue can be found here:
 *   http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=736641
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <libnet.h>
#include <string.h>
#include <arpa/inet.h>

#define MCAST_ALL_HOSTS "224.0.0.1"
#define MCAST_MDNS      "224.0.0.251"
#define LINK_INTERFACE  "eth0"
#define INTERVAL_SECS   60
#define ERRBUFFSIZE     256

static struct libnet_ethernet_hdr etherhdr;
static struct libnet_ip_hdr iphdr;
static struct libnet_igmp_hdr igmphdr;
static struct libnet_link_int *linkint = NULL;
static u_int32_t igmp_packetlen = 0;
static u_int8_t *pkt = NULL;
static char errbuf[ERRBUFFSIZE];

int init(void) {
    /* Ethernet header */
    etherhdr.ether_type = ETHERTYPE_IP;      /* Ethernet type IP */
    memset(etherhdr.ether_shost, 0, 6);      /* Ethernet source address */
    memset(etherhdr.ether_dhost, 0xff, 6);   /* Ethernet destination address */

    /* IP header */
    memset(&iphdr.ip_src.s_addr, 0, 4);               /* IP source address 0.0.0.0
                                                       * (pretend to be proxy to
                                                       * avoid being elected as master) */
    inet_aton(MCAST_ALL_HOSTS, &iphdr.ip_dst);        /* IP destination address */
    iphdr.ip_tos = 0;                                 /* IP type of services */
    iphdr.ip_id = (u_int16_t)libnet_get_prand(PRu16); /* IP ID */
    iphdr.ip_p = IPPROTO_IGMP;                        /* IP protocol IGMP */
    iphdr.ip_off = 0;                                 /* IP fragmentation offset */
    iphdr.ip_ttl = 1;                                 /* IP TTL - set to 1 purposely */

    /* IGMP header */
    igmphdr.igmp_type = IGMP_MEMBERSHIP_QUERY;   /* IGMP type */
    igmphdr.igmp_code = 0;                       /* IGMP code */
    inet_aton(MCAST_MDNS, &igmphdr.igmp_group);  /* IGMP group address */

    /* Create packet */
    linkint = libnet_open_link_interface(LINK_INTERFACE, errbuf);

    if (linkint == NULL) {
         return -1;
    }

    igmp_packetlen = LIBNET_ETH_H + LIBNET_IP_H + LIBNET_IGMP_H;

    if (libnet_init_packet(igmp_packetlen, &pkt) == -1) {
        return -1;
    } 

    libnet_build_ethernet(etherhdr.ether_dhost, etherhdr.ether_shost,
        ETHERTYPE_IP, NULL, 0, pkt);

    libnet_build_ip(LIBNET_IGMP_H, iphdr.ip_tos, iphdr.ip_id, iphdr.ip_off,
        iphdr.ip_ttl, iphdr.ip_p, iphdr.ip_src.s_addr, iphdr.ip_dst.s_addr,
        NULL, 0, pkt + LIBNET_ETH_H);

    libnet_build_igmp(igmphdr.igmp_type, igmphdr.igmp_code,
        igmphdr.igmp_group.s_addr, NULL, 0, 
        pkt + LIBNET_ETH_H + LIBNET_IP_H);

    libnet_do_checksum(pkt + LIBNET_ETH_H, IPPROTO_IP, LIBNET_IP_H);

    libnet_do_checksum(pkt + LIBNET_ETH_H, IPPROTO_IGMP, LIBNET_IGMP_H);
}

int cleanup(void) {
    libnet_destroy_packet(&pkt);
    libnet_close_link_interface(linkint);
}

int do_igmp_query(void) {
    int n = 0;

    n = libnet_write_link_layer(linkint, LINK_INTERFACE, pkt, igmp_packetlen);

    if (n != igmp_packetlen) {
      /* incomplete packet injection */
    }

    return n;
}

int main(void) {
    pid_t pid = 0, sid = 0;

    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: fork failed.\n");
        return -1;
    }
  
    if (pid > 0) {
        return 0; /* exit parent */
    } 

    umask(0);

    sid = setsid();
    if (sid < 0) {
        fprintf(stderr, "Error: setsid failed.\n");
        return -1;
    }

    chdir("/");

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    init();

    /* main loop */
    while (1) {
        do_igmp_query();
        sleep(INTERVAL_SECS);
    }

    cleanup();

    return 0;
}
