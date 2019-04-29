default:
	make build
build:
	g++ sdb.cpp -o sdb -W
debug:
	g++ -g sdb.cpp -o sdb
clear:
	rm sdb