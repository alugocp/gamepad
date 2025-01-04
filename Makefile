all: clean drivers

clean:
	rm -rf out
	mkdir out

drivers: out/snes out/switch

out/snes:
	gcc snes/snes.c -o out/snes -lxdo -lusb-1.0

out/switch:
	echo "Not yet"

install:
	echo "Not yet"