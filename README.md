# Bubble-Gumz

Bubble-Gumz é um jogo do género *Beat'em Up* desenvolvido em linguagem C, utilizando a biblioteca Raylib para renderização e manipulação de assets.

## Funcionalidades Principais

O projeto está organizado em diversos módulos para garantir uma arquitetura modular e escalável:

*   **Sistema de Combate (`combat.c`, `projectile.c`):** Lógica completa de ataques, detetores de hitboxes e projéteis.
*   **Gestão de Inimigos (`enemy.c`):** Suporte para diferentes tipos de inimigos (Melee, Ranged) e confrontos contra Bosses[cite: 5].
*   **Sistema de Waves (`wave.c`):** Progressão de jogo baseada em ondas de inimigos, permitindo uma dificuldade crescente[cite: 5].
*   **Narrativa (`cutscene.c`):** Integração de cutscenes para contar a história do jogo[cite: 5].
*   **Sistema de Save (`save_system.c`):** Funcionalidade para gravar e carregar o progresso do jogador, garantindo que o estado do jogo persista[cite: 5].
*   **UI e Menu (`ui.c`, `menu.c`):** Interface de utilizador e menus de navegação[cite: 5].

## Estrutura do Projeto

A organização dos assets e do código segue uma separação lógica:

*   `assets/`: Contém os sprites de jogadores, inimigos, boss, cenários e partículas[cite: 5].
*   `src/` (ou raiz): Contém a lógica de jogo dividida em ficheiros `.c` e `.h`[cite: 5].

## Como Compilar

Para compilar o projeto no Linux (GCC), utiliza o seguinte comando na raiz do projeto:

```bash
gcc *.c -o jogo -I. -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
