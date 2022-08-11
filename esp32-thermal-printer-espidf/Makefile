.PHONY: build run monitor test

build:
	pio run -e nodemcu-32s

run:
	pio run -e nodemcu-32s -t upload

monitor:
	pio device monitor

test:
	pio test -e native -vvv