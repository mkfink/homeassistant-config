# Overview
This repo mainly consists of my personal Home Assistant and ESPHome configurations, but below I also want to document some of the aspects of my setup and workflow that aren't captured in the config files alone. I also have some devices running Tasmota (which I've used for years, but have mostly switched to ESPHome now) and I manage them with this utility that I wrote with some friends: https://github.com/i3detroit/Automatic-IoT-module-programming. My personal config there is in `/config/mike.json`.

## Hardware
* Raspberry Pi 4 8GB
* 500GB Samsung EVO 860 SSD
* ELUTENG USB 3.0 SATA Adapter
* Argon40 Fan Hat
* Noctua NF-A4x10 5V Fan to replace the fan on the Argon40 hat when it gets loud

## Software
* Home Assistant OS installed on a Raspberry Pi 4 8GB w/ SSD
  * https://community.home-assistant.io/t/installing-home-assistant-on-a-rpi-4b-8gb-with-ssd-boot/230948
  * https://www.home-assistant.io/installation/raspberrypi
* Home Assistant Add-ons installed via Supervisor Add-on Store:
  * ESPHome - adds GUI for configuring and flashign devices from right in Home Assistant
  * File editor - allows editing any config files in Home Assistant UI
  * SSH & Web Terminal (Community Add-on) - Can be useful to have a terminal right in the UI
  * Mosquitto broker - For any MQTT devices since I still have a few devices running Tasmota instead of ESPHome
  * Samba share - Easy access to the file system from Windows/VS Code
  * InfluxDB - DB for logging
  * Grafana - Data visualization
  * ArgonOne Active Cooling - Manages the fan hat
* VS Code installed on PC
  * Home Assistant Config Helper add-on
  * ESPHome add-on

## Home Assitant Add-on Config
Here are notable changes to add-on settings. For the rest, the default settings seem to generally suffice.
### ESPHome
* `leave_front_door_open: true` - for VS Code integration
* Enable port 6052
### SSH & Web Terminal
I like to have a specific bash/tmux environment set up. Each time this add-on is started most of the directory structure including the home directory is reset. `/config` and `/data` are persistent, so we can use `/data` to store things like `.bashrc` and `.tmux.conf` and a start-up script to properly apply them.
* Put your ssh key in `/data/.ssh`
* Clone .dotfiles repo into /data
<br>
`cd /data`
<br>
`git clone -b home-assistant-os https://github.com/mkfink/.dotfiles.git`

* Add-on config yaml:
```
ssh:
  username: hassio
  password: ''
  authorized_keys:
    - your pubkey here
  sftp: false
  compatibility_mode: false
  allow_agent_forwarding: false
  allow_remote_port_forwarding: false
  allow_tcp_forwarding: false
zsh: false
share_sessions: false
packages: []
init_commands:
  - /data/.dotfiles/setup.sh
```
### Mosquitto Broker
I use this with some Tasmota devices, some of which are set up with the default MQTT user/pass and some no user/pass. This presents some problems for the default configuration of this add-on which are (partly) addressed in its documentation. So the important part of the add-on config yaml:
```
logins:
  - username: DVES_USER
    password: DVES_PASS
```
Then we must create two files:
`/share/mosquitto/acl.conf` with contents `acl_file /share/mosquitto/accesscontrollist`
`/share/mosquitto/accesscontrollist` with contents:
```
topic readwrite #
user DVES_USER
topic readwrite #
```
In the latter file, line 1 enables read/write access to anonymous users. Lines 2 and 3 give read/write access to the default Tasmota MQTT username. In my experience, simply enabling anonymous users with the first like causes the broker to reject any named users. At the moment this is not addressed in the add-on documentation.

## VS Code
* The Home Assistant add-on requires a long-lived access token (from your user's page in Home Assistant) and the URL of your Home Assistant install (i.e. `http://hassio.local:8123`)
* The ESPHome add-on needs the dashboard URI (i.e. `http://hassio.local:6052`) and change the validator to `dashboard`
* The ESPHome add-on stores your device configs inside the Home Assistant `/config` directory which means by default, VS Code will try to apply the `home-assistant` language when editing them which. This prevents using the ESPHome validator, and there are also minor syntax incompatibilities. So we need to specify that all .yaml files in `/config/esphome` use the `esphome` language. So in VS Code, Settings > Text Editor > Files > Associations I add the following two associations:
  * item: `**/esphome/*.yaml` association: `esphome`
  * item: `**/esphome/**/*.yaml` association `esphome`
