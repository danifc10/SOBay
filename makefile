all: backend frontend

backend: backend.c
	gcc backend.c item.h users_lib.h -o backend

frontend: frontend.c
	gcc fontend.c item.h -o frontend

clean: 
	rm backend frontend