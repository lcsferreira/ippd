enum TipoPeca {
  VAZIO,
  PRETA,
  BRANCA,
  DAMA_PRETA,
  DAMA_BRANCA
} typedef TipoPeca;

enum Lado {
  LADO_PRETO,
  LADO_BRANCO
} typedef Lado;

enum Status {
  COMPLETADO,
  FAIL_PECA_INVALIDA,
  FAIL_DESTINO_INVALIDO,
  GAME_OVER
} typedef Status;

struct Ponto {
  int linha;
  int coluna;
} typedef Ponto;

struct Movimento {
  Ponto origem;
  Ponto destino;
} typedef Movimento;

struct Tabuleiro {
  TipoPeca tabuleiro[8][8];
  int numPecasPretas;
  int numPecasBrancas;
  int numDamasPretas;
  int numDamasBrancas;
} typedef Tabuleiro;

struct Jogador {
  Lado lado;
} typedef Jogador;

struct IA {
  Lado lado;
  int profundidade;
} typedef IA;