#include "jogo_damas_v4.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define max(a, b) a > b ? a : b
#define min(a, b) a < b ? a : b

// decladacao das funcoes
void clear();
void imprimeMovimento(Movimento movimento);
TipoPeca pegarPeca(Tabuleiro *tabuleiro, int linha, int coluna);
TipoPeca pegarPecaPonto(Tabuleiro *tabuleiro, Ponto ponto);
void inicializarTabuleiro(Tabuleiro *tabuleiro);
void imprimeTabuleiro(Tabuleiro *tabuleiro);
void gerarMovimentosValidosPeca(int linha, int coluna, Movimento *movimentos, int *qtdMovimentos, Tabuleiro *tabuleiro);
Ponto pegarPecaMeio(Movimento *movimento);
int ehPecaOponente(TipoPeca peca, Lado lado);
int ehPecaPropria(Tabuleiro *tabuleiro, int linha, int coluna, Lado lado);
void gerarMovimentosPuloPeca(int linha, int coluna, Lado lado, Movimento *movimentos, int *qtdMovimentos, Tabuleiro *tabuleiro);
int movimentoEhIgual(Movimento movimento1, Movimento movimento2);
Movimento *gerarTodosMovimentos(Lado lado, Movimento *movimentos, int *qtdMovimentos, Tabuleiro *tabuleiro);
Status fazerMovimentoTabuleiro(Tabuleiro *tabuleiro, Movimento movimento, Lado lado);
Tabuleiro clonaTabuleiro(Tabuleiro *tabuleiro);
Status fazerMovimentoJogador(Tabuleiro *tabuleiro, Movimento movimento, Jogador jogador);
double pegarHeuristica(Tabuleiro *tabuleiro, Lado lado);
Lado trocaLado(Lado lado);
double minimax(Tabuleiro *tabuleiro, Lado lado, int maximizando, int profundidade, int alpha, int beta);
Movimento minimaxStart(Tabuleiro *tabuleiro, Lado lado, int maximizando, int profundidade);
Status fazerMovimentoIA(Tabuleiro *tabuleiro, IA ia);
void jogar();

void clear() {
  system("cls");
}

void imprimeMovimento(Movimento movimento) {
  printf("Move: (%d, %d) para (%d, %d)\n", movimento.origem.linha, movimento.origem.coluna, movimento.destino.linha, movimento.destino.coluna);
}

TipoPeca pegarPeca(Tabuleiro *tabuleiro, int linha, int coluna) {
  return tabuleiro->tabuleiro[linha][coluna];
}

TipoPeca pegarPecaPonto(Tabuleiro *tabuleiro, Ponto ponto) {
  return tabuleiro->tabuleiro[ponto.linha][ponto.coluna];
}

void inicializarTabuleiro(Tabuleiro *tabuleiro) {
  int linha, coluna;
  for (linha = 0; linha < 8; linha++) {
    for (coluna = 0; coluna < 8; coluna++) {
      if (linha < 3) {
        if ((linha + coluna) % 2 == 0) {
          tabuleiro->tabuleiro[linha][coluna] = BRANCA;
        } else {
          tabuleiro->tabuleiro[linha][coluna] = VAZIO;
        }
      } else if (linha > 4) {
        if ((linha + coluna) % 2 == 0) {
          tabuleiro->tabuleiro[linha][coluna] = PRETA;
        } else {
          tabuleiro->tabuleiro[linha][coluna] = VAZIO;
        }
      } else {
        tabuleiro->tabuleiro[linha][coluna] = VAZIO;
      }
    }
  }
  tabuleiro->numPecasBrancas = 12;
  tabuleiro->numPecasPretas = 12;
  tabuleiro->numDamasPretas = 0;
  tabuleiro->numDamasBrancas = 0;
}

