src=$(wildcard *.cpp)
objs=$(patsubst %.cpp,%.o,$(src))
target=sltMain
LIBS=-lstdc++ -lssl -lcrypto -I /usr/include/mysql -L /usr/lib64/mysql/ -lmysqlclient -lstdc++ -lpthread -ldl -lrt
$(target):$(objs)
	g++ -o $@ $^ $(LIBS) 

%.o:%.cpp
#	g++ -std=c++11 -c $<
	g++ -c $^
.PHONY:clean
clean:
	rm -f $(target) $(objs)
