# Bubble-Gumz

Recriação de um jogo *Beat'em Up* em C utilizando a biblioteca [raylib](https://www.raylib.com/). O objetivo é derrotar ondas de inimigos, gerir a vida dos personagens (Gumz e Bubbles) e enfrentar bosses desafiadores em cenários dinâmicos.

## Funcionalidades

* **Sistema de Combate:** Lógica de hitboxes, ataques e projéteis.
* **Gestão de Inimigos:** Inimigos com IA básica, suporte para tipos diferentes (Melee, Ranged) e Boss "Trator".
* **Sistema de Waves:** Progressão automática baseada em ondas de inimigos.
* **Persistência:** Sistema de Save/Load de progresso e ranking (persistido em arquivo `.txt`).
* **Multijogador:** Suporte para dois jogadores (P1 e P2) com controlos locais.

## Controles

### Jogabilidade

| Ação | Jogador 1 (Gumz) | Jogador 2 (Bubbles) |
| :--- | :--- | :--- |
| **Movimentação** | W, A, S, D | Setas do Teclado |
| **Ataque** | R | K |
| **Dash / Bloqueio** | Q (Dash) | 0 (Numpad - Bloqueio) |

### Sistema e Menu

| Ação | Tecla |
| :--- | :--- |
| **Salvar Jogo** | 1 |
| **Carregar Jogo** | 2 |
| **Menu Principal** | M |
| **Confirmar / Iniciar** | Espaço |

## Como Compilar (Makefile)

Utiliza o `Makefile` na raiz do projeto para automatizar a compilação:

```makefile
# Nome do executável
TARGET = bubble-gumz

# Compilador e Flags
CC = gcc
CFLAGS = -I. -Wall

# Bibliotecas (Detecta SO)
ifeq ($(OS),Windows_NT)
    LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    TARGET := $(TARGET).exe
else
    LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

SRC = *.c

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LIBS)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
