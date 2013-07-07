all:
	gcc pt100.c -o pt100 -I/usr/local/include -L/usr/local/lib -lwiringPi
	
install:
	install -m 755 pt100 /usr/bin/pt100
	install -m 755 pt100scan /usr/bin/pt100scan

uninstall:
	rm /usr/bin/pt100*
	
clean:
	rm pt100