void imprimeTabuleiro(Tabuleiro *tabuleiro) {
  printf("    ");
  for (int i = 0; i < 8; i++) {
    printf("0%d ", i);
  }
  printf("\n");
  for (int i = 0; i <= 16; i++) {
    if (i % 2 == 0) {
      printf("    ");
      for (int j = 0; j < 23; j++) {
        printf("-");
      }
      printf(" ");
    } else {
      printf("0%d ", i / 2);
      for (int j = 0; j < 8; j++) {
        printf("|");
        TipoPeca p = tabuleiro->tabuleiro[i / 2][j];
        if (p == PRETA) {
          printf("PP");
        } else if (p == DAMA_PRETA) {
          printf("DP");
        } else if (p == BRANCA) {
          printf("PB");
        } else if (p == DAMA_BRANCA) {
          printf("DB");
        } else if (p == VAZIO) {
          printf("  ");
        }
      }
      printf("| 0%d", i / 2);
    }
    printf("\n");
  }

  printf("    ");
  for (int i = 0; i < 8; i++) {

    printf("0%d ", i);
  }

  printf("\n\nPP = Peca Preta / DP = Dama Preta / PB = Peca Branca / DB = Dama Branca\n\n\n");
}

void gerarMovimentosValidosPeca(int linha, int coluna, Movimento *movimentos, int *qtdMovimentos, Tabuleiro *tabuleiro) {

  TipoPeca peca = pegarPeca(tabuleiro, linha, coluna);

  if (peca == BRANCA || peca == PRETA) {
    int linhaMudar = peca == PRETA ? -1 : 1;
    int novaLinha = linha + linhaMudar;
    if (novaLinha >= 0 && novaLinha < 8) {
      int novaColuna = coluna + 1;
      TipoPeca pecaNova = pegarPeca(tabuleiro, novaLinha, novaColuna);

      if (novaColuna < 8 && pecaNova == VAZIO) {
        movimentos[*qtdMovimentos].origem.coluna = coluna;
        movimentos[*qtdMovimentos].origem.linha = linha;
        movimentos[*qtdMovimentos].destino.coluna = novaColuna;
        movimentos[*qtdMovimentos].destino.linha = novaLinha;

        (*qtdMovimentos)++;

        Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
        if (newMovimentos == NULL) {
          // Handle memory allocation failure
        } else {
          movimentos = newMovimentos;
        }
      }
      novaColuna = coluna - 1;
      pecaNova = pegarPeca(tabuleiro, novaLinha, novaColuna);

      if (novaColuna >= 0 && pecaNova == VAZIO) {
        movimentos[*qtdMovimentos].origem.coluna = coluna;
        movimentos[*qtdMovimentos].origem.linha = linha;
        movimentos[*qtdMovimentos].destino.coluna = novaColuna;
        movimentos[*qtdMovimentos].destino.linha = novaLinha;

        (*qtdMovimentos)++;

        Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
        if (newMovimentos == NULL) {
          // Handle memory allocation failure
        } else {
          movimentos = newMovimentos;
        }
      }
    }
  } else {
    int novaLinha = linha + 1;
    if (novaLinha < 8) {
      int novaColuna = coluna + 1;
      TipoPeca pecaNova = pegarPeca(tabuleiro, novaLinha, novaColuna);

      if (novaColuna < 8 && pecaNova == VAZIO) {
        movimentos[*qtdMovimentos].origem.coluna = coluna;
        movimentos[*qtdMovimentos].origem.linha = linha;
        movimentos[*qtdMovimentos].destino.coluna = novaColuna;
        movimentos[*qtdMovimentos].destino.linha = novaLinha;

        (*qtdMovimentos)++;

        Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
        if (newMovimentos == NULL) {
          // Handle memory allocation failure
        } else {
          movimentos = newMovimentos;
        }
      }
      novaColuna = coluna - 1;
      pecaNova = pegarPeca(tabuleiro, novaLinha, novaColuna);

      if (novaColuna >= 0 && pecaNova == VAZIO) {
        movimentos[*qtdMovimentos].origem.coluna = coluna;
        movimentos[*qtdMovimentos].origem.linha = linha;
        movimentos[*qtdMovimentos].destino.coluna = novaColuna;
        movimentos[*qtdMovimentos].destino.linha = novaLinha;

        (*qtdMovimentos)++;

        Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
        if (newMovimentos == NULL) {
          // Handle memory allocation failure
        } else {
          movimentos = newMovimentos;
        }
      }
    }
    novaLinha = linha - 1;
    if (novaLinha >= 0) {
      int novaColuna = coluna + 1;
      TipoPeca pecaNova = pegarPeca(tabuleiro, novaLinha, novaColuna);

      if (novaColuna < 8 && pecaNova == VAZIO) {
        movimentos[*qtdMovimentos].origem.coluna = coluna;
        movimentos[*qtdMovimentos].origem.linha = linha;
        movimentos[*qtdMovimentos].destino.coluna = novaColuna;
        movimentos[*qtdMovimentos].destino.linha = novaLinha;

        (*qtdMovimentos)++;

        Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
        if (newMovimentos == NULL) {
          // Handle memory allocation failure
        } else {
          movimentos = newMovimentos;
        }
      }
      novaColuna = coluna - 1;
      pecaNova = pegarPeca(tabuleiro, novaLinha, novaColuna);

      if (novaColuna >= 0 && pecaNova == VAZIO) {
        movimentos[*qtdMovimentos].origem.coluna = coluna;
        movimentos[*qtdMovimentos].origem.linha = linha;
        movimentos[*qtdMovimentos].destino.coluna = novaColuna;
        movimentos[*qtdMovimentos].destino.linha = novaLinha;

        (*qtdMovimentos)++;

        Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
        if (newMovimentos == NULL) {
          // Handle memory allocation failure
        } else {
          movimentos = newMovimentos;
        }
      }
    }
  }
}

