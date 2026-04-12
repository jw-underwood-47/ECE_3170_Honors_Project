CC = gcc
important = sim.c
output = -o sim.exe
FLAGS = -g -o3
default:
	$(CC) $(FLAGS)  $(important) $(output)
