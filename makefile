all:
	gcc backend.c util.h item.h item.c users_lib.h users_lib.o -o backend -pthread
	gcc frontend.c item.h item.c -o frontend

clean:
	rm backend
	rm frontend