Ponto pegarPecaMeio(Movimento *movimento) {
  Ponto meio;
  meio.linha = (movimento->origem.linha + movimento->destino.linha) / 2;
  meio.coluna = (movimento->origem.coluna + movimento->destino.coluna) / 2;
  return meio;
}

int ehPecaOponente(TipoPeca peca, Lado lado) {
  if (lado == LADO_PRETO) {
    return peca == BRANCA || peca == DAMA_BRANCA;
  }
  return peca == PRETA || peca == DAMA_PRETA;
}

int ehPecaPropria(Tabuleiro *tabuleiro, int linha, int coluna, Lado lado) {
  TipoPeca peca = pegarPeca(tabuleiro, linha, coluna);
  if (lado == LADO_PRETO) {
    return peca == PRETA || peca == DAMA_PRETA;
  }
  return peca == BRANCA || peca == DAMA_BRANCA;
}

void gerarMovimentosPuloPeca(int linha, int coluna, Lado lado, Movimento *movimentos, int *qtdMovimentos, Tabuleiro *tabuleiro) {
  Ponto inicio;
  inicio.linha = linha;
  inicio.coluna = coluna;
  Ponto *movimentosPossiveis = malloc(sizeof(Ponto) * 1);
  int qtdMovimentosPossiveis = 0;
  TipoPeca peca = pegarPeca(tabuleiro, linha, coluna);

  if (lado == LADO_BRANCO && peca == BRANCA) {

    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha + 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna + 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));
    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha + 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna - 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));

  } else if (lado == LADO_PRETO && peca == PRETA) {
    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha - 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna + 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));

    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha - 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna - 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));

  } else if (peca == DAMA_PRETA || peca == DAMA_BRANCA) {
    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha + 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna + 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));

    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha + 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna - 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));

    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha - 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna + 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));

    movimentosPossiveis[qtdMovimentosPossiveis].linha = linha - 2;
    movimentosPossiveis[qtdMovimentosPossiveis].coluna = coluna - 2;
    qtdMovimentosPossiveis++;
    movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Ponto) * (qtdMovimentosPossiveis + 1));
  }
  int n = qtdMovimentosPossiveis;
  for (int i = 0; i < n; i++) {
    Ponto temp = movimentosPossiveis[i];

    Movimento movimento;
    movimento.origem = inicio;
    movimento.destino = temp;

    Ponto meio = pegarPecaMeio(&movimento);
    TipoPeca pecaTemp = pegarPeca(tabuleiro, temp.linha, temp.coluna);
    TipoPeca pecaMeio = pegarPecaPonto(tabuleiro, meio);

    int ehPecaOponenteBool = ehPecaOponente(lado, pecaMeio);

    if (temp.linha >= 0 && temp.linha < 8 && temp.coluna >= 0 && temp.coluna < 8 && pecaTemp == VAZIO && ehPecaOponenteBool && !ehPecaPropria(tabuleiro, meio.linha, meio.coluna, lado)) {
      // printf("meio: (%d, %d) = %d\n", meio.linha, meio.coluna, pecaMeio);
      // printf("qtdMovimentos: %d\n", *qtdMovimentos);
      movimentos[*qtdMovimentos] = movimento;
      (*qtdMovimentos)++;
      Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
      if (newMovimentos == NULL) {
        // Handle memory allocation failure
      } else {
        movimentos = newMovimentos;
      }
    }
  }
  free(movimentosPossiveis);
  return;
}

