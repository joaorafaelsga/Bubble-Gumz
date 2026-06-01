# Nome do executável
TARGET = bubble-gumz

# Compilador e Flags
CC = gcc
CFLAGS = -I. -Wall

# Bibliotecas (Detecta automaticamente se estás no Windows ou Linux)
ifeq ($(OS),Windows_NT)
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

# Lista de ficheiros fonte
SRC = *.c

# Regra principal
all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

# Regra para compilar e rodar
run: all
	./$(TARGET)

# Regra para limpar os ficheiros compilados
clean:
	rm -f $(TARGET)