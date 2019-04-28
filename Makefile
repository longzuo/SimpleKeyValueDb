default:
	make build
build:
	g++ sdb.cpp -o sdb
debug:
	g++ -g sdb.cpp -o sdb
clear:
	rm sdb