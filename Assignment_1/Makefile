# the compiler: gcc for C program, define as g++ for C++
CC = g++

# compiler flags:
#  -g     - this flag adds debugging information to the executable file
#  -Wall  - this flag is used to turn on most compiler warnings
CFLAGS  = -g -Wall

# The build target 
TARGET1 = A1_1
TARGET2 = A1_2
TARGET3 = A1_3

all: $(TARGET1) $(TARGET2) $(TARGET3)


$(TARGET1): $(TARGET1).cpp
	$(CC) $(CFLAGS) -o $(TARGET1) $(TARGET1).cpp

$(TARGET2): $(TARGET2).cpp
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).cpp -pthread

$(TARGET3): $(TARGET3).cpp
	$(CC) $(CFLAGS) -o $(TARGET3) $(TARGET3).cpp -pthread

clean:
	$(RM) $(TARGET1)
	$(RM) $(TARGET2)
	$(RM) $(TARGET3)