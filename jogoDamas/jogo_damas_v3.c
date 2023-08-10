#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 8
#define MAX_JOGADAS 30
#define MAX_THREADS 4
#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b
#define preto 1
#define branco 2
// Jogador Maquina = 1 = Preto
// Jogador Humano = 2 = Branco

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
} Jogador;

typedef struct {
  Coordenada posicaoAtual;
  Coordenada posicaoNova;
} Jogada;

// decalaração de funções
void inicializarJogador(Jogador *jogador, int cor);
void inicializarTabuleiro(Tabuleiro *tabuleiro);
void imprimirTabuleiro(Tabuleiro *tabuleiro);
Coordenada escolherPeca(Tabuleiro *tabuleiro, Jogador *jogador);
Coordenada escolherPosicao(Tabuleiro *tabuleiro, Jogador *jogador, Coordenada *coordenada);
void moverPeca(Tabuleiro *tabuleiro, Jogador *jogador, Coordenada coordenada, Coordenada novaCoordenada, int *jogador_da_rodada);
void capturarPeca(Tabuleiro *tabuleiro, Jogador *jogador, Coordenada coordenada, Coordenada novaCoordenada);
void pegarCoordenadasPecasPretas(Tabuleiro *tabuleiro, Coordenada *coordenadasPecasPretas);
void verificarPossiveisJogadas(Tabuleiro *tabuleiro, Coordenada *coordenadasPecasPretas, Jogada *jogadasValidas);
Jogada pegarJogadaAleatoriaPecasPretas(Tabuleiro *tabuleiro, Jogada *jogadasValidas);
Jogada minimaxStart(Tabuleiro *tabuleiro, Jogador *jogador, Jogador *jogadorHumano, int maximizando);
int minimax(Tabuleiro *tabuleiro, Jogador *jogador, Jogador *jogadorHumano, Jogador *lado, int maximizando, int profundidade, int alpha, int beta);
int pegarHeuristica(Tabuleiro *tabuleiro);

