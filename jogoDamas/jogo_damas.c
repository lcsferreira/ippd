#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define BOARD_SIZE 8

#define EMPTY 0
#define PLAYER_PIECE 1
#define AI_PIECE 2

#define PLAYER_PIECE_VALUE 1
#define AI_PIECE_VALUE -1

typedef struct {
    int board[BOARD_SIZE][BOARD_SIZE];
} GameState;

int evaluateBoard(GameState state) {
    int playerScore = 0;
    int aiScore = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == PLAYER_PIECE) {
                playerScore += PLAYER_PIECE_VALUE;
            } else if (state.board[i][j] == AI_PIECE) {
                aiScore += AI_PIECE_VALUE;
            }
        }
    }

    return playerScore + aiScore;
}

int isMoveValid(GameState state, int row, int col, int newRow, int newCol, int player) {
    // Implemente a lógica para verificar se um movimento é válido
    // ...
    return 0;
}

int getNumPieces(GameState state, int player) {
    int numPieces = 0;

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == player) {
                numPieces++;
            }
        }
    }

    return numPieces;
}

void makeMove(GameState* state, int row, int col, int newRow, int newCol, int player) {
    // Realizar o movimento
    state->board[newRow][newCol] = player;
    state->board[row][col] = EMPTY;

    // Verificar se o movimento capturou alguma peça do oponente
    int captureRow = (row + newRow) / 2;
    int captureCol = (col + newCol) / 2;

    if (abs(newRow - row) == 2 && abs(newCol - col) == 2 && state->board[captureRow][captureCol] != EMPTY) {
        // Capturou uma peça do oponente
        state->board[captureRow][captureCol] = EMPTY;
        // Atualizar outras informações relacionadas à captura, se necessário
        // ...
    }

    // Verificar se o movimento promoveu uma peça do jogador
    if ((player == PLAYER_PIECE && newRow == BOARD_SIZE - 1) || (player == AI_PIECE && newRow == 0)) {
        // Peça do jogador promovida a dama
        state->board[newRow][newCol] = player + 2; // Marcar como dama
        // Atualizar outras informações relacionadas à promoção, se necessário
        // ...
    }
}


void undoMove(GameState* state, int row, int col, int newRow, int newCol, int player) {

    // Desfazer o movimento
    state->board[newRow][newCol] = EMPTY;
    state->board[row][col] = player;

    // Verificar se o movimento capturou alguma peça do oponente
    int captureRow = (row + newRow) / 2;
    int captureCol = (col + newCol) / 2;

    if (abs(newRow - row) == 2 && abs(newCol - col) == 2 && state->board[captureRow][captureCol] != EMPTY) {
        // Desfazer a captura da peça do oponente
        state->board[captureRow][captureCol] = (player == PLAYER_PIECE) ? AI_PIECE : PLAYER_PIECE;
        // Atualizar outras informações relacionadas à captura, se necessário
        // ...
    }

    // Verificar se o movimento promoveu uma peça do jogador

    if ((player == PLAYER_PIECE && newRow == BOARD_SIZE - 1) || (player == AI_PIECE && newRow == 0)) {
        // Peça do jogador promovida a dama
        state->board[newRow][newCol] = player;
        // Atualizar outras informações relacionadas à promoção, se necessário
        // ...
    }

}

int evaluateMove(GameState state, int player, int depth, int alpha, int beta) {
    if (depth == 0) {
        return evaluateBoard(state);
    }

    int bestScore = (player == AI_PIECE) ? -1000 : 1000;

    int numPieces = getNumPieces(state, player);

    // Gerar todos os movimentos possíveis para o jogador atual
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == player) {
                // Gerar todos os movimentos possíveis para uma peça
                int numMoves = generateMoves(state, i, j, player); // Função para gerar movimentos possíveis

                for (int k = 0; k < numMoves; k++) {
                    int newRow, newCol; // Variáveis para armazenar as coordenadas do movimento

                    // Obter as coordenadas do movimento atual
                    getMoveCoordinates(state, i, j, k, &newRow, &newCol); // Função para obter as coordenadas do movimento

                    // Realizar o movimento
                    makeMove(&state, i, j, newRow, newCol, player);

                    int score = evaluateMove(state, (player == PLAYER_PIECE) ? AI_PIECE : PLAYER_PIECE, depth - 1, alpha, beta);

                    if (player == AI_PIECE) {
                        bestScore = (score > bestScore) ? score : bestScore;
                        alpha = (bestScore > alpha) ? bestScore : alpha;
                    } else {
                        bestScore = (score < bestScore) ? score : bestScore;
                        beta = (bestScore < beta) ? bestScore : beta;
                    }

                    // Desfazer o movimento
                    undoMove(&state, i, j, newRow, newCol, player);

                    // Realizar a poda alfa-beta
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
        }
    }

    return bestScore;
}

