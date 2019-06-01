# Watchdog library for Mongoose OS

## Overview
This library implements a simple watchdog timer that checks for network connectivity by send an ICMP echo request to a given host or default gateway address. If the remote host failed to reply for a given number of retries the device will restart.

## Background
I have come to notice that sometimes my ESP8266 looses it's network connection but would not reconnect to the WIFI accesspoint. Instead of creating a timer in my program to check for connectivity I decided to try and create a Mongoose OS library so that it would be reusable and maybe help someone else that is having this problem.

## Configuration
This library adds a `watchdog` section. Containing an `interval` and a `net` section.

### Default wachtdog configuration
```javascript
"watchdog": {
    "enable": false,
    "interval": 60000,
    "net": {
        "host": "",
        "retry": 5
    }
}
```

By default the library is not enabled. Default configuration had a check `interval` of 60 seconds, `retry` count of 5 and an empty `host` configuration.

If you leave `host` empty it will use the networks default gateway as the check target.

## Usage
Because this project is uploaded with name `mgos_watchdog` you need to add the `name` attribute to your `origin` configuration item in `mgos.yml` or else the compiler will not find the `mgos_watchdog...` functions.

```yaml
libs:
  - origin: https://github.com/wowthur/mgos_watchdog
    name: watchdog
```