// Função para inicializar o jogador
void inicializarJogador(Jogador *jogador, int cor) {
  jogador->cor = cor;
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
  // printf("Peca capturada!\n");
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
  if (novaCoordenada.y == coordenada.y + 2 || novaCoordenada.y == coordenada.y - 2) {
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

void verificarPossiveisJogadas(Tabuleiro *tabuleiro, Coordenada *coordenadasPecasPretas, Jogada *jogadasValidas) {
  // para cada peca preta, verificar as possiveis jogadas (diagonal superior esquerda, diagonal superior direita, diagonal inferior esquerda, diagonal inferior direita)
  // verificar as jogadas de captura (diagonal superior esquerda, diagonal superior direita, diagonal inferior esquerda, diagonal inferior direita)
  // se a posicao estiver vazia, adicionar na lista de possiveis jogadas
  int k = 0;

  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < tabuleiro->pecasPretas; i++) {
        if (tabuleiro->tabuleiro[coordenadasPecasPretas[i].x][coordenadasPecasPretas[i].y] != 0) {
          if (!(coordenadasPecasPretas[i].x + 1 >= N || coordenadasPecasPretas[i].y - 1 < 0) && !(coordenadasPecasPretas[i].x + 1 >= N || coordenadasPecasPretas[i].y + 1 >= N)) {
            // JOGADA
            if (tabuleiro->tabuleiro[coordenadasPecasPretas[i].x + 1][coordenadasPecasPretas[i].y - 1] == 0) {
              // adicionar na lista de possiveis jogadas
              jogadasValidas[k].posicaoAtual.x = coordenadasPecasPretas[i].x;
              jogadasValidas[k].posicaoAtual.y = coordenadasPecasPretas[i].y;
              jogadasValidas[k].posicaoNova.x = coordenadasPecasPretas[i].x + 1;
              jogadasValidas[k].posicaoNova.y = coordenadasPecasPretas[i].y - 1;
              k++;
            }
            if (tabuleiro->tabuleiro[coordenadasPecasPretas[i].x + 1][coordenadasPecasPretas[i].y + 1] == 0) {
              // adicionar na lista de possiveis jogadas
              jogadasValidas[k].posicaoAtual.x = coordenadasPecasPretas[i].x;
              jogadasValidas[k].posicaoAtual.y = coordenadasPecasPretas[i].y;
              jogadasValidas[k].posicaoNova.x = coordenadasPecasPretas[i].x + 1;
              jogadasValidas[k].posicaoNova.y = coordenadasPecasPretas[i].y + 1;
              k++;
            }
          }
          if (!(coordenadasPecasPretas[i].x + 2 >= N || coordenadasPecasPretas[i].y - 2 < 0) && !(coordenadasPecasPretas[i].x + 2 >= N || coordenadasPecasPretas[i].y + 2 >= N)) {
            // JOGADA
            if (tabuleiro->tabuleiro[coordenadasPecasPretas[i].x + 2][coordenadasPecasPretas[i].y - 2] == 0) {
              if (tabuleiro->tabuleiro[coordenadasPecasPretas[i].x + 1][coordenadasPecasPretas[i].y - 1] == 2) {
                // adicionar na lista de possiveis jogadas
                jogadasValidas[k].posicaoAtual.x = coordenadasPecasPretas[i].x;
                jogadasValidas[k].posicaoAtual.y = coordenadasPecasPretas[i].y;
                jogadasValidas[k].posicaoNova.x = coordenadasPecasPretas[i].x + 2;
                jogadasValidas[k].posicaoNova.y = coordenadasPecasPretas[i].y - 2;
                k++;
              }
            }
            if (tabuleiro->tabuleiro[coordenadasPecasPretas[i].x + 2][coordenadasPecasPretas[i].y + 2] == 0) {
              if (tabuleiro->tabuleiro[coordenadasPecasPretas[i].x + 1][coordenadasPecasPretas[i].y + 1] == 2) {
                // adicionar na lista de possiveis jogadas
                jogadasValidas[k].posicaoAtual.x = coordenadasPecasPretas[i].x;
                jogadasValidas[k].posicaoAtual.y = coordenadasPecasPretas[i].y;
                jogadasValidas[k].posicaoNova.x = coordenadasPecasPretas[i].x + 2;
                jogadasValidas[k].posicaoNova.y = coordenadasPecasPretas[i].y + 2;
                k++;
              }
            }
          }
        }
      }
  }
}

Jogada pegarJogadaAleatoriaPecasPretas(Tabuleiro *tabuleiro, Jogada *jogadasValidas) {
  // Escolher randomicamente uma jogada da lista de possiveis jogadas
  srand(time(NULL));
  int random = rand() % tabuleiro->pecasPretas * 4;
  // verificar se a jogada nao vai de 00 para 00
  while (jogadasValidas[random].posicaoAtual.x == 0 && jogadasValidas[random].posicaoAtual.y == 0 && jogadasValidas[random].posicaoNova.x == 0 && jogadasValidas[random].posicaoNova.y == 0) {
    random = rand() % tabuleiro->pecasPretas * 4;
  }
  return jogadasValidas[random];
}

