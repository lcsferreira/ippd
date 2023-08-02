#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 8

// Jogador 1 = Preto
// Jogador 2 = Branco

// Estrutura de dados para representar o tabuleiro
typedef struct {
  int tabuleiro[N][N];
  int pecasPretas;
  int pecasBrancas;
} Tabuleiro;

typedef struct {
  int x;
  int y;
} Coordenada;

typedef struct {
  int cor;
  int pecasCapturadas;
} Jogador;

// Função para inicializar o jogador
void inicializarJogador(Jogador *jogador, int cor) {
  jogador->cor = cor;
  jogador->pecasCapturadas = 0;
}
// Função para inicializar o tabuleiro
void inicializarTabuleiro(Tabuleiro *tabuleiro) {
  int i, j;

  // Inicializar todas as posições com 0
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      tabuleiro->tabuleiro[i][j] = 0;

  // Colocar as peças pretas
  for (i = 0; i < 3; i++)
    for (j = 0; j < N; j++)
      if ((i + j) % 2 == 0)
        tabuleiro->tabuleiro[i][j] = 1;

  // Colocar as peças brancas
  for (i = N - 3; i < N; i++)
    for (j = 0; j < N; j++)
      if ((i + j) % 2 == 0)
        tabuleiro->tabuleiro[i][j] = 2;

  // Inicializar o número de peças
  tabuleiro->pecasPretas = 12;
  tabuleiro->pecasBrancas = 12;
}

// Função para imprimir o tabuleiro
void imprimirTabuleiro(Tabuleiro *tabuleiro) {
  int i, j;

  printf("\n");

  // Imprimir as letras das colunas
  printf("  ");
  for (i = 0; i < N; i++)
    printf("%d ", i);
  printf("\n");

  // Imprimir o tabuleiro
  for (i = 0; i < N; i++) {
    printf("%d ", i);
    for (j = 0; j < N; j++) {
      if (tabuleiro->tabuleiro[i][j] == 0)
        printf("- ");
      else if (tabuleiro->tabuleiro[i][j] == 1)
        printf("P ");
      else if (tabuleiro->tabuleiro[i][j] == 2)
        printf("B ");
    }
    printf("\n");
  }

  printf("\n");
}

// Função para o jogador escolher a peça que deseja mover
Coordenada escolherPeca(Tabuleiro *tabuleiro, Jogador *jogador) {
  Coordenada coordenada;

  printf("Escolha a peca que deseja mover (linha coluna): ");
  scanf("%d %d", &coordenada.x, &coordenada.y);

  // Verificar se a peça escolhida é do jogador
  if (tabuleiro->tabuleiro[coordenada.x][coordenada.y] != jogador->cor) {
    printf("A peca escolhida nao pertence ao jogador!\n");
    escolherPeca(tabuleiro, jogador);
  } else {
    return coordenada;
  }
}

// funcao para verificar movimento de captura
void capturarPeca(Tabuleiro *tabuleiro, Jogador *jogador, Coordenada coordenada, Coordenada novaCoordenada) {
  int coordenadaX_capturada = (coordenada.x + novaCoordenada.x) / 2;
  int coordenadaY_capturada = (coordenada.y + novaCoordenada.y) / 2;
  tabuleiro->tabuleiro[coordenada.x][coordenada.y] = 0;
  tabuleiro->tabuleiro[coordenadaX_capturada][coordenadaY_capturada] = 0;
  tabuleiro->tabuleiro[novaCoordenada.x][novaCoordenada.y] = jogador->cor;
  jogador->pecasCapturadas++;
  printf("Peca capturada!\n");
  printf("Voce possui %d pecas capturadas\n", jogador->pecasCapturadas);
}

