SRCS=util.cpp random.cpp pri_queue.cpp kd_rect.cpp kd_node.cpp kd_tree.cpp main.cpp
OBJS=$(SRCS:.cpp=.o)

CXX = g++ -std=c++11
CPPFLAGS=-w -O3

.PHONY: clean

all: ${OBJS}
	${CXX} ${CPPFLAGS} -o qalsh_plus ${OBJS}

util.o: util.h

random.o: random.h

pri_queue.o: pri_queue.h

kd_rect.o: kd_rect.h

kd_node.o: kd_node.h

kd_tree.o: kd_tree.h

main.o:

clean:
	-rm ${OBJS} qalsh_plus *.o
