#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// #include <omp.h>
#include <limits.h>
#include <omp.h>
#include <stdbool.h>
// #include "/usr/local/opt/libomp/include/omp.h"

// Define constantes para as direções das diagonais
#define UP_LEFT 0
#define UP_RIGHT 1
#define DOWN_LEFT 2
#define DOWN_RIGHT 3
#define BOARD_SIZE 8

// Estrutura para representar uma posição no tabuleiro
typedef struct {
  int row;
  int col;
} Position;

// Estrutura para representar uma peça
typedef struct {
  int color; // 0 para peças do jogador, 1 para peças do oponente
  Position position;
} Piece;

// Estrutura para representar o tabuleiro
typedef struct {
  Piece pieces[BOARD_SIZE][BOARD_SIZE];
} Board;

// declaraçao de funçoes
bool is_valid_position(int row, int col);
void generate_valid_moves(const Board *board, const Piece *piece, Position *valid_moves, int *num_moves);
int evaluate_board(const Board *board, int player_color);
int minimax(const Board *board, int depth, int max_depth, int player_color);
void make_computer_move(Board *board, int player_color, int difficulty);
void print_board(const Board *board);
void initialize_board(Board *board);
int has_valid_moves(const Board *board, int player_color);
void make_move(Board *board, int row_from, int col_from, int row_to, int col_to);

// Função para verificar se uma posição está dentro dos limites do tabuleiro
bool is_valid_position(int row, int col) {
  return (row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE);
}

// Função para gerar os movimentos válidos para uma peça
void generate_valid_moves(const Board *board, const Piece *piece, Position *valid_moves, int *num_moves) {
  *num_moves = 0;
  int row = piece->position.row;
  int col = piece->position.col;
  int color = piece->color;

  // Vetores que representam as direções das diagonais
  int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

  // Verifica cada uma das direções
  for (int dir = 0; dir < 4; ++dir) {
    int dr = directions[dir][0];
    int dc = directions[dir][1];
    int new_row = row + dr;
    int new_col = col + dc;

    // Verifica se a nova posição está dentro do tabuleiro
    if (is_valid_position(new_row, new_col)) {
      // Verifica se a casa está vazia
      if (board->pieces[new_row][new_col].color == -1) {
        // Se estiver vazia, adiciona a posição aos movimentos válidos
        valid_moves[*num_moves].row = new_row;
        valid_moves[*num_moves].col = new_col;
        (*num_moves)++;
      }
      // Verifica se é possível fazer uma captura pulando sobre a peça do oponente
      else if (board->pieces[new_row][new_col].color != color) {
        int jump_row = new_row + dr;
        int jump_col = new_col + dc;

        // Verifica se a posição após a captura está dentro do tabuleiro e vazia
        if (is_valid_position(jump_row, jump_col) && board->pieces[jump_row][jump_col].color == -1) {
          // Adiciona a posição após a captura aos movimentos válidos
          valid_moves[*num_moves].row = jump_row;
          valid_moves[*num_moves].col = jump_col;
          (*num_moves)++;
        }
      }
    }
  }
}

// Função heurística para avaliar a pontuação de um determinado estado do tabuleiro
int evaluate_board(const Board *board, int player_color) {
  int score = 0;

  for (int row = 0; row < BOARD_SIZE; ++row) {
    for (int col = 0; col < BOARD_SIZE; ++col) {
      const Piece *piece = &board->pieces[row][col];

      // Verifica se a posição contém uma peça do jogador atual
      if (piece->color == player_color) {
        // Atribui uma pontuação base para a peça
        int piece_value = 1;

        // Bônus para peças promovidas (damas)
        if ((player_color == 0 && row == 0) || (player_color == 1 && row == BOARD_SIZE - 1)) {
          piece_value += 3;
        }

        // Bônus para peças em posições avançadas
        int crown_distance = (player_color == 0) ? BOARD_SIZE - 1 - row : row;
        piece_value += crown_distance;

        // Adiciona a pontuação da peça ao placar total
        score += piece_value;
      }
    }
  }

  return score;
}

