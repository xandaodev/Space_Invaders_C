# Nome do executável
TARGET = jogo

# Compilador
CC = gcc

# Flags para Linux/WSL (o que fizemos funcionar)
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Arquivo fonte
SRC = spaceInvaders.c

all:
	$(CC) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)