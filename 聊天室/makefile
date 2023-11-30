# OBJECTS=ThreadPool.c DoubleLinkList.c Queue.c StdThread.c main.c
# threadPool:${OBJECTS}
# 	gcc ${OBJECTS} -o ThreadPool





OOBJECTS_SERVER=StdTcp.c testServer.c StdThread.c DoubleLinkList.c Queue.c ThreadPool.c StdSqlite.c StdFile.c
server:$(OOBJECTS_SERVER)
	gcc $(OOBJECTS_SERVER) -o server -lsqlite3

OOBJECTS_CLIENT=StdTcp.c testClient.c StdThread.c DoubleLinkList.c StdSqlite.c StdFile.c
client:$(OOBJECTS_CLIENT)
	gcc $(OOBJECTS_CLIENT) -o client -lsqlite3

# OOBJECTS_cc=StdTcp.c test.c StdThread.c DoubleLinkList.c StdSqlite.c 
# cc:$(OOBJECTS_cc)
# 	gcc $(OOBJECTS_cc) -o cc -lsqlite3
.PHONY:clean
clean:
	rm -f server client




