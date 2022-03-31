edit : os_core.o    ucos_ii.o
		cc -o edit os_core.o    ucos_ii.o  
		./edit
		rm edit
		rm ucos_ii.o     os_core.o

ucos_ii.o : ucos_ii.c  ucos_ii.h  os_cfg_r.h
		cc -c ucos_ii.c

os_core.o : os_core.c  ucos_ii.h  os_cfg_r.h
		cc -c os_core.c




