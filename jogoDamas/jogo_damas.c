#include "/usr/local/opt/libomp/include/omp.h"
// #include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 8
#define MAX_THREADS 4
#define MAX_MOVES 100
#define PLAYER1 1
#define PLAYER2 2
#define INT_MIN -2147483648
#define INT_MAX 2147483647

typedef struct {
  int **tabuleiro;
  int currentPlayer;
} Game;

typedef struct {
  int x;
  int y;
  int newX;
  int newY;
} Move;

typedef struct {
  int nPieces;
} Player;

int menu();
Move getMove();
void playGame(Game game);
void verifyWinner(Game game);
void makeMove(Game game, Move move);
int verifyValidMove(Game game, Move move);
void inicializaTabuleiro(int **tabuleiro, int n);
void imprimeTabuleiro(int **tabuleiro, int n);
int **criaTabuleiro(int n);
Game initGame();
int evaluateGame(Game game);
Move searchBestMove(Game game);
int minimax(Game game, int depth, int isMaximizingPlayer, int alpha, int beta, int currentPlayer, int opponentPlayer);
int verifyValidMoveNoPrint(Game game, Move move);


int **criaTabuleiro(int n) {
  int **tabuleiro = (int **)malloc(n * sizeof(int *));
  for (int i = 0; i < n; i++) {
    tabuleiro[i] = (int *)malloc(n * sizeof(int));
  }
  return tabuleiro;
}

void imprimeTabuleiro(int **tabuleiro, int n) {
  printf("\n");
  for (int i = 0; i < n; i++) {
    printf("  %d", i);
  }
  printf("\n");
  for (int i = 0; i < n; i++) {
    printf("%d ", i);
    for (int j = 0; j < n; j++) {
      printf(" %d ", tabuleiro[i][j]);
    }
    printf("\n");
  }
}

void inicializaTabuleiro(int **tabuleiro, int n) {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < n / 2; j++) {
      if (i % 2 == 0) {
        tabuleiro[i][2 * j] = 0;
        tabuleiro[i][2 * j + 1] = (i < 3) ? PLAYER1 : (i > 4) ? PLAYER2
                                                              : 0;
      } else {
        tabuleiro[i][2 * j] = (i < 3) ? PLAYER1 : (i > 4) ? PLAYER2
                                                          : 0;
        tabuleiro[i][2 * j + 1] = 0;
      }
    }
  }
}

int evaluateGame(Game game) {
  int currentPlayer = game.currentPlayer;
  int opponentPlayer = (currentPlayer == PLAYER1) ? PLAYER2 : PLAYER1;

  int player1Score = 0;
  int player2Score = 0;

  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (game.tabuleiro[i][j] == PLAYER1) {
        player1Score += 5; // Give a base score for each piece

        // Give higher scores for pieces closer to the opponent's side
        player1Score += (currentPlayer == PLAYER1) ? SIZE - i : i;
      } else if (game.tabuleiro[i][j] == PLAYER2) {
        player2Score += 5; // Give a base score for each piece

        // Give higher scores for pieces closer to the opponent's side
        player2Score += (currentPlayer == PLAYER2) ? i : SIZE - i;
      }
    }
  }

  // Return the difference between the scores for the current player and the opponent player
  if (currentPlayer == PLAYER1) {
    return player1Score - player2Score;
  } else {
    return player2Score - player1Score;
  }
}

Move searchBestMove(Game game) {
  int maxScore = INT_MIN;
  Move bestMove;
  int currentPlayer = PLAYER2;
  int opponentPlayer = PLAYER1;

#pragma omp parallel for collapse(2) num_threads(MAX_THREADS)
  for (int i = 0; i < SIZE; i++) {
    for (int j = 0; j < SIZE; j++) {
      if (game.tabuleiro[i][j] == currentPlayer) {
        for (int dx = -1; dx <= 1; dx++) {
          for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 || dy == 0) continue;
            int newX = i + dx;
            int newY = j + dy;
            Move move = {i, j, newX, newY};

            if (verifyValidMoveNoPrint(game, move) == 1) {
            //   printf("Jogador %d moveu a peca de [%d][%d] para [%d][%d]\n", game.currentPlayer, move.x, move.y, move.newX, move.newY);
              Game newGame;
              newGame.tabuleiro = criaTabuleiro(SIZE);
              newGame.currentPlayer = currentPlayer;
              memcpy(newGame.tabuleiro[0], game.tabuleiro[0], SIZE * SIZE * sizeof(int));
              makeMove(newGame, move);
              int score = minimax(newGame, 1, 0, INT_MIN, INT_MAX, currentPlayer, opponentPlayer);
              free(newGame.tabuleiro);
#pragma omp critical
              {
                if (score > maxScore) {
                  maxScore = score;
                  bestMove = move;
                }
              }
            }
          }
        }
      }
    }
  }

  return bestMove;
}

