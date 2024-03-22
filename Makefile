
CC=gcc
FLAGS=-Wextra -Wall
BIN=./main
SRC=./src
OBJ=./obj

SOURCES=$(wildcard ${SRC}/*.c)
SUB_FOLDERS=$(wildcard ${SRC}/*/)

SUB_FOLDERS_SOURCES=$(wildcard $(addsuffix *.c, ${SUB_FOLDERS}))
SUB_FOLDERS_OBJECTS=$(filter %.o, $(foreach F, ${SUB_FOLDERS}, $(patsubst $F%.c, ${OBJ}/%.o, ${SUB_FOLDERS_SOURCES})))
OBJECTS=$(patsubst ${SRC}/%.c, ${OBJ}/%.o, ${SOURCES}) ${SUB_FOLDERS_OBJECTS}
INCLUDES= -I./

$(info $$SUB_FOLDERS = $(SUB_FOLDERS))
$(info $$SUB_FOLDERS_SOURCES = $(SUB_FOLDERS_SOURCES))
$(info $$SUB_FOLDERS_OBJECTS = $(SUB_FOLDERS_OBJECTS))
$(info $$OBJECTS = $(OBJECTS))

all: ${BIN}

${BIN}: ${OBJECTS}
	${CC} ${FLAGS} $^ -o $@

${OBJ}/%.o: ${SRC}/%.c
	${CC} ${FLAGS} -c $< -o $@

${OBJ}/%.o: ${SRC}/*/%.c
	${CC} ${FLAGS} -c $< -o $@

clear:
	rm -r ${OBJ}/* 