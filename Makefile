CC=gcc
GLOBAL_FOLDER=global_heavy/
STRUCT_FOLDER=struct_based/
SIM=sim
SIM_ZERO=sim_zero
STRUCT_BASED=combined_with_struct
FLAGS = -g -o3
default:
	$(CC) $(FLAGS) $(GLOBAL_FOLDER)$(SIM).c -o $(SIM).exe
	$(CC) $(FLAGS) $(GLOBAL_FOLDER)$(SIM_ZERO).c -o $(SIM_ZERO).exe
struct:
	$(CC) $(FLAGS) $(STRUCT_FOLDER)$(STRUCT_BASED).c -o $(STRUCT_BASED).exe