int minimax(Game game, int depth, int isMaximizingPlayer, int alpha, int beta, int currentPlayer, int opponentPlayer) {
  if (depth == 0) {
    return evaluateGame(game);
  }

  if (isMaximizingPlayer) {
    int maxScore = INT_MIN;
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (game.tabuleiro[i][j] == currentPlayer) {
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              if (dx == 0 || dy == 0) continue;
              int newX = i + dx;
              int newY = j + dy;
              Move move = {i, j, newX, newY};

              if (verifyValidMoveNoPrint(game, move) == 1) {
                Game newGame;
                newGame.tabuleiro = criaTabuleiro(SIZE);
                newGame.currentPlayer = currentPlayer;
                makeMove(newGame, move);
                int score = minimax(newGame, depth - 1, 0, alpha, beta, currentPlayer, opponentPlayer);
                maxScore = (score > maxScore) ? score : maxScore;
                alpha = (alpha > maxScore) ? alpha : maxScore;
                if (beta <= alpha) break;
              }
            }
          }
        }
      }
    }
    return maxScore;
  } else {
    int minScore = INT_MAX;
    for (int i = 0; i < SIZE; i++) {
      for (int j = 0; j < SIZE; j++) {
        if (game.tabuleiro[i][j] == opponentPlayer) {
          for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
              if (dx == 0 || dy == 0) continue;
              int newX = i + dx;
              int newY = j + dy;
              Move move = {i, j, newX, newY};

              if (verifyValidMoveNoPrint(game, move) == 1) {
                Game newGame;
                newGame.tabuleiro = criaTabuleiro(SIZE);
                newGame.currentPlayer = currentPlayer;
                makeMove(newGame, move);
                int score = minimax(newGame, depth - 1, 1, alpha, beta, currentPlayer, opponentPlayer);
                minScore = (score < minScore) ? score : minScore;
                beta = (beta < minScore) ? beta : minScore;
                if (beta <= alpha) break;
              }
            }
          }
        }
      }
    }
    return minScore;
  }
}

Game initGame() {
  Game game;
  game.tabuleiro = criaTabuleiro(SIZE);
  inicializaTabuleiro(game.tabuleiro, SIZE);
  game.currentPlayer = PLAYER1;
  return game;
}

int verifyValidMove(Game game, Move move) {
  int currentPlayer = game.currentPlayer;

  // Check if the move is inside the matrix bounds
  if (move.x < 0 || move.x >= SIZE || move.y < 0 || move.y >= SIZE ||
      move.newX < 0 || move.newX >= SIZE || move.newY < 0 || move.newY >= SIZE) {
    printf("Jogada invalida: Posicao fora dos limites do tabuleiro!\n");
    return 0;
  }

  // Check if the piece is moving forward (player 1) or backward (player 2)
  if ((currentPlayer == PLAYER1 && move.newX < move.x) || (currentPlayer == PLAYER2 && move.newX > move.x)) {
    printf("Jogada invalida: Peca deve mover-se apenas para frente!\n");
    return 0;
  }

  // Check if the destination position is empty
  if (game.tabuleiro[move.newX][move.newY] == 1 && game.currentPlayer == 1) {
    printf("Jogada invalida: A posicao de destino ja possui uma peca sua!\n");
    return 0;
  }
  if (game.tabuleiro[move.newX][move.newY] == 2 && game.currentPlayer == 2) {
    printf("Jogada invalida: A posicao de destino ja possui uma peca sua!\n");
    return 0;
  }

  // Check if the move is diagonal and in front
  if (abs(move.x - move.newX) != 1 || abs(move.y - move.newY) != 1) {
    // Check if the move is capturing an opponent's piece
    if (abs(move.x - move.newX) == 2 && abs(move.y - move.newY) == 2) {
      int capturedX = (move.x + move.newX) / 2;
      int capturedY = (move.y + move.newY) / 2;
      int capturedPiece = game.tabuleiro[capturedX][capturedY];

      if (capturedPiece == currentPlayer || capturedPiece == 0) {
        printf("Jogada invalida: Voce deve capturar a peca adversaria!\n");
        return 0;
      } else {
        game.tabuleiro[capturedX][capturedY] = 0; // Remove the captured piece
        return 1;
      }
    }
    printf("Jogada invalida: Movimento deve ser diagonal!\n");
    return 0;
  }

  // All checks passed, move is valid
  return 1;
}

