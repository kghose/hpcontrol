# HP Control

Control Mitsubishi heat pump system via a web based interface.

```
arduino-cli compile \
--library hpcontrol/src/ \
--fqbn esp8266:esp8266:nodemcuv2 \
--build-path hpcontrol/build \
hpcontrol
```

# Design 
## Negotiation

I would like to have a resilient system, where, if one node goes down, the rest 
of the units still work. I'd like to be able to command each unit separately, 
but also command all the units together. I might even want to set schedules.

It looks like we want a system where each node can build a list of what other 
mini-split control nodes exist on the network and synchronize state with them 
whenever some kind of change occurs. 

Each node is responsible for sending commands to its own head.

When a unit is turned on

1. It loads the SSID and password from non-volatile storage and attempts to 
   connect to the router.
1. If the router is down, it keeps periodically retrying.
1. If the SSID/password pair doesn't work, or it can't find the SSID/password 
   pair, it goes into AP mode so we can connect to it directly.  
   1. It creates an AP name with a random suffix to minimize clashes if multiple 
      nodes are in the same situation.
1. Once connected to the router it uses mDNS to announce itself and periodically 
   check for all devices that advertise themselves as `mini-split-controller`.

## Synchronization

1. Settings are time stamped.  
1. When a unit first connects to the network it asks all its peers for their 
   settings. It updates itself to the latest settings if needed.
1. If we use the web interface to change something (including schedule) the 
   serving node
  1. Refreshes the mDNS list
  1. Contacts each node in turn and updates the settings on all of them.
1. If a peer node changes something in the settings of a node, the node commands 
   the mini-split head, but doesn't propagate the changed settings (which would 
result in an endless loop) 


## Upgrade path to CN105 solution

This solution has a simple upgrade path to a wired (CN105) solution. 

1. We replace the IR library with a library to talk to the mini-split head via 
   the CN105
1. We transfer the code to a WeMOS D1 mini-board

We therefore have an abstraction layer above the IR library code where the 
settings are in a driver independent format (fancy way of saying we have a 
settings object and the IR driver reads the settings object)



# Architecture

```
       ┌──────────────────┐                                                
       │ ┌──────────────┐ │   IR     ┌──────┐                              
       │ │ IR TX        │─┼──────────► HEAD │                              
       │ └─────▲────────┘ │          └──────┘                              
       │       │          │                                                
       │       │          │                                                
       │ ┌─────┴────────┐ │                                                
       │ │ DRIVER       │ │                                                
       │ │ Direct call  │ │          ┌──────────────────┐                  
       │ └─────▲────────┘ │          │ ┌──────────────┐ │   IR     ┌──────┐
       │       │          │          │ │ IR TX        │─┼──────────► HEAD │
       │       │          │          │ └─────▲────────┘ │          └──────┘
       │       │          │          │       │          │                  
       │       │          │          │       │          │                  
       │ ┌─────┴────────┐ │          │ ┌─────┴────────┐ │                  
┌──────┼─► CONTROLLER   │ │  WiFi    │ │ DRIVER       │ │                  
│      │ │ CoAP server  │◄┼──────────┼►│ CoAP observer│ │                  
│      │ └──────────────┘ │          │ └──────────────┘ │                  
│      │                  │          │                  │                  
│      ┼────────┬─────────┤          ┼──────────────────┼                  
│      │  STA   │   AP    │          │      STA         │                  
│      └────────┴─────────┘          └──────────────────┘                  
│                                                                          
│                                                                          
│                                                                          
│        ┌─────────┐                                                       
│        │         │                                                       
└───────►┼  USER   │                                                       
         │         │                                                       
         └─────────┘                                                       
```

All devices are ESP8266 development boards with an IR Tx LEDs. One device is 
arbitrarily configured as the "Server" and the others as "Client". 

The Server operates in both AP and STA mode and connects to a router. The 
Clients operate in STA mode and connect to the Server's WiFi.


## References

1. https://www.emqx.com/en/blog/mqtt-vs-coap



# Roadmap