// Função para o jogador escolher a posição para onde deseja mover a peça
Coordenada escolherPosicao(Tabuleiro *tabuleiro, Jogador *jogador, Coordenada *coordenada) {
  Coordenada novaCoordenada;

  printf("Escolha a posicao para onde deseja mover a peca (linha coluna): ");
  scanf("%d %d", &novaCoordenada.x, &novaCoordenada.y);

  // Verificar se a posição escolhida é válida
  if (novaCoordenada.x < 0 || novaCoordenada.x > 7 || novaCoordenada.y < 0 ||
      novaCoordenada.y > 7) {
    printf("Posicao invalida!\n");
    escolherPosicao(tabuleiro, jogador, coordenada);
  } else {
    // Verificar se a posição escolhida está vazia
    if (tabuleiro->tabuleiro[novaCoordenada.x][novaCoordenada.y] != 0) {
      printf("Posicao ocupada!\n");
      escolherPosicao(tabuleiro, jogador, coordenada);
    } else {
      // Verificar se a posição escolhida é válida
      if (jogador->cor == 1) {
        if (novaCoordenada.x == coordenada->x + 1) {
          if (novaCoordenada.y == coordenada->y + 1 || novaCoordenada.y == coordenada->y - 1) {
            return novaCoordenada;
          } else {
            printf("Voce deve escolher uma diagonal!\n");
            escolherPosicao(tabuleiro, jogador, coordenada);
          }
        } else {
          if (novaCoordenada.x == coordenada->x + 2) {
            if (novaCoordenada.y == coordenada->y + 2 || novaCoordenada.y == coordenada->y - 2) {
              // capturarPeca(tabuleiro, jogador, coordenada, novaCoordenada);
              return novaCoordenada;
            } else {
              printf("Voce deve escolher uma diagonal!\n");
              escolherPosicao(tabuleiro, jogador, coordenada);
            }
          } else {
            printf("Voce nao pode mover para tras!\n");
            escolherPosicao(tabuleiro, jogador, coordenada);
          }
          printf("Voce nao pode mover para tras!\n");
          escolherPosicao(tabuleiro, jogador, coordenada);
        }

      } else if (jogador->cor == 2) {
        if (novaCoordenada.x == coordenada->x - 1) {
          if (novaCoordenada.y == coordenada->y + 1 || novaCoordenada.y == coordenada->y - 1) {
            return novaCoordenada;
          } else {
            printf("Voce deve escolher uma diagonal!\n");
            escolherPosicao(tabuleiro, jogador, coordenada);
          }
        } else {
          if (novaCoordenada.x == coordenada->x - 2) {
            if (novaCoordenada.y == coordenada->y + 2 || novaCoordenada.y == coordenada->y - 2) {
              // capturarPeca(tabuleiro, jogador, coordenada, novaCoordenada);
              return novaCoordenada;
            } else {
              printf("Voce deve escolher uma diagonal!\n");
              escolherPosicao(tabuleiro, jogador, coordenada);
            }
          } else {
            printf("Voce nao pode mover para tras!\n");
            escolherPosicao(tabuleiro, jogador, coordenada);
          }
          printf("Voce nao pode mover para tras!\n");
          escolherPosicao(tabuleiro, jogador, coordenada);
        }
      }
    }
  }
}

// Função para executar o movimento da peça no tabuleiro
void moverPeca(Tabuleiro *tabuleiro, Jogador *jogador, Coordenada coordenada, Coordenada novaCoordenada, int *jogador_da_rodada) {

  if (novaCoordenada.x == coordenada.x + 2 || novaCoordenada.x == coordenada.x - 2) {
    capturarPeca(tabuleiro, jogador, coordenada, novaCoordenada);
    if (jogador->cor == 1) {
      tabuleiro->pecasBrancas--;
      *jogador_da_rodada = 1;
    } else {
      tabuleiro->pecasPretas--;
      *jogador_da_rodada = 2;
    }
  } else {
    tabuleiro->tabuleiro[coordenada.x][coordenada.y] = 0;
    tabuleiro->tabuleiro[novaCoordenada.x][novaCoordenada.y] = jogador->cor;
    if (jogador->cor == 1) {
      *jogador_da_rodada = 2;
    } else {
      *jogador_da_rodada = 1;
    }
  }
}

// Função para pegar as coordenadas das peças pretas
void pegarCoordenadasPecasPretas(Tabuleiro *tabuleiro, Coordenada *coordenadasPecasPretas) {
  int i, j;
  int k = 0;
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      if (tabuleiro->tabuleiro[i][j] == 1) {
        coordenadasPecasPretas[k].x = i;
        coordenadasPecasPretas[k].y = j;
        k++;
      }
}

// função para selecionar randomicamente uma peça preta
Coordenada selecionarPecaPretaAleatoria(Tabuleiro *tabuleiro, Coordenada *coordenadasPecasPretas) {
  int i;
  int pecaAleatoria = rand() % tabuleiro->pecasPretas;
  Coordenada coordenada;
  coordenada.x = coordenadasPecasPretas[pecaAleatoria].x;
  coordenada.y = coordenadasPecasPretas[pecaAleatoria].y;
  return coordenada;
}

// Função para testar as jogadas possíveis da peças preta selecionada
Coordenada *testarJogadasPossiveisPecasPretas(Tabuleiro *tabuleiro, Coordenada coordenada, Coordenada *jogadasPossiveisPecasPretas) {
  int i;
  int k = 0;
  if (coordenada.x + 1 < N && coordenada.y + 1 < N) {
    if (tabuleiro->tabuleiro[coordenada.x + 1][coordenada.y + 1] == 0) {
      jogadasPossiveisPecasPretas[k].x = coordenada.x + 1;
      jogadasPossiveisPecasPretas[k].y = coordenada.y + 1;
      k++;
    }
  }
  if (coordenada.x + 1 < N && coordenada.y - 1 >= 0) {
    if (tabuleiro->tabuleiro[coordenada.x + 1][coordenada.y - 1] == 0) {
      jogadasPossiveisPecasPretas[k].x = coordenada.x + 1;
      jogadasPossiveisPecasPretas[k].y = coordenada.y - 1;
      k++;
    }
  }
  if (coordenada.x + 2 < N && coordenada.y + 2 < N) {
    if (tabuleiro->tabuleiro[coordenada.x + 2][coordenada.y + 2] == 0) {
      jogadasPossiveisPecasPretas[k].x = coordenada.x + 2;
      jogadasPossiveisPecasPretas[k].y = coordenada.y + 2;
      k++;
    }
  }
  if (coordenada.x + 2 < N && coordenada.y - 2 >= 0) {
    if (tabuleiro->tabuleiro[coordenada.x + 2][coordenada.y - 2] == 0) {
      jogadasPossiveisPecasPretas[k].x = coordenada.x + 2;
      jogadasPossiveisPecasPretas[k].y = coordenada.y - 2;
      k++;
    }
  }
}

