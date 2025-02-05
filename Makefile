OBJS = main.o generation_change.o agent.o cal_fdtd.o \
	allocate_memory.o  initialize_surface_impedance.o  update_E.o \
	current_source.o  update_H.o output.o  update_H_PML.o \
	initialize_conductivity.o  update_D.o initialize_pml.o \
	update_D_PML.o suffix.o input.o 

.PHONY: all clean

all: main


main: $(OBJS)
	g++ -o $@  $(OBJS) -fopenmp -O3

%.o: %.cpp %.cpp GA.h agent.h  fdtd2d.h
	g++ -c $< -Wall -fopenmp -O3 -I.

clean:
	rm -rf *.o main

