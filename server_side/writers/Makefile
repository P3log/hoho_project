CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++17 -pthread -I$(INCLUDE_DIR)

TARGET = writers
LOGGER = logger

SRC_DIR = ../src
OBJ_DIR = ../obj
BIN_DIR = ../bin
INCLUDE_DIR = ../include

SRC = $(SRC_DIR)/$(TARGET).cpp
OBJ = $(OBJ_DIR)/$(TARGET).o
BIN = $(BIN_DIR)/$(TARGET)

LOGGER_SRC = $(SRC_DIR)/$(LOGGER).cpp
LOGGER_OBJ = $(OBJ_DIR)/$(LOGGER).o
LOGGER_INCLUDE = $(INCLUDE_DIR)/$(LOGGER).hpp


$(BIN): $(OBJ) $(LOGGER_OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) $(LOGGER_OBJ) -o $(BIN)

$(OBJ): $(SRC)
	$(CXX) $(CXXFLAGS) -c $(SRC) -o $(OBJ)

$(LOGGER_OBJ): $(LOGGER_SRC) $(LOGGER_INCLUDE)
	$(CXX) $(CXXFLAGS) -c $(LOGGER_SRC) -o $(LOGGER_OBJ)

clean:
	rm -f $(OBJ) $(LOGGER_OBJ)

fclean: clean
	rm -f $(BIN)

re: fclean all