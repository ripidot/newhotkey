# Makefile

build:
	docker compose build

up:
	docker composer up

down:
	docker compose down

restart:
	make down && make up

code:
	code .

