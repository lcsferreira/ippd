#include <vector>
#include <string>
using namespace std;

enum Peca{
  VAZIO,
  PRETA,
  BRANCA,
  DAMA_PRETA,
  DAMA_BRANCA
};

enum Lado{
  LADO_PRETO,
  LADO_BRANCO
};

enum Status{
  COMPLETO,
  FAIL_PECA_INVALIDA,
  FAIL_DESTINO_INVALIDO,
  GAME_OVER
};

struct Ponto{
  int x;
  int y;
};

struct Jogada{
  Ponto origem;
  Ponto destino;
};

class Falha {};

class Jogo{
  private:
    Peca** tabuleiro;
    int num_pecas_pretas;
    int num_pecas_brancas;
    int num_damas_pretas;
    int num_damas_brancas;
  public:
    int getNumPecasPretas();
    int getNumPecasBrancas();
    int getNumDamasPretas();
    int getNumDamasBrancas();
    Jogo();
    Jogo(Peca** tabuleiro);
    bool verificaEhPecaOponente(Lado lado, Peca);
    bool verificaEhPecaAliada(Lado lado, Peca);
    bool verificaJogadaIgual(Jogada jogada1, Jogada jogada2);
    Jogo* clonaJogo();
    Peca getPeca(Ponto ponto);
    Peca getPeca(int x, int y);
    Ponto getPecaMeio(Jogada jogada);
    Status fazJogada(Jogada jogada, Lado lado);
    void imprimeTabuleiro();
    void geraTodosOsMovimentos(Lado lado, vector<Jogada> &jogadas);
    void geraJogadasValidas(Ponto ponto,Lado lado, vector<Jogada> &jogadas);
    void geraJogadasDePuloValidas(Ponto ponto,Lado lado, vector<Jogada> &jogadas);
    void montarTabuleiro();
};

class Jogador {
  protected:
    Lado lado;
    string nome;
  public:
    Jogador(Lado lado, string nome);
    virtual Status fazJogada(Jogada jogada, Jogo jogo);
    Lado getLado();
    string getNome();
};

class IA: public Jogador {
  private:
    int profundidade;
  public:
    IA(Lado lado, string nome): Jogador(lado, nome){
      profundidade = 3;
    };
    Status fazJogada(Jogo jogo);
    Jogada inicioMinimax(Jogo jogo, Lado lado, bool maximizando);
    double minimax(Jogo jogo, Lado lado, int profundidade, bool maximizando, int alpha, int beta);
    double calcHeuristica(Jogo jogo);
    Lado trocaLado(Lado lado);
};