Jogada minimaxStart(Tabuleiro *tabuleiro, Jogador *jogador, Jogador *jogadorHumano, int maximizando) {
  int alpha = -1000;
  int beta = 1000;

  Coordenada *coordenadasPecasPretas = (Coordenada *)malloc(sizeof(Coordenada) * tabuleiro->pecasPretas);
  Jogada *jogadasValidas = (Jogada *)malloc(sizeof(Jogada) * MAX_JOGADAS);
  Jogador *lado = (Jogador *)malloc(sizeof(Jogador));
  int *heuristicas = (int *)malloc(sizeof(int) * MAX_JOGADAS);

  lado->cor = jogador->cor;
  // cria um vetor para heuristicas

  if (coordenadasPecasPretas == NULL || jogadasValidas == NULL || heuristicas == NULL) {
    printf("Erro ao alocar memoria\n");
    exit(1);
  }

  pegarCoordenadasPecasPretas(tabuleiro, coordenadasPecasPretas);

  // limpa a lista de jogadas validas
  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < MAX_JOGADAS; i++) {
        jogadasValidas[i].posicaoAtual.x = -1;
        jogadasValidas[i].posicaoAtual.y = -1;
        jogadasValidas[i].posicaoNova.x = -1;
        jogadasValidas[i].posicaoNova.y = -1;
      }
  }

  verificarPossiveisJogadas(tabuleiro, coordenadasPecasPretas, jogadasValidas);
  // se não houver jogadas validas
  if (jogadasValidas[0].posicaoAtual.x == -1 && jogadasValidas[0].posicaoAtual.y == -1 && jogadasValidas[0].posicaoNova.x == -1 && jogadasValidas[0].posicaoNova.y == -1) {
    printf("Não há jogadas validas\n");
    free(coordenadasPecasPretas);
    free(jogadasValidas);
    free(heuristicas);
    return jogadasValidas[0];
  }

  // faz uma copia do tabuleiro atual
  Tabuleiro *tabuleiroCopia = (Tabuleiro *)malloc(sizeof(Tabuleiro));

  if (tabuleiroCopia == NULL) {
    printf("Erro ao alocar memoria\n");
    exit(1);
  }

  int numeroJogadasValidas = 0;

  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < MAX_JOGADAS; i++) {
        if (jogadasValidas[i].posicaoAtual.x != jogadasValidas[i].posicaoNova.x && jogadasValidas[i].posicaoAtual.y != jogadasValidas[i].posicaoNova.y) {
          if (jogadasValidas[i].posicaoAtual.x >= 0 && jogadasValidas[i].posicaoAtual.y >= 0 && jogadasValidas[i].posicaoNova.x >= 0 && jogadasValidas[i].posicaoNova.y >= 0) {
            if (jogadasValidas[i].posicaoAtual.x < N && jogadasValidas[i].posicaoAtual.y < N && jogadasValidas[i].posicaoNova.x < N && jogadasValidas[i].posicaoNova.y < N) {
              numeroJogadasValidas++;
            }
          }
        }
      }
  }

  for (int i = 0; i < numeroJogadasValidas; i++) {
    memcpy(tabuleiroCopia, tabuleiro, sizeof(Tabuleiro));
    moverPeca(tabuleiroCopia, jogador, jogadasValidas[i].posicaoAtual, jogadasValidas[i].posicaoNova, &lado->cor);
    heuristicas[i] = minimax(tabuleiroCopia, jogador, jogadorHumano, lado, maximizando, 3, alpha, beta);
    // printf("Heuristica avaliada %d\n", heuristicas[i]);
  }

  // printf("heuristicas avaliadas\n");

  int max_heuristica = -1000;

  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < numeroJogadasValidas; i++) {
        if (heuristicas[i] >= max_heuristica) {
          max_heuristica = heuristicas[i];
        }
      }
  }

  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < numeroJogadasValidas; i++) {
        if (heuristicas[i] < max_heuristica) {
          jogadasValidas[i].posicaoAtual.x = -1;
          jogadasValidas[i].posicaoAtual.y = -1;
          jogadasValidas[i].posicaoNova.x = -1;
          jogadasValidas[i].posicaoNova.y = -1;

          heuristicas[i] = -1000;
        }
      }
  }

  numeroJogadasValidas = 0;

  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < MAX_JOGADAS; i++) {
        if (jogadasValidas[i].posicaoAtual.x != jogadasValidas[i].posicaoNova.x && jogadasValidas[i].posicaoAtual.y != jogadasValidas[i].posicaoNova.y) {
          if (jogadasValidas[i].posicaoAtual.x >= 0 && jogadasValidas[i].posicaoAtual.y >= 0 && jogadasValidas[i].posicaoNova.x >= 0 && jogadasValidas[i].posicaoNova.y >= 0) {
            if (jogadasValidas[i].posicaoAtual.x < N && jogadasValidas[i].posicaoAtual.y < N && jogadasValidas[i].posicaoNova.x < N && jogadasValidas[i].posicaoNova.y < N) {
              numeroJogadasValidas++;
            }
          }
        }
      }
  }

  srand(time(NULL));
  int random = rand() % numeroJogadasValidas;
  // verificar se a jogada nao vai de 00 para 00
  while (jogadasValidas[random].posicaoAtual.x == -1 && jogadasValidas[random].posicaoAtual.y == -1 && jogadasValidas[random].posicaoNova.x == -1 && jogadasValidas[random].posicaoNova.y == -1) {
    random = rand() % numeroJogadasValidas;
  }
  Jogada jogada = jogadasValidas[random];

  free(coordenadasPecasPretas);
  free(jogadasValidas);
  free(heuristicas);
  free(tabuleiroCopia);
  return jogada;
}

