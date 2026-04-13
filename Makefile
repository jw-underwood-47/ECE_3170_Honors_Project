CC = gcc
sim = sim.c
sim_zero = sim_zero.c
sim_out = -o sim.exe
sim_zero_out = -o sim_zero.exe
FLAGS = -g -o3
default:
	$(CC) $(FLAGS)  $(sim) $(sim_out)
	$(CC) $(FLAGS)  $(sim_zero) $(sim_zero_out)
