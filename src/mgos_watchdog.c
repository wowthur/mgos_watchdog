/**
 * @file    mgos_watchdog.c
 * @author  Wouter Langenberg (wowthur@wowthur.com)
 * @brief   Mongoose OS Watchdog library
 * @version 0.1
 * @date    2019-05-30
 * 
 * @copyright Copyright (c) 2019 DotAsterisk.org
 * 
 */
#include "mgos_watchdog.h"

#include "mgos_wifi.h"
#include "lwip/app/ping.h"

static char mgos_watchdog_host[16];
static struct ping_option mgos_watchdog_ping_option;
static int mgos_watchdog_retrycount;

/**
 * @brief Callback for NET timer
 */
static void mgos_watchdog_net_timer_cb(void *arg)
{
    LOG(LL_INFO, ("PING: %s", mgos_watchdog_host));

    // mgos_watchdog_ping_option.count = mgos_sys_config_get_watchdog_net_retry();
    ping_start(&mgos_watchdog_ping_option);

    (void) arg;
}

/**
 * @brief Callback for wifi events
 * 
 * @param ev  Event
 * @param evd Event data
 * @param arg Event arguments
 */
static void mgos_watchdog_ev_cb(int ev, void *evd, void *arg)
{
    switch (ev) {
        case MGOS_NET_EV_IP_ACQUIRED:
            LOG(LL_DEBUG, ("IP Aquired"));
            mgos_watchdog_init_net_timer(mgos_sys_config_get_watchdog_interval());
            break;
    }
    (void) arg;
    (void) evd;
    (void) arg;
}

/**
 * @brief Callback for PING received data
 * 
 * @param arg   Event arguments
 * @param pdata Event data
 */
static void ICACHE_FLASH_ATTR mgos_watchdog_ping_recv_cb(void *arg, void *pdata)
{
    struct ping_resp *pingrsp = (struct ping_resp *)pdata;

    if (pingrsp->bytes > 0)
    {
        LOG(LL_DEBUG, ("Reply from: %s: bytes=%d time=%dms", mgos_watchdog_host, pingrsp->bytes, pingrsp->resp_time));
        // mgos_watchdog_ping_option.count = 0;
    }
    else
    {
        LOG(LL_WARN, ("Request timed out"));
    }
    (void) arg;
}

void mgos_watchdog_reset(void) {
    LOG(LL_INFO, ("Restarting device"));
    mgos_fs_gc();
    mgos_system_restart_after(100);
}

/**
 * @brief Callback after PING request completed
 * 
 * @param arg   Event arguments
 * @param pdata Event data
 */
static void ICACHE_FLASH_ATTR mgos_watchdog_ping_sent_cb (void* arg, void *pdata)
{
    struct ping_resp *pingrsp = (struct ping_resp *)pdata;

    LOG(LL_DEBUG, ("Ping statistics for: %s", mgos_watchdog_host));
    LOG(LL_DEBUG, ("Packets: Sent = %d, Received = %d, Lost = %d (%f%%loss)", 
        pingrsp->total_count, 
        pingrsp->total_count - pingrsp->timeout_count, 
        pingrsp->timeout_count,
        (double)pingrsp->timeout_count/pingrsp->total_count*100));
    
    // All requests failed
    if (pingrsp->total_count == pingrsp->timeout_count) {
        LOG(LL_INFO, ("Retry %d/%d", mgos_watchdog_retrycount, mgos_sys_config_get_watchdog_net_retry()));

        if (mgos_watchdog_retrycount == mgos_sys_config_get_watchdog_net_retry()) {
            mgos_watchdog_reset();
        }

        mgos_watchdog_retrycount++;
    }
    else {
        mgos_watchdog_retrycount = 0;
    }

    (void) arg;    
}

/**
 * @brief Initialize NET watchdog
 */
void mgos_watchdog_init_net()
{
    mgos_event_add_group_handler(MGOS_EVENT_GRP_NET, mgos_watchdog_ev_cb, NULL);
}

/**
 * @brief Initialize NET timer
 * 
 * @param interval Interval at which to check NET connectivity
 */
void mgos_watchdog_init_net_timer(int interval)
{
    memset(mgos_watchdog_host, 0, sizeof(mgos_watchdog_host));
    
    if (mgos_sys_config_get_watchdog_net_host() == NULL) {
        struct mgos_net_ip_info ip_info;        
        memset(&ip_info, 0, sizeof(ip_info));
        char gw_ip[16];
        memset(gw_ip, 0, sizeof(gw_ip));

        if (mgos_net_get_ip_info(MGOS_NET_IF_TYPE_WIFI, MGOS_NET_IF_WIFI_STA,
                                 &ip_info)) {
            mgos_net_ip_to_str(&ip_info.gw, gw_ip);
            strncpy(mgos_watchdog_host, gw_ip, sizeof(mgos_watchdog_host));
        }
    }
    else {
        strncpy(mgos_watchdog_host, mgos_sys_config_get_watchdog_net_host(), sizeof(mgos_watchdog_host));
    }

    struct sockaddr_in sin;

    if(mgos_net_str_to_ip(mgos_watchdog_host, &sin)) {
        LOG(LL_DEBUG, ("ip: %d", sin.sin_addr.s_addr));

        mgos_watchdog_ping_option.ip = sin.sin_addr.s_addr;
        mgos_watchdog_ping_option.count = 1;
        mgos_watchdog_ping_option.coarse_time = 1;
        ping_regist_recv(&mgos_watchdog_ping_option, mgos_watchdog_ping_recv_cb);
        ping_regist_sent(&mgos_watchdog_ping_option, mgos_watchdog_ping_sent_cb);
    }

    LOG(LL_DEBUG, ("Starting net timer with interval: %d, host: %s", interval, mgos_watchdog_host));
    mgos_set_timer(interval, MGOS_TIMER_REPEAT, mgos_watchdog_net_timer_cb, NULL);
}

/**
 * @brief Entrypoint of watchdog library 
 * 
 * @return true  If initialization succeeded
 * @return false If initialization failed
 */
bool mgos_watchdog_init(void)
{
    if (mgos_sys_config_get_watchdog_enable())
    {
        LOG(LL_INFO, ("Watchdog reporting"));

        mgos_watchdog_init_net();
    }

    return true;
}