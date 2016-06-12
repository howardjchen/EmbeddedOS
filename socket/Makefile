CC=gcc
all:
	$(CC) server.c -o server -lpthread -I /usr/include/mysql -L /usr/lib/mysql -lmysqlclient
	$(CC) client.c -o client
clean:
	rm -rf server client
