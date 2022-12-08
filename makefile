all: backend frontend

backend: backend.c
	gcc backend.c item.h users_lib.h users_lib.o -o backend

frontend: frontend.c
	gcc frontend.c item.h -o frontend

clean: 
	rm backend frontend
