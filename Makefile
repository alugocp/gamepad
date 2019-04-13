all: clean driver
clean:
	rm -f bin/*

driver:
	gcc src/driver.c -lusb-1.0 -o bin/driver
