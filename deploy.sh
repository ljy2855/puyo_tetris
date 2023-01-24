#!/bin/sh

make clean
make
docker-compose up --build -d --remove-orphans