int movimentoEhIgual(Movimento movimento1, Movimento movimento2) {
  return movimento1.origem.linha == movimento2.origem.linha && movimento1.origem.coluna == movimento2.origem.coluna && movimento1.destino.linha == movimento2.destino.linha && movimento1.destino.coluna == movimento2.destino.coluna;
}

Movimento *gerarTodosMovimentos(Lado lado, Movimento *movimentos, int *qtdMovimentos, Tabuleiro *tabuleiro) {
  TipoPeca normal = lado == LADO_PRETO ? PRETA : BRANCA;
  TipoPeca dama = lado == LADO_PRETO ? DAMA_PRETA : DAMA_BRANCA;

  movimentos = malloc(sizeof(Movimento) * 1);

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      TipoPeca peca = pegarPeca(tabuleiro, i, j);
      if (peca == normal || peca == dama) {
        gerarMovimentosPuloPeca(i, j, lado, movimentos, qtdMovimentos, tabuleiro);
      }
    }
  }
  if (*qtdMovimentos == 0) {
    for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
        TipoPeca peca = pegarPeca(tabuleiro, i, j);
        if (peca == normal || peca == dama) {
          gerarMovimentosValidosPeca(i, j, movimentos, qtdMovimentos, tabuleiro);
        }
      }
    }
  }
  // printf("qtdMovimentos : %d\n", *qtdMovimentos);
  Movimento *newMovimentos = realloc(movimentos, sizeof(Movimento) * (*qtdMovimentos + 1));
  if (newMovimentos == NULL) {
    // Handle memory allocation failure
  } else {
    movimentos = newMovimentos;
  }
  return movimentos;
}

Status fazerMovimentoTabuleiro(Tabuleiro *tabuleiro, Movimento movimento, Lado lado) {
  if (movimento.origem.linha == -1) {
    return GAME_OVER;
  }

  Ponto origem = movimento.origem;
  int linhaOrigem = origem.linha;
  int colunaOrigem = origem.coluna;
  Ponto destino = movimento.destino;
  int linhaDestino = destino.linha;
  int colunaDestino = destino.coluna;

  TipoPeca peca_atual = pegarPeca(tabuleiro, linhaOrigem, colunaOrigem);

  if (!ehPecaPropria(tabuleiro, linhaOrigem, colunaOrigem, lado) || pegarPeca(tabuleiro, linhaOrigem, colunaOrigem) == VAZIO) {
    return FAIL_PECA_INVALIDA;
  }

  Movimento *movimentosPossiveis = NULL;
  int qtdMovimentosPossiveis = 0;

  movimentosPossiveis = gerarTodosMovimentos(lado, movimentosPossiveis, &qtdMovimentosPossiveis, tabuleiro);

  Movimento *newMovimentosPossiveis = realloc(movimentosPossiveis, sizeof(Movimento) * (qtdMovimentosPossiveis));
  if (newMovimentosPossiveis == NULL) {
    // Handle memory allocation failure
    printf("ERRO\n");
  } else {
    movimentosPossiveis = newMovimentosPossiveis;
  }

  // movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Movimento) * (qtdMovimentosPossiveis));

  int movimento_na_lista = 0;
  int n = qtdMovimentosPossiveis;
  for (int i = 0; i < n && !movimento_na_lista; i++) {
    movimento_na_lista = movimentoEhIgual(movimento, movimentosPossiveis[i]);
  }

  if (movimento_na_lista) {
    if (linhaOrigem + 1 == linhaDestino || linhaOrigem - 1 == linhaDestino) {
      tabuleiro->tabuleiro[linhaOrigem][colunaOrigem] = VAZIO;
      tabuleiro->tabuleiro[linhaDestino][colunaDestino] = peca_atual;
    } else {
      tabuleiro->tabuleiro[linhaOrigem][colunaOrigem] = VAZIO;
      tabuleiro->tabuleiro[linhaDestino][colunaDestino] = peca_atual;
      Ponto meio = pegarPecaMeio(&movimento);
      TipoPeca peca_meio = pegarPecaPonto(tabuleiro, meio);
      if (peca_meio == PRETA) {
        tabuleiro->numPecasPretas--;
      } else if (peca_meio == BRANCA) {
        tabuleiro->numPecasBrancas--;
      } else if (peca_meio == DAMA_PRETA) {
        tabuleiro->numPecasPretas--;
        tabuleiro->numDamasPretas--;
      } else if (peca_meio == DAMA_BRANCA) {
        tabuleiro->numPecasBrancas--;
        tabuleiro->numDamasBrancas--;
      }
      tabuleiro->tabuleiro[meio.linha][meio.coluna] = VAZIO;
    }
    if (linhaDestino == 0 && lado == LADO_PRETO) {
      tabuleiro->tabuleiro[linhaDestino][colunaDestino] = DAMA_PRETA;
      tabuleiro->numDamasPretas++;
    } else if (linhaDestino == 7 && lado == LADO_BRANCO) {
      tabuleiro->tabuleiro[linhaDestino][colunaDestino] = DAMA_BRANCA;
      tabuleiro->numDamasBrancas++;
    }
    return COMPLETADO;
  } else {
    return FAIL_DESTINO_INVALIDO;
  }
}

