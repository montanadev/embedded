.PHONY: build upload monitor test clean menuconfig check 

build:
	pio run -e nodemcu-32s

upload:
	pio run -e nodemcu-32s -t upload

monitor:
	pio device monitor

test:
	pio test -e native -vvv

clean:
	pio run --target clean

menuconfig:
	pio run -t menuconfig

check:
	pio check --skip-packages --fail-on-defect low

erase: 
	pio run --target erase -e nodemcu-32s