int verifyValidMoveNoPrint(Game game, Move move){
    int currentPlayer = game.currentPlayer;

  // Check if the move is inside the matrix bounds
  if (move.x < 0 || move.x >= SIZE || move.y < 0 || move.y >= SIZE ||
      move.newX < 0 || move.newX >= SIZE || move.newY < 0 || move.newY >= SIZE) {
    
    return 0;
  }

  // Check if the piece is moving forward (player 1) or backward (player 2)
  if ((currentPlayer == PLAYER1 && move.newX < move.x) || (currentPlayer == PLAYER2 && move.newX > move.x)) {
    
    return 0;
  }

  // Check if the destination position is empty
  if (game.tabuleiro[move.newX][move.newY] == 1 && game.currentPlayer == 1) {
    
    return 0;
  }
  if (game.tabuleiro[move.newX][move.newY] == 2 && game.currentPlayer == 2) {
    
    return 0;
  }

  // Check if the move is diagonal and in front
  if (abs(move.x - move.newX) != 1 || abs(move.y - move.newY) != 1) {
    // Check if the move is capturing an opponent's piece
    if (abs(move.x - move.newX) == 2 && abs(move.y - move.newY) == 2) {
      int capturedX = (move.x + move.newX) / 2;
      int capturedY = (move.y + move.newY) / 2;
      int capturedPiece = game.tabuleiro[capturedX][capturedY];

      if (capturedPiece == currentPlayer || capturedPiece == 0) {
        
        return 0;
      } else {
        game.tabuleiro[capturedX][capturedY] = 0; // Remove the captured piece
        return 1;
      }
    }
    
    return 0;
  }

  // All checks passed, move is valid
  return 1;
}

void makeMove(Game game, Move move) {
  game.tabuleiro[move.newX][move.newY] = game.tabuleiro[move.x][move.y];
  game.tabuleiro[move.x][move.y] = 0;
  if (abs(move.x - move.newX) == 2 && abs(move.y - move.newY) == 2) {
    // If the move captures an opponent's piece, remove it
    int capturedX = (move.x + move.newX) / 2;
    int capturedY = (move.y + move.newY) / 2;
    game.tabuleiro[capturedX][capturedY] = 0;

    // Check if the piece can move one more house further
    int furtherX = move.newX + (move.newX - move.x);
    int furtherY = move.newY + (move.newY - move.y);
    Move furtherMove = {move.newX, move.newY, furtherX, furtherY};
    if (verifyValidMoveNoPrint(game, furtherMove) == 1) {
      // If valid, make the move one more house further
      game.tabuleiro[furtherX][furtherY] = game.tabuleiro[move.newX][move.newY];
      game.tabuleiro[move.newX][move.newY] = 0;
    }else{
      printf("Voce nao pode capturar a peca!\n");
    }
  }
}


void verifyWinner(Game game) {
  int i, j;
  int player1 = 0;
  int player2 = 0;
  for (i = 0; i < SIZE; i++) {
    for (j = 0; j < SIZE / 2; j++) {
      if (game.tabuleiro[i][2 * j] == PLAYER1) {
        player1++;
      } else if (game.tabuleiro[i][2 * j + 1] == PLAYER1) {
        player1++;
      }
      if (game.tabuleiro[i][2 * j] == PLAYER2) {
        player2++;
      } else if (game.tabuleiro[i][2 * j + 1] == PLAYER2) {
        player2++;
      }
    }
  }
  if (player1 == 0) {
    printf("Jogador 2 venceu!\n");
    exit(0);
  }
  if (player2 == 0) {
    printf("Jogador 1 venceu!\n");
    exit(0);
  }
}

void playGame(Game game) {
  int i = 0;
  int opcao, isValid;
  while (i < MAX_MOVES) {
    if (game.currentPlayer == PLAYER2) {
      imprimeTabuleiro(game.tabuleiro, SIZE);
      Move bestMove = searchBestMove(game);
        printf("Jogada do computador: "
            "de [%d][%d] para [%d][%d]\n",
            bestMove.x, bestMove.y, bestMove.newX, bestMove.newY);
        makeMove(game, bestMove);
        game.currentPlayer = PLAYER1;
        verifyWinner(game);
        i++;
    } else {
      opcao = menu();
      switch (opcao) {
      case 1:
        printf("Jogador %d\n", game.currentPlayer);
        Move move;
        move = getMove();
        isValid = verifyValidMove(game, move);
        if (isValid == 0) {
          break;
        } else {
          makeMove(game, move);
          if (game.currentPlayer == 1) {
            game.currentPlayer = PLAYER2;
          } else {
            game.currentPlayer = PLAYER1;
          }
          verifyWinner(game);
          i++;
          break;
        }
      case 2:
        imprimeTabuleiro(game.tabuleiro, SIZE);
        break;
      case 3:
        exit(0);
        break;
      default:
        printf("Opcao invalida!\n");
        break;
      }
    }
  }
}

Move getMove() {
  Move move;
  printf("Digite a linha da peca que deseja mover: ");
  scanf("%d", &move.x);
  printf("Digite a coluna da peca que deseja mover: ");
  scanf("%d", &move.y);
  printf("Digite a linha para onde deseja mover a peca: ");
  scanf("%d", &move.newX);
  printf("Digite a coluna para onde deseja mover a peca: ");
  scanf("%d", &move.newY);
  return move;
}

int menu() {
  int opcao;
  printf("--- Jogo de Damas ---\n");
  printf("1 - Jogar\n");
  printf("2 - Imprimir tabuleiro\n");
  printf("3 - Sair\n");
  printf("Digite a opcao desejada: ");
  scanf("%d", &opcao);
  return opcao;
}

int main(int argc, char *argv[]) {
  Game game = initGame();
  playGame(game);
  return 0;
}