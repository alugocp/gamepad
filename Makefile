all: clean driver

clean:
	rm -f driver

driver:
	gcc driver.c -o driver -lxdo -lusb-1.0