```
Version  Features

V0       List of all heads with ON/OFF buttons that highlight to show what the
         server thinks the head's state is.

V1       Allow setting of temperature and mode (HEAT/COOL) of each head.

V2       Learn and Enable security

V3       Expose controls via proxy

V4       Expose more parameters TBD
 ```


# Design decisions

## Why not use the CN105

Many Mitsubishi mini-split heads have a serial interface plug (CN105) that 
allows control of all head functions. The plug can also supply power. This is 
the interface that Mitsubishi uses for its own WiFi control. The homeowner 
community has detailed instructions on how to use a [WeMos D1 mini board 
(ESP8266) to interface via the CN105 port][D1minicontrol] 

[D1minicontrol]: https://github.com/SwiCago/HeatPump/issues/13#issuecomment-457897457 

(Also see [this thread][findcn105])

[findcn105]: https://www.geekzone.co.nz/forums.asp?forumid=73&topicid=303958


I decided against this method:

1. It involves opening up each head and attaching a home-made device. If ever a 
   company had an excuse not to honor a warranty, it is this.
1. There is a real possibility that a malfunction on my home-made board could 
   damage the head circuitry. (e.g. [see this post][cuttlefish])
1. It might confuse a tech coming to service or fix the head.
1. It will likely confuse the rest of my family: they'll be trying to set the 
   mini-split with the remote and Dad's secret circuit keeps changing things 
from some hidden location and the only way to make it stop is to switch off the 
heat pump breaker and dismantle the mini-split heads. 

While the IR method is inelegant (instead of a dongle tucked away in the head, 
drawing power from it, there's this ugly box nailed to the wall and plugged into 
an outlet) and open-loop (there is no way to get the current state of the head) 
it is non-invasive and far less obscure.

It's much easier to explain to my family that this box on the wall is an 
automatic remote and if you ever think it's wonky, just unplug it.

[cuttlefish]: https://cuttlefishblacknet.wordpress.com/2019/05/31/mitsubishi-heatpump-2/


# Check ESP32 connection via USB

(https://docs.espressif.com/projects/esp-idf/en/v5.3.1/esp32/get-started/establish-serial-connection.html)

List all the serial devices
```
ls /dev/tty*
```

Try `screen` or `cu` to call up the serial terminal on the ESP32 board

```
screen /dev/ttyUSB0 115200
cu -l /dev/ttyUSB0 -s 115200
```

If screen terminates (unhelpfully silently) or `cu` gives the useful error 
message
```
cu: open (/dev/ttyUSB0): Permission denied
cu: /dev/ttyUSB0: Line in use
```

Add current user to `dialout`

```
sudo usermod -a -G dialout $USER
```

# Setup for ESP8266

The modern ESP tooling, [ESP-IDF](https://idf.espressif.com/), which works for
ESP32 [does not support](https://idf.espressif.com/) the ESP8266. We have to use
the [ESP8266_RTOS_SDK](https://github.com/espressif/ESP8266_RTOS_SDK) instead.
This toolchain only works with Python2 which has been deprecated on Ubuntu 24
(and other Linux distributions, I think)

## Installing Python 2 on Ubuntu 24

If we only wanted to install Python 2 it would be easy, but the problem is that
we need to install Python 2 with SSL support because `pip` needs this and we
need `pip`.

After many dead ends I found [these instructions on James Betker's
blog](https://medium.com/@jbetker/how-to-build-python-2-7-with-ssl-b9e4aa26f061)
which applies a custom openssl patch that fell of a lorry somewhere. (I'm not
kidding, I wouldn't be surprised if this patch was a Chinese intelligence
operation to install backdoors in computers).

In any case this got me a Python2 install with SSL, and I added a softlink to
this Python in a special directory that I only add to the path when I use the
ESP tool chain.

## Installing the ESP8266 RTOS SDK

Follow the instructions
[here](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html)


# Build and flash

(https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/linux-macos-setup.html#get-started-linux-macos-first-steps)

```
idf.py menuconfig

idf.py build 

OR

idf.py flash monitor
```

- Needs a flash size of 4 MB
- In menuconfig enable two OTA partitions if you want OTA