int minimax(Tabuleiro *tabuleiro, Jogador *jogador, Jogador *jogadorHumano, Jogador *lado, int maximizando, int profundidade, int alpha, int beta) {
  Coordenada *coordenadasPecasPretas = (Coordenada *)malloc(sizeof(Coordenada) * tabuleiro->pecasPretas);
  Jogada *jogadasValidas = (Jogada *)malloc(sizeof(Jogada) * MAX_JOGADAS);
  Tabuleiro *tabuleiroCopia = (Tabuleiro *)malloc(sizeof(Tabuleiro));
  // printf("Profundidade %d\n", profundidade);

  if (tabuleiroCopia == NULL) {
    printf("Erro ao alocar memoria\n");
    exit(1);
  }
  if (coordenadasPecasPretas == NULL || jogadasValidas == NULL) {
    printf("Erro ao alocar memoria\n");
    exit(1);
  }

  // imprimirTabuleiro(tabuleiro);
  pegarCoordenadasPecasPretas(tabuleiro, coordenadasPecasPretas);
  // for (int i = 0; i < tabuleiro->pecasPretas; i++) {
  //   printf("Coordenada %d %d\n", coordenadasPecasPretas[i].x, coordenadasPecasPretas[i].y);
  // }

  // clear jogadasValidas
  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < MAX_JOGADAS; i++) {
        jogadasValidas[i].posicaoAtual.x = -1;
        jogadasValidas[i].posicaoAtual.y = -1;
        jogadasValidas[i].posicaoNova.x = -1;
        jogadasValidas[i].posicaoNova.y = -1;
      }
  }

  verificarPossiveisJogadas(tabuleiro, coordenadasPecasPretas, jogadasValidas);
  // printf("\n");
  int numeroJogadasValidas = 0;

  #pragma omp parallel
  {
    #pragma omp for schedule(dynamic)
      for (int i = 0; i < MAX_JOGADAS; i++) {
        if (jogadasValidas[i].posicaoAtual.x != jogadasValidas[i].posicaoNova.x && jogadasValidas[i].posicaoAtual.y != jogadasValidas[i].posicaoNova.y) {
          if (jogadasValidas[i].posicaoAtual.x >= 0 && jogadasValidas[i].posicaoAtual.y >= 0 && jogadasValidas[i].posicaoNova.x >= 0 && jogadasValidas[i].posicaoNova.y >= 0) {
            if (jogadasValidas[i].posicaoAtual.x < N && jogadasValidas[i].posicaoAtual.y < N && jogadasValidas[i].posicaoNova.x < N && jogadasValidas[i].posicaoNova.y < N) {
              numeroJogadasValidas++;
            }
          }
        }
      }
  }
  // printf("Numero de jogadas validas %d\n", numeroJogadasValidas);

  // for (int i = 0; i < numeroJogadasValidas; i++) {
  //   printf("Jogada %d %d para %d %d\n", jogadasValidas[i].posicaoAtual.x, jogadasValidas[i].posicaoAtual.y, jogadasValidas[i].posicaoNova.x, jogadasValidas[i].posicaoNova.y);
  // }
  // printf("\n----\n");

  if (profundidade == 0) {
    int heuristica = pegarHeuristica(tabuleiro);
    free(coordenadasPecasPretas);
    free(jogadasValidas);
    free(tabuleiroCopia);
    return heuristica;
  }

  int inicial = 0;

  if (maximizando == 1) {
    #pragma omp parallel
    {
      #pragma omp for schedule(dynamic)
        for (int i = 0; i < numeroJogadasValidas; i++) {
          memcpy(tabuleiroCopia, tabuleiro, sizeof(Tabuleiro));
          // printf("Jogada MAX %d %d para %d %d\n", jogadasValidas[i].posicaoAtual.x, jogadasValidas[i].posicaoAtual.y, jogadasValidas[i].posicaoNova.x, jogadasValidas[i].posicaoNova.y);
          moverPeca(tabuleiroCopia, jogador, jogadasValidas[i].posicaoAtual, jogadasValidas[i].posicaoNova, &lado->cor);
          if (maximizando == 1) {
            maximizando = 0;
          } else {
            maximizando = 1;
          }
          int valor = minimax(tabuleiroCopia, jogador, jogadorHumano, lado, maximizando, profundidade - 1, alpha, beta);
          inicial = max(valor, inicial);
          alpha = max(inicial, alpha);
          if (alpha >= beta) {
            break;
          }
        }
    }
  } else {
    inicial = 1000;
    #pragma omp parallel
    {
      #pragma omp for schedule(dynamic)
        for (int i = 0; i < numeroJogadasValidas; i++) {
          memcpy(tabuleiroCopia, tabuleiro, sizeof(Tabuleiro));
          // printf("Jogada MIN %d %d para %d %d\n", jogadasValidas[i].posicaoAtual.x, jogadasValidas[i].posicaoAtual.y, jogadasValidas[i].posicaoNova.x, jogadasValidas[i].posicaoNova.y);
          moverPeca(tabuleiroCopia, jogador, jogadasValidas[i].posicaoAtual, jogadasValidas[i].posicaoNova, &lado->cor);
          if (maximizando == 1) {
            maximizando = 0;
          } else {
            maximizando = 1;
          }
          int valor = minimax(tabuleiroCopia, jogador, jogadorHumano, lado, maximizando, profundidade - 1, alpha, beta);
          inicial = min(valor, inicial);
          beta = min(beta, inicial);
          if (alpha >= beta) {
            break;
          }
        }
    }
  }

  free(coordenadasPecasPretas);
  free(jogadasValidas);
  free(tabuleiroCopia);
  return inicial;
}

int pegarHeuristica(Tabuleiro *tabuleiro) {
  // heuristica = (quantidade de pecas pretas - quantidade de pecas brancas) / (quantidade de pecas pretas + quantidade de pecas brancas)
  int heuristica = (tabuleiro->pecasPretas - tabuleiro->pecasBrancas);
  return heuristica;
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
      printf("Vez do Computador\n");

      Jogada jogada = minimaxStart(&tabuleiro, &jogadorMaquina, &jogadorHumano, 1);
      printf("Mover peca %d %d para %d %d\n", jogada.posicaoAtual.x, jogada.posicaoAtual.y, jogada.posicaoNova.x, jogada.posicaoNova.y);

      moverPeca(&tabuleiro, &jogadorMaquina, jogada.posicaoAtual, jogada.posicaoNova, &jogador_da_rodada);
      imprimirTabuleiro(&tabuleiro);
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