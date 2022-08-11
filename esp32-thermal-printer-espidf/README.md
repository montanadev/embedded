# esp32-thermal-printer-espidf

This printer takes in Markdown and prints out nicely formatted receipts.

```bash
# markdown with body
curl -X POST -d '# h1 test\n## h2 test\n **bold**' <esp-ip-address>/print

# markdown as a file
$ curl -F 'data=@testfile.txt' <esp-ip-address>/upload
```

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
