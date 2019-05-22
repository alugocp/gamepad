all: clean driver

clean:
	rm -f driver

driver:
	gcc src/driver.c -o driver -lxdo -lusb-1.0
