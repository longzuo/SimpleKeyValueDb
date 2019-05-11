CLIENTDIR=./client
SERVERDIR=./server
default:
	make all
sdbclient:
	make -C client
sdbserver:
	make -C server
all:
	make sdbclient
	make sdbserver
clean:
	rm ./client/sdbclient ./server/sdbserver