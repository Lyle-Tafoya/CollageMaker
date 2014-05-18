CC=g++
CFLAGS=`Magick++-config --cxxflags --cppflags` --std=c++11 -c -g
LFLAGS=`Magick++-config --ldflags --libs`
NAME=CollageMaker
OBJ=main.o

default: $(OBJ)
	$(CC) -o $(NAME) $(OBJ) $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

clean:
	rm -f $(OBJ) $(NAME)
