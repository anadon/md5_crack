all:
	g++ -Ofast md5_cracker.cpp -lssl -lcrypto -o cracker

clean:
	rm -f cracker
