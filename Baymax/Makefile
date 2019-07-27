CC=g++
bin=Baymax
src=Baymax.cc
INCLUDE=-Ispeech/
LIB=-ljsoncpp -lcurl -lcrypto -lpthread
FLAGS=-D_LOG_
STD=-std=c++11
$(bin):$(src)
	$(CC) -o $@ $^ $(FLAGS) $(INCLUDE) $(LIB) $(STD)
.PHONY:clean
clean:
	rm -f $(bin)



