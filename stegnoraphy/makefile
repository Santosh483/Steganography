obj:=$(patsubst %.c, %.o, $(wildcard *.c))
stegno.exe: $(obj)
	gcc -o $@ $^
clean:
	rm *.exe *.o