// Função MinMax com Alpha-Beta Pruning para encontrar a melhor jogada
int minimax(const Board *board, int depth, int max_depth, int player_color) {
  if (depth == max_depth) {
    return evaluate_board(board, player_color);
  }

  int best_score;

  if (player_color == 0) {
    // Jogador 0 (máquina) está maximizando
    best_score = INT_MIN;

#pragma omp parallel for reduction(max : best_score)
    for (int row = 0; row < BOARD_SIZE; ++row) {
      for (int col = 0; col < BOARD_SIZE; ++col) {
        const Piece *piece = &board->pieces[row][col];

        // Verifica se a posição contém uma peça do jogador atual
        if (piece->color == player_color) {
          Position valid_moves[BOARD_SIZE * BOARD_SIZE];
          int num_moves = 0;

          // Gera os movimentos válidos para a peça
          generate_valid_moves(board, piece, valid_moves, &num_moves);

          // Realiza a busca para cada movimento válido
          for (int i = 0; i < num_moves; ++i) {
            // printf("Movimento %d %d %d %d\n", row, col, valid_moves[i].row, valid_moves[i].col);
            Board new_board = *board;
            make_move(&new_board, row, col, valid_moves[i].row, valid_moves[i].col);

            // Chama recursivamente a função minimax para o próximo nível da árvore
            int score = minimax(&new_board, depth + 1, max_depth, 1 - player_color);
            best_score = (score > best_score) ? score : best_score;
          }
        }
      }
    }
  } else {
    // Jogador 1 (humano) está minimizando
    best_score = INT_MAX;

#pragma omp parallel for reduction(min : best_score)
    for (int row = 0; row < BOARD_SIZE; ++row) {
      for (int col = 0; col < BOARD_SIZE; ++col) {
        const Piece *piece = &board->pieces[row][col];

        // Verifica se a posição contém uma peça do jogador atual
        if (piece->color == player_color) {
          Position valid_moves[BOARD_SIZE * BOARD_SIZE];
          int num_moves = 0;

          // Gera os movimentos válidos para a peça
          generate_valid_moves(board, piece, valid_moves, &num_moves);

          // Realiza a busca para cada movimento válido
          for (int i = 0; i < num_moves; ++i) {
            printf("Movimento %d %d %d %d\n", row, col, valid_moves[i].row, valid_moves[i].col);
            Board new_board = *board;
            make_move(&new_board, row, col, valid_moves[i].row, valid_moves[i].col);

            // Chama recursivamente a função minimax para o próximo nível da árvore
            int score = minimax(&new_board, depth + 1, max_depth, 1 - player_color);
            best_score = (score < best_score) ? score : best_score;
          }
        }
      }
    }
  }

  return best_score;
}

// Função para fazer a jogada do computador
void make_computer_move(Board *board, int player_color, int difficulty) {
  // Variável para armazenar as melhores jogadas encontradas
  Position best_moves[BOARD_SIZE * BOARD_SIZE];
  int num_best_moves = 0;

  // Ajuste a profundidade máxima da busca com base na dificuldade
  int max_depth = 2 + difficulty;

  // Inicializa a semente aleatória
  srand(time(NULL));

  int best_score;
  if (player_color == 0) {
    // Jogador 0 (máquina) está maximizando
    best_score = INT_MIN;

    // Percorre todas as peças do jogador atual para encontrar as melhores jogadas
    for (int row = 0; row < BOARD_SIZE; ++row) {
      for (int col = 0; col < BOARD_SIZE; ++col) {
        const Piece *piece = &board->pieces[row][col];

        // Verifica se a posição contém uma peça do jogador atual
        if (piece->color == player_color) {
          Position valid_moves[BOARD_SIZE * BOARD_SIZE];
          int num_moves = 0;

          // Gera os movimentos válidos para a peça
          generate_valid_moves(board, piece, valid_moves, &num_moves);

          // Realiza a busca para cada movimento válido
          for (int i = 0; i < num_moves; ++i) {
            printf("Movimento %d %d %d %d\n", row, col, valid_moves[i].row, valid_moves[i].col);
            Board new_board = *board;
            make_move(&new_board, row, col, valid_moves[i].row, valid_moves[i].col);

            // Chama a função minimax para avaliar a pontuação do estado do tabuleiro
            int score = minimax(&new_board, 1, max_depth, 1 - player_color);

            // Se a pontuação for maior que a melhor pontuação atual, reinicia o vetor de melhores jogadas
            if (score > best_score) {
              best_score = score;
              num_best_moves = 0;
              best_moves[num_best_moves++] = valid_moves[i];
            } else if (score == best_score) {
              // Se a pontuação for igual à melhor pontuação atual, adiciona a jogada ao vetor de melhores jogadas
              best_moves[num_best_moves++] = valid_moves[i];
            }
          }
        }
      }
    }
  } else {
    // Jogador 1 (humano) está minimizando
    best_score = INT_MAX;

    // Percorre todas as peças do jogador atual para encontrar as melhores jogadas
    for (int row = 0; row < BOARD_SIZE; ++row) {
      for (int col = 0; col < BOARD_SIZE; ++col) {
        const Piece *piece = &board->pieces[row][col];

        // Verifica se a posição contém uma peça do jogador atual
        if (piece->color == player_color) {
          Position valid_moves[BOARD_SIZE * BOARD_SIZE];
          int num_moves = 0;

          // Gera os movimentos válidos para a peça
          generate_valid_moves(board, piece, valid_moves, &num_moves);

          // Realiza a busca para cada movimento válido
          for (int i = 0; i < num_moves; ++i) {
            Board new_board = *board;
            printf("Movimento %d %d %d %d\n", row, col, valid_moves[i].row, valid_moves[i].col);
            make_move(&new_board, row, col, valid_moves[i].row, valid_moves[i].col);

            // Chama a função minimax para avaliar a pontuação do estado do tabuleiro
            int score = minimax(&new_board, 1, max_depth, 1 - player_color);

            // Se a pontuação for menor que a melhor pontuação atual, reinicia o vetor de melhores jogadas
            if (score < best_score) {
              best_score = score;
              num_best_moves = 0;
              best_moves[num_best_moves++] = valid_moves[i];
            } else if (score == best_score) {
              // Se a pontuação for igual à melhor pontuação atual, adiciona a jogada ao vetor de melhores jogadas
              best_moves[num_best_moves++] = valid_moves[i];
            }
          }
        }
      }
    }
  }

  // Escolhe aleatoriamente uma das melhores jogadas encontradas
  int random_index = rand() % num_best_moves;
  Position chosen_move = best_moves[random_index];
  printf("Melhor jogada: %d %d %d %d\n", chosen_move.row, chosen_move.col, chosen_move.row, chosen_move.col);

  // Executa a jogada escolhida
  Piece *piece = &board->pieces[chosen_move.row][chosen_move.col];
  printf("Jogada da máquina: %d %d %d %d\n", piece->position.row, piece->position.col, chosen_move.row, chosen_move.col);
  make_move(board, piece->position.row, piece->position.col, chosen_move.row, chosen_move.col);
}