Tabuleiro clonaTabuleiro(Tabuleiro *tabuleiro) {
  Tabuleiro novoTabuleiro;
  novoTabuleiro.numDamasBrancas = tabuleiro->numDamasBrancas;
  novoTabuleiro.numDamasPretas = tabuleiro->numDamasPretas;
  novoTabuleiro.numPecasBrancas = tabuleiro->numPecasBrancas;
  novoTabuleiro.numPecasPretas = tabuleiro->numPecasPretas;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      novoTabuleiro.tabuleiro[i][j] = tabuleiro->tabuleiro[i][j];
    }
  }
  return novoTabuleiro;
}

Status fazerMovimentoJogador(Tabuleiro *tabuleiro, Movimento movimento, Jogador jogador) {
  return fazerMovimentoTabuleiro(tabuleiro, movimento, jogador.lado);
}

double pegarHeuristica(Tabuleiro *tabuleiro, Lado lado) {
  double peso_dama = 1.5;
  if (lado == LADO_PRETO) {
    return (tabuleiro->numPecasPretas + tabuleiro->numDamasPretas * peso_dama) - (tabuleiro->numPecasBrancas + tabuleiro->numDamasBrancas * peso_dama);
  }
  return (tabuleiro->numPecasBrancas + tabuleiro->numDamasBrancas * peso_dama) - (tabuleiro->numPecasPretas + tabuleiro->numDamasPretas * peso_dama);
}

Lado trocaLado(Lado lado) {
  if (lado == LADO_BRANCO) {
    return LADO_PRETO;
  }
  return LADO_BRANCO;
}

double minimax(Tabuleiro *tabuleiro, Lado lado, int maximizando, int profundidade, int alpha, int beta) {
  if (profundidade == 0) {
    return pegarHeuristica(tabuleiro, lado);
  }

  Movimento *movimentosPossiveis = NULL;
  int qtdMovimentosPossiveis = 0;
  movimentosPossiveis = gerarTodosMovimentos(lado, movimentosPossiveis, &qtdMovimentosPossiveis, tabuleiro);
  Movimento *newMovimentosPossiveis = realloc(movimentosPossiveis, sizeof(Movimento) * (qtdMovimentosPossiveis));
  if (newMovimentosPossiveis == NULL) {
    // Handle memory allocation failure
  } else {
    movimentosPossiveis = newMovimentosPossiveis;
  }
  // movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Movimento) * (qtdMovimentosPossiveis));
  int numMovimentos = qtdMovimentosPossiveis;
  int inicial = 0;
  Tabuleiro tempTabuleiro;

  if (maximizando) {
    for (int i = 0; i < numMovimentos; i++) {
      tempTabuleiro = clonaTabuleiro(tabuleiro);
      Status status = fazerMovimentoTabuleiro(&tempTabuleiro, movimentosPossiveis[i], lado);
      if (status != COMPLETADO && status != GAME_OVER) {
        continue;
      }
      int resultado = minimax(&tempTabuleiro, trocaLado(lado), 0, profundidade - 1, alpha, beta);
      inicial = max(resultado, inicial);
      alpha = max(inicial, alpha);
      if (alpha >= beta) {
        break;
      }
    }
  } else {
    inicial = 1000;
    for (int i = 0; i < numMovimentos; i++) {
      tempTabuleiro = clonaTabuleiro(tabuleiro);
      Status status = fazerMovimentoTabuleiro(&tempTabuleiro, movimentosPossiveis[i], lado);
      if (status != COMPLETADO && status != GAME_OVER) {
        continue;
      }
      int resultado = minimax(&tempTabuleiro, trocaLado(lado), 1, profundidade - 1, alpha, beta);
      inicial = min(resultado, inicial);
      beta = min(beta, inicial);
      if (alpha >= beta) {
        break;
      }
    }
  }
  return inicial;
}