// Função para pegar uma jogada aleatória das peças pretas
Coordenada pegarJogadaAleatoriaPecasPretas(Tabuleiro *tabuleiro, Coordenada *jogadasPossiveisPecasPretas) {
  int i;
  int jogadaAleatoria = rand() % tabuleiro->pecasPretas;
  Coordenada jogada;
  jogada.x = jogadasPossiveisPecasPretas[jogadaAleatoria].x;
  jogada.y = jogadasPossiveisPecasPretas[jogadaAleatoria].y;
  return jogada;
}

int main() {
  Tabuleiro tabuleiro;
  Jogador jogadorMaquina;
  Jogador jogadorHumano;
  int gameover = 0;
  int jogador_da_rodada = 1;
  inicializarTabuleiro(&tabuleiro);
  imprimirTabuleiro(&tabuleiro);
  inicializarJogador(&jogadorMaquina, 1);
  inicializarJogador(&jogadorHumano, 2);

  while (!gameover) {
    if (jogador_da_rodada == 1) {
      // printf("Jogador 1 - Voce eh as pecas pretas\n");
      // Coordenada coordenada = escolherPeca(&tabuleiro, &jogadorMaquina);
      // Coordenada novaCoordenada = escolherPosicao(&tabuleiro, &jogadorMaquina, &coordenada);
      // moverPeca(&tabuleiro, &jogadorMaquina, coordenada, novaCoordenada, &jogador_da_rodada);
      printf("Vez do Computador\n");
      // Coordenada coordenadasPecasPretas[tabuleiro.pecasPretas];
      // Coordenada jogadasPossiveisPecasPretas[tabuleiro.pecasPretas * 4];

      Coordenada *coordenadasPecasPretas = (Coordenada *)malloc(sizeof(Coordenada) * tabuleiro.pecasPretas);
      Coordenada *jogadasPossiveisPecasPretas = (Coordenada *)malloc(sizeof(Coordenada) * 4);
      if (coordenadasPecasPretas == NULL || jogadasPossiveisPecasPretas == NULL) {
        printf("Erro ao alocar memoria\n");
        exit(1);
      }

      pegarCoordenadasPecasPretas(&tabuleiro, coordenadasPecasPretas);

      for (int i = 0; i <= 11; i++) {
        printf("coordenadasPecasPretas[%d].x = %d\n", i, coordenadasPecasPretas[i].x);
        printf("coordenadasPecasPretas[%d].y = %d\n", i, coordenadasPecasPretas[i].y);
      }

      Coordenada coordenada = selecionarPecaPretaAleatoria(&tabuleiro, coordenadasPecasPretas);

      printf("coordenada.x = %d\n", coordenada.x);
      printf("coordenada.y = %d\n", coordenada.y);

      testarJogadasPossiveisPecasPretas(&tabuleiro, coordenada, jogadasPossiveisPecasPretas);

      for (int i = 0; i <= 4; i++) {
        printf("jogadasPossiveisPecasPretas[%d].x = %d\n", i, jogadasPossiveisPecasPretas[i].x);
        printf("jogadasPossiveisPecasPretas[%d].y = %d\n", i, jogadasPossiveisPecasPretas[i].y);
      }

      Coordenada jogada = pegarJogadaAleatoriaPecasPretas(&tabuleiro, jogadasPossiveisPecasPretas);

      printf("jogada.x = %d\n", jogada.x);
      printf("jogada.y = %d\n", jogada.y);

      moverPeca(&tabuleiro, &jogadorMaquina, coordenada, jogada, &jogador_da_rodada);
      imprimirTabuleiro(&tabuleiro);

      free(coordenadasPecasPretas);
      free(jogadasPossiveisPecasPretas);
    } else {
      printf("Sua vez - Voce eh as pecas brancas\n");
      Coordenada coordenada = escolherPeca(&tabuleiro, &jogadorHumano);
      Coordenada novaCoordenada = escolherPosicao(&tabuleiro, &jogadorHumano, &coordenada);
      moverPeca(&tabuleiro, &jogadorHumano, coordenada, novaCoordenada, &jogador_da_rodada);
      imprimirTabuleiro(&tabuleiro);
    }
    if (tabuleiro.pecasPretas == 0) {
      printf("Voce venceu!\n");
      gameover = 1;
    } else if (tabuleiro.pecasBrancas == 0) {
      printf("Maquina venceu!\n");
      gameover = 1;
    }
  }
  return 0;
}