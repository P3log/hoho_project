CXX = g++
CC=gcc
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -pthread
CFLAGS = -Wall -Wextra -Werror

TARGET = dht22_tracer
SENSOR = dht22

SRC_DIR = ../src
OBJ_DIR = ../obj
BIN_DIR = ../bin
INCLUDE_DIR = ../include

PROG_SRC = $(SRC_DIR)/$(TARGET).cpp
PROG_OBJ = $(OBJ_DIR)/$(TARGET).o
PROG_BIN = $(BIN_DIR)/$(TARGET)

SENSOR_SRC = $(SRC_DIR)/$(SENSOR).c
SENSOR_OBJ = $(OBJ_DIR)/$(SENSOR).o
SENSOR_INCLUDE = $(INCLUDE_DIR)/$(SENSOR).h

all: $(PROG_BIN)

$(PROG_BIN): $(PROG_OBJ) $(SENSOR_OBJ)
	$(CXX) $(CXXFLAGS) $(PROG_OBJ) $(SENSOR_OBJ) -o $(PROG_BIN) -lpigpio

$(PROG_OBJ): $(PROG_SRC)
	$(CXX) $(CXXFLAGS) -c $(PROG_SRC) -o $(PROG_OBJ)

$(SENSOR_OBJ): $(SENSOR_SRC) $(SENSOR_INCLUDE)
	$(CC) $(CFLAGS) -c $(SENSOR_SRC) -o $(SENSOR_OBJ)

clean:
	rm -f $(OBJ_DIR)/*.o

fclean: clean
	rm -f $(PROG_BIN)

re: fclean all