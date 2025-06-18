# Makefile

build:
	docker compose up --build

up:
	docker composer up

down:
	docker compose down

restart:
	make down && make up

code:
	code .