// Função para exibir o tabuleiro
void print_board(const Board *board) {
  // Caracteres para representar as peças no tabuleiro
  const char pieces_chars[2] = {'O', 'X'};
  const char empty_char = '-';

  printf("   0 1 2 3 4 5 6 7\n");
  for (int row = 0; row < BOARD_SIZE; ++row) {
    printf("%d ", row);
    for (int col = 0; col < BOARD_SIZE; ++col) {
      const Piece *piece = &board->pieces[row][col];
      char piece_char;

      if (piece->color == -1) {
        // Se a posição estiver vazia, imprime o caracter vazio
        piece_char = empty_char;
      } else {
        // Se a posição tiver uma peça, imprime o caracter correspondente ao jogador
        piece_char = pieces_chars[piece->color];
      }

      printf("%c ", piece_char);
    }
    printf("\n");
  }
}

// make move

// Função para inicializar o tabuleiro com as peças nas posições iniciais
void initialize_board(Board *board) {
  // Inicializa todas as posições como vazias
  for (int row = 0; row < BOARD_SIZE; ++row) {
    for (int col = 0; col < BOARD_SIZE; ++col) {
      board->pieces[row][col].color = -1;
    }
  }

  // Preenche as peças nas posições iniciais
  for (int row = 0; row < BOARD_SIZE; ++row) {
    for (int col = 0; col < BOARD_SIZE; ++col) {
      // Coloca as peças do jogador 0 (máquina) nas duas primeiras linhas
      if (row < 2) {
        if ((row + col) % 2 == 0) {
          board->pieces[row][col].color = 0;
        }
      }
      // Coloca as peças do jogador 1 (humano) nas duas últimas linhas
      else if (row >= BOARD_SIZE - 2) {
        if ((row + col) % 2 == 0) {
          board->pieces[row][col].color = 1;
        }
      }
    }
  }
}

// Função para verificar se o jogador de determinada cor tem movimentos possíveis
int has_valid_moves(const Board *board, int player_color) {
  for (int row = 0; row < BOARD_SIZE; ++row) {
    for (int col = 0; col < BOARD_SIZE; ++col) {
      const Piece *piece = &board->pieces[row][col];
      if (piece->color == player_color) {
        Position valid_moves[BOARD_SIZE * BOARD_SIZE];
        int num_moves = 0;

        // Gera os movimentos válidos para a peça
        generate_valid_moves(board, piece, valid_moves, &num_moves);
        printf("Jogador %d tem %d movimentos válidos.\n", player_color, num_moves);
        printf("Peça %d %d tem %d movimentos válidos.\n", row, col, num_moves);

        // Se houver algum movimento válido, o jogador ainda tem movimentos possíveis
        if (num_moves > 0) {
          return 1;
        }
      }
    }
  }

  // Se chegou aqui, o jogador não tem mais movimentos possíveis
  return 0;
}