Movimento minimaxStart(Tabuleiro *tabuleiro, Lado lado, int maximizando, int profundidade) {
  int alpha = -1000, beta = 1000;
  Movimento *movimentosPossiveis = NULL;
  int qtdMovimentosPossiveis = 0;
  movimentosPossiveis = gerarTodosMovimentos(lado, movimentosPossiveis, &qtdMovimentosPossiveis, tabuleiro);
  printf("qtdMovimentosPossiveis: %d\n", qtdMovimentosPossiveis);
  Movimento *newMovimentosPossiveis = realloc(movimentosPossiveis, sizeof(Movimento) * (qtdMovimentosPossiveis));
  if (newMovimentosPossiveis == NULL) {
    // Handle memory allocation failure
  } else {
    movimentosPossiveis = newMovimentosPossiveis;
  }

  // movimentosPossiveis = realloc(movimentosPossiveis, sizeof(Movimento) * (qtdMovimentosPossiveis));

  double *heuristicas = malloc(sizeof(double) * qtdMovimentosPossiveis);
  int qtdHeuristicas = 0;
  printf("qtdHeuristicas: %d\n", qtdHeuristicas);

  if (qtdMovimentosPossiveis == 0) {
    printf("NENHUM MOVIMENTO POSSIVEL\n");
    Movimento movimento;
    movimento.origem.linha = -1;
    movimento.origem.coluna = -1;
    movimento.destino.linha = -1;
    movimento.destino.coluna = -1;
    return movimento;
  }

  Tabuleiro tempTabuleiro;
  int numMovimentos = qtdMovimentosPossiveis;
  for (int i = 0; i < numMovimentos; i++) {
    tempTabuleiro = clonaTabuleiro(tabuleiro);
    // printf("Movimento: (%d, %d) para (%d, %d)\n", movimentosPossiveis[i].origem.linha, movimentosPossiveis[i].origem.coluna, movimentosPossiveis[i].destino.linha, movimentosPossiveis[i].destino.coluna);
    fazerMovimentoTabuleiro(&tempTabuleiro, movimentosPossiveis[i], lado);
    printf("\nentrou\n");
    heuristicas[i] = minimax(&tempTabuleiro, trocaLado(lado), !maximizando, profundidade - 1, alpha, beta);
    qtdHeuristicas++;
  }

  double max_heuristica = -1000;
  for (int i = 0; i < qtdHeuristicas; i++) {
    if (heuristicas[i] >= max_heuristica) {
      max_heuristica = heuristicas[i];
    }
  }

  for (int i = 0; i < qtdHeuristicas; i++) {
    if (heuristicas[i] < max_heuristica) {
      heuristicas[i] = -1000;
      movimentosPossiveis[i].origem.linha = -1;
      movimentosPossiveis[i].origem.coluna = -1;
      movimentosPossiveis[i].destino.linha = -1;
      movimentosPossiveis[i].destino.coluna = -1;
      i--;
      qtdHeuristicas--;
      qtdMovimentosPossiveis--;
    }
  }

  numMovimentos = qtdMovimentosPossiveis;
  int random = rand() % numMovimentos;
  Movimento movimento = movimentosPossiveis[random];
  printf("Movimento final: (%d, %d) para (%d, %d)\n", movimento.origem.linha, movimento.origem.coluna, movimento.destino.linha, movimento.destino.coluna);
  printf("\n");
  return movimento;
}

Status fazerMovimentoIA(Tabuleiro *tabuleiro, IA ia) {
  Movimento movimento = minimaxStart(tabuleiro, ia.lado, 1, ia.profundidade);
  imprimeMovimento(movimento);
  printf("\n");
  return fazerMovimentoTabuleiro(tabuleiro, movimento, ia.lado);
}

