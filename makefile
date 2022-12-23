all: backend frontend

backend: backend.c
	gcc backend.c item.h users_lib.h users_lib.o -o backend

frontend: frontend.c
	gcc frontend.c item.h users_lib.h users_lib.o -o frontend

clean: 
	rm backend frontend
