.PHONY:all
all:fifo1 fifo2
fifo1:fifo1.c
	gcc -o $@ $^
fifo2:fifo2.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -f fifo1 fifo2