void jogar() {
  char peca;
  Lado jogador, cpu;
  do {
    printf("Escolha sua peca (P/B): ");
    scanf("%c", &peca);
  } while (peca != 'P' && peca != 'p' && peca != 'B' && peca != 'b');
  clear();
  if (peca == 'B' || peca == 'b') {
    jogador = LADO_BRANCO;
    cpu = LADO_PRETO;
  } else {
    jogador = LADO_PRETO;
    cpu = LADO_BRANCO;
  }

  Jogador jogador1;
  jogador1.lado = jogador;
  IA jogador2;
  jogador2.lado = cpu;

  int vez = 1;
  Tabuleiro tabuleiro;
  inicializarTabuleiro(&tabuleiro);
  Status msg = COMPLETADO;
  while (msg != GAME_OVER) {
    clear();
    printf("\n\n");
    if (vez) {
      printf("Sua vez\n");
    } else {
      printf("Vez do computador\n");
    }
    printf("\n\n");
    imprimeTabuleiro(&tabuleiro);
    if (!vez) {
      system("pause");
    }
    if (vez) {
      Movimento *movimento_jogador = NULL;
      int qtd_movimentos_jogador = 0;
      movimento_jogador = gerarTodosMovimentos(jogador1.lado, movimento_jogador, &qtd_movimentos_jogador, &tabuleiro);
      movimento_jogador = realloc(movimento_jogador, sizeof(Movimento) * qtd_movimentos_jogador);
      // printf("(%d, %d) para (%d, %d)\n", movimento_jogador[1].origem.linha, movimento_jogador[1].origem.coluna, movimento_jogador[1].destino.linha, movimento_jogador[1].destino.coluna);
      if (qtd_movimentos_jogador == 0) {
        printf("Nao ha movimentos possiveis, voce perdeu\n");
        break;
      } else {
        printf("Movimentos possiveis (precisa comer quando possivel): \n");
        // printf("qtde movimentos: %d\n", qtd_movimentos_jogador);
        for (int i = 0; i < qtd_movimentos_jogador; i++) {
          imprimeMovimento(movimento_jogador[i]);
        }
        printf("\n");
      }
      Movimento movimento;
      do {
        printf("Escolha seu movimento (x1 y1 x2 y2): ");
        scanf("%d %d %d %d", &movimento.origem.linha, &movimento.origem.coluna, &movimento.destino.linha, &movimento.destino.coluna);
        msg = fazerMovimentoJogador(&tabuleiro, movimento, jogador1);
        if (msg == FAIL_DESTINO_INVALIDO) {
          printf("Movimento invalido\n");
        } else if (msg == FAIL_PECA_INVALIDA) {
          printf("Peca de origem invalida\n");
        }
      } while (msg != COMPLETADO && msg != GAME_OVER);
      free(movimento_jogador);
      vez = !vez;
    } else {
      Movimento *movimentos_ia = NULL;
      int qtd_movimentos_ia = 0;
      movimentos_ia = gerarTodosMovimentos(jogador2.lado, movimentos_ia, &qtd_movimentos_ia, &tabuleiro);
      movimentos_ia = realloc(movimentos_ia, sizeof(Movimento) * qtd_movimentos_ia);
      if (qtd_movimentos_ia == 0) {
        printf("Nao ha movimentos possiveis, voce ganhou\n");
        break;
      }

      msg = fazerMovimentoIA(&tabuleiro, jogador2);
      free(movimentos_ia);
      vez = !vez;
    }
  }
}

int main() {
  srand(time(NULL));
  clear();
  printf("Jogo de Damas\n\n");
  printf("Desenvolvido por:\n");
  printf("  - Lucas Simoes Ferreira\n");
  printf("  - Thiago Scholl\n");
  char jogar_novamente;
  do {
    jogar();
    do {

      printf("Deseja jogar novamente? (S/N): ");
      scanf("%c", &jogar_novamente);
    } while (jogar_novamente != 'S' && jogar_novamente != 's' && jogar_novamente != 'N' && jogar_novamente != 'n');
  } while (jogar_novamente == 'S' || jogar_novamente == 's');
  return 0;
}