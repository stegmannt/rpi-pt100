all:
	gcc pt100.c -o pt100 -I/usr/local/include -L/usr/local/lib -lwiringPi

clean:
	rm pt100

