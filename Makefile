CC = gcc
CFLAG = -Wall -g


TARGET = exec

OBJ = main.o

main.o : main.c
	$(CC) $(CFLAG) -c main.c -o main.o

$(TARGET) : $(OBJ)
	$(CC) $(OBJ)  -o $(TARGET)

.PHONY = clean
clean :  
	rm -f $(TARGET) $(OBJ)

.PHONY : run
run : $(TARGET)
	./$(TARGET)

