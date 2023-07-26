#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "/usr/local/opt/libomp/include/omp.h"

#define SIZE 8
#define MAX_THREADS 4
#define MAX_MOVES 100
#define PLAYER1 1
#define PLAYER2 2

typedef struct{
    int **tabuleiro;
    int currentPlayer; 
} Game;

typedef struct{
    int x;
    int y;
    int newX;
    int newY;
} Move;

typedef struct{
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

int **criaTabuleiro(int n) {
    int **tabuleiro = (int **) malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        tabuleiro[i] = (int *) malloc(n * sizeof(int));
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
                tabuleiro[i][2 * j + 1] = (i < 3) ? PLAYER1 : (i > 4) ? PLAYER2 : 0;
            } else {
                tabuleiro[i][2 * j] = (i < 3) ? PLAYER1 : (i > 4) ? PLAYER2 : 0;
                tabuleiro[i][2 * j + 1] = 0;
            }
        }
    }
}

Move searchBestMove(Game game){
    // implement a minimax algorithm to search for the best move using omp
    // return the best move
}

Game initGame(){
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


void makeMove(Game game, Move move) {
    game.tabuleiro[move.newX][move.newY] = game.tabuleiro[move.x][move.y];
    game.tabuleiro[move.x][move.y] = 0;
    if(move.x - move.newX == 2 || move.y - move.newY == 2){
        game.tabuleiro[move.x - 1][move.y - 1] = 0;
    }
    if(move.x - move.newX == -2 || move.y - move.newY == -2){
        game.tabuleiro[move.x + 1][move.y + 1] = 0;
    }
    printf("Jogador %d moveu a peca de [%d][%d] para [%d][%d]\n", game.currentPlayer, move.x, move.y, move.newX, move.newY);
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
    if(player1 == 0){
        printf("Jogador 2 venceu!\n");
        exit(0);
    }
    if(player2 == 0){
        printf("Jogador 1 venceu!\n");
        exit(0);
    }
}

void playGame(Game game) {
    int i = 0;
    int opcao;
    while (i < MAX_MOVES) {
        opcao = menu();
        switch (opcao) {
            case 1:
                printf("Jogador %d\n", game.currentPlayer);
                Move move;
                int isValid;
                move = getMove();
                isValid = verifyValidMove(game, move);
                if(isValid == 0){
                    break;
                }else{
                    makeMove(game, move);
                    if(game.currentPlayer == 1){
                        game.currentPlayer = PLAYER2;
                    }else{
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

Move getMove(){
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

int menu(){
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