# esp32-thermal-printer-espidf

This printer takes in Markdown and prints out nicely formatted receipts.

```bash
# markdown with body
curl -X POST -d '# h1 test\n## h2 test\n **bold**' <esp-ip-address>/print

# markdown as a file
$ curl -F 'data=@testfile.txt' <esp-ip-address>/print
```

## Hardware & Schematics

TODO

## Development

Requires PlatformIO to be installed

```bash
# run the unit tests
$ make test

# compile the firmware
$ make build

# flash and monitor for logs
$ make run monitor
```

## Secrets

A `secrets.h` file with the following should be placed in `src/` 

```C++
const char *ssid_name = "ssid";
const char *ssid_password = "password";
const char *firmware_url = "https://path-to-firmware.bin";
```

`firmware_url` allows an optional path to an OTA update. Trigger the update process with a `POST` to `/ota`.

## API

|path|example|
|-|-|
|/print|`curl -X POST -d '# h1 test\n## h2 test\n **bold**' <esp-ip-address>/print`
|/ping|`curl <esp-ip-address>/ping`
|/ota|`curl -X POST <esp-ip-address>/ota`