// Função para fazer um movimento no tabuleiro
void make_move(Board *board, int row_from, int col_from, int row_to, int col_to) {
  // Verifica se a posição de origem está dentro dos limites do tabuleiro
  if (row_from < 0 || row_from >= BOARD_SIZE || col_from < 0 || col_from >= BOARD_SIZE) {
    printf("Erro: posição de origem inválida.\n");
    return;
  }

  // Verifica se a posição de destino está dentro dos limites do tabuleiro
  if (row_to < 0 || row_to >= BOARD_SIZE || col_to < 0 || col_to >= BOARD_SIZE) {
    printf("Erro: posição de destino inválida.\n");
    return;
  }

  // Obtém a peça que será movida
  Piece *piece = &board->pieces[row_from][col_from];

  // Verifica se a posição de origem contém uma peça válida
  if (piece->color == -1) {
    printf("Erro: não há peça na posição de origem.\n");
    return;
  }

  // Verifica se a posição de destino está vazia
  if (board->pieces[row_to][col_to].color != -1) {
    printf("Erro: posição de destino ocupada por outra peça.\n");
    return;
  }

  // Realiza o movimento atualizando as posições da peça
  board->pieces[row_to][col_to] = *piece;
  board->pieces[row_from][col_from].color = -1;
  board->pieces[row_to][col_to].position.row = row_to;
  board->pieces[row_to][col_to].position.col = col_to;
}

// Função principal do jogo
int main() {
  // Inicializa o tabuleiro
  Board board;
  initialize_board(&board);

  // Define o jogador humano como jogador 1 (humano) e a máquina como jogador 0 (máquina)
  int human_player = 1;
  int machine_player = 0;

  // Define a cor do jogador atual para o jogador humano no início do jogo
  int player_color = human_player;

  // Variável para indicar se o jogo terminou
  int game_over = 0;

  // Loop principal do jogo
  while (!game_over) {
    // Exibe o tabuleiro
    print_board(&board);

    if (player_color == human_player) {
      // Jogada do jogador humano
      // Implemente a lógica para obter a jogada do jogador humano aqui
      // ...

      // Aqui, você precisaria chamar uma função para obter a jogada do jogador humano e atualizar o tabuleiro com a jogada feita.
      // Algo como:
      int row_from, col_from, row_to, col_to;
      printf("Digite a posição da peça que deseja mover (linha coluna): ");
      scanf("%d %d", &row_from, &col_from);
      printf("Digite a posição para onde deseja mover a peça (linha coluna): ");
      scanf("%d %d", &row_to, &col_to);
      make_move(&board, row_from, col_from, row_to, col_to);

    } else {
      // Jogada do computador
      make_computer_move(&board, player_color, 2); // Nível de dificuldade 2 (ajuste conforme necessário)
    }

    // Verifique se há vencedor ou empate
    // Implemente a lógica para verificar o fim do jogo aqui
    // ...
    // Por exemplo, você pode verificar se alguma das cores de jogador não tem mais peças no tabuleiro ou se não há mais movimentos possíveis.
    // Se o jogo terminar, você pode definir a variável game_over como 1 para sair do loop principal.
    // ...

    // Aqui, você também pode adicionar a lógica para exibir uma mensagem indicando que o jogo terminou e quem foi o vencedor.
    // Verifique se há vencedor ou empate
    int has_machine_moves = has_valid_moves(&board, machine_player);
    int has_human_moves = has_valid_moves(&board, human_player);

    // Se a máquina não tem mais movimentos possíveis, o jogador humano venceu
    if (!has_machine_moves) {
      printf("Jogador humano venceu!\n");
      game_over = 1;
    }

    // Se o jogador humano não tem mais movimentos possíveis, a máquina venceu
    if (!has_human_moves) {
      printf("Máquina venceu!\n");
      game_over = 1;
    }

    // Se nenhum dos jogadores tem mais movimentos possíveis, é um empate
    if (!has_machine_moves && !has_human_moves) {
      printf("Empate!\n");
      game_over = 1;
    }

    // Alterne a cor do jogador para a próxima rodada
    player_color = !player_color;
  }

  return 0;
}