GameState findBestMove(GameState state, int player, int depth) {
    GameState bestMove;
    int bestScore = -1000;

    int numPieces = getNumPieces(state, player);

    // Gerar todos os movimentos possíveis para o jogador atual
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == player) {
                // Gerar todos os movimentos possíveis para uma peça
                int numMoves = generateMoves(state, i, j, player); // Função para gerar movimentos possíveis

                for (int k = 0; k < numMoves; k++) {
                    int newRow, newCol; // Variáveis para armazenar as coordenadas do movimento

                    // Obter as coordenadas do movimento atual
                    getMoveCoordinates(state, i, j, k, &newRow, &newCol); // Função para obter as coordenadas do movimento

                    // Realizar o movimento
                    makeMove(&state, i, j, newRow, newCol, player);

                    int score = evaluateMove(state, (player == PLAYER_PIECE) ? AI_PIECE : PLAYER_PIECE, depth - 1, -1000, 1000);

                    if (score > bestScore) {
                        bestScore = score;
                        bestMove = state;
                    }

                    // Desfazer o movimento
                    undoMove(&state, i, j, newRow, newCol, player);
                }
            }
        }
    }

    return bestMove;
}


int isGameOver(GameState state) {
    // Verificar se o jogador humano venceu
    int playerWin = 1;
    // Percorrer o tabuleiro e verificar se ainda existem peças da máquina
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == AI_PIECE || state.board[i][j] == AI_PIECE + 2) {
                playerWin = 0;
                break;
            }
        }
        if (!playerWin) {
            break;
        }
    }
    if (playerWin) {
        printf("Parabéns! Você venceu!\n");
        return 1;
    }

    // Verificar se a máquina venceu
    int aiWin = 1;
    // Percorrer o tabuleiro e verificar se ainda existem peças do jogador humano
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == PLAYER_PIECE || state.board[i][j] == PLAYER_PIECE + 2) {
                aiWin = 0;
                break;
            }
        }
        if (!aiWin) {
            break;
        }
    }
    if (aiWin) {
        printf("Você perdeu! A máquina venceu!\n");
        return 1;
    }

    // Verificar se houve empate
    int numPlayerPieces = 0;
    int numAiPieces = 0;
    // Percorrer o tabuleiro e contar o número de peças do jogador humano e da máquina
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (state.board[i][j] == PLAYER_PIECE || state.board[i][j] == PLAYER_PIECE + 2) {
                numPlayerPieces++;
            } else if (state.board[i][j] == AI_PIECE || state.board[i][j] == AI_PIECE + 2) {
                numAiPieces++;
            }
        }
    }
    if (numPlayerPieces == 0 && numAiPieces == 0) {
        printf("Empate! O jogo terminou em empate!\n");
        return 1;
    }

    // O jogo não terminou
    return 0;
}


void printBoard(GameState state) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%d ", state.board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

void playGame() {
    GameState state;

    // Inicializar o estado do jogo, como o tabuleiro e outras informações necessárias
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            state.board[i][j] = EMPTY;
        }
    }

    state.board[1][0] = AI_PIECE;
    state.board[3][2] = AI_PIECE;
    state.board[5][4] = PLAYER_PIECE;
    state.board[7][6] = PLAYER_PIECE;

    printBoard(state);

    // Loop principal do jogo
    while (1) {
        // Jogador humano faz a jogada
        int row, col, newRow, newCol;
        printf("Digite a posição atual da peça (linha coluna): ");
        scanf("%d %d", &row, &col);
        printf("Digite a posição para onde mover a peça (linha coluna): ");
        scanf("%d %d", &newRow, &newCol);

        if (isMoveValid(state, row, col, newRow, newCol, PLAYER_PIECE)) {
            makeMove(&state, row, col, newRow, newCol, PLAYER_PIECE);
        } else {
            printf("Movimento inválido! Tente novamente.\n");
            continue;
        }

        printBoard(state);

        // Verificar se o jogo terminou após a jogada do jogador humano
        // ...
        if (isGameOver(state)) {
            break;
        }


        // Máquina encontra a melhor jogada usando o algoritmo Minimax com poda alfa-beta
        GameState bestMove = findBestMove(state, AI_PIECE, 5);  // Busca limitada de 5 iterações

        // Máquina faz a melhor jogada encontrada
        // ...
        makeMove(&state, row, col, newRow, newCol, AI_PIECE);

        // Verificar se o jogo terminou após a jogada da máquina
        // ...
        if (isGameOver(state)) {
            break;
        }

        printBoard(state);

        // Repetir o loop para a próxima rodada
        // ...
    }
}

int main() {
    // Inicializar OpenMP
    omp_set_num_threads(4);

    // Iniciar o jogo
    playGame();

    return 0;
}
