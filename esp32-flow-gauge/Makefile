.PHONY: build run monitor test clean

build:
	pio run -e nodemcu-32s

run:
	pio run -e nodemcu-32s -t upload

monitor:
	pio device monitor

test:
	pio test -e native -vvv

clean:
	pio run --target clean