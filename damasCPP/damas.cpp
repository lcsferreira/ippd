#include <algorithm>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <mpi.h>
#include "damas.h"
using namespace std;

// para executar o programa, basta executar o comando "g++ -fopenmp damas.cpp -o damas" e depois "./damas"

void printJogada(Jogada jogada);

Jogo::Jogo(){
  montarTabuleiro();
}

Jogo::Jogo(Peca** tabuleiro_novo){
  num_pecas_brancas = 0;
  num_pecas_pretas = 0;
  num_damas_brancas = 0;
  num_damas_pretas = 0;
  tabuleiro = tabuleiro_novo;

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      Ponto p = {i, j};
      Peca peca = getPeca(p);
      if(peca == PRETA || peca == DAMA_PRETA){
        num_pecas_pretas++;
        if(peca == DAMA_PRETA){
          num_damas_pretas++;
        }
      }else if(peca == BRANCA || peca == DAMA_BRANCA){
        num_pecas_brancas++;
        if(peca == DAMA_BRANCA){
          num_damas_brancas++;
        }
      }
    }
  }
}

Peca Jogo::getPeca(Ponto ponto){
  return tabuleiro[ponto.x][ponto.y];
}

Peca Jogo::getPeca(int x, int y){
  return tabuleiro[x][y];
}

void Jogo::montarTabuleiro(){
  num_pecas_brancas = 12;
  num_pecas_pretas = 12;
  num_damas_brancas = 0;
  num_damas_pretas = 0;
  tabuleiro = new Peca*[8];
  for(int i = 0; i < 8; i++){
    tabuleiro[i] = new Peca[8];
  }
  for(int i = 0; i < 8; i++){
    int primeiro = 0;
    if(i % 2 == 0){
      primeiro = 1;
    }
    Peca peca =VAZIO;
    if(i <= 2){
      peca = BRANCA;
    }else if(i >= 5){
      peca = PRETA;
    }
    for (int j = primeiro; j < 8; j+=2){
      tabuleiro[i][j] = peca;
    }
  }
  for (int i = 0; i< 8; i++){
    for (int j = 0; j<8; j++){
      if(!(tabuleiro[i][j] == BRANCA || tabuleiro[i][j] == PRETA) || (i > 2 && i < 5)){
        tabuleiro[i][j] = VAZIO;
      }
    }
  }
}

string Jogador::getNome(){
  return nome;
}

int Jogo::getNumPecasBrancas(){
  return num_pecas_brancas;
}

int Jogo::getNumPecasPretas(){
  return num_pecas_pretas;
}

int Jogo::getNumDamasBrancas(){
  return num_damas_brancas;
}

int Jogo::getNumDamasPretas(){
  return num_damas_pretas;
}

void Jogo::geraTodosOsMovimentos(Lado lado, vector<Jogada>& jogadas){
  Peca normal = lado == LADO_BRANCO ? BRANCA : PRETA;
  Peca dama = lado == LADO_BRANCO ? DAMA_BRANCA : DAMA_PRETA;

  #pragma omp parallel
  {
    vector<Jogada> local_jogadas;

    #pragma omp for
    for (int i=0; i<8; i++){
      for (int j=0; j<8; j++){
        Ponto p = {i,j};
        Peca peca = getPeca(p);
        if (peca == normal || peca == dama){
          geraJogadasDePuloValidas(p, lado, local_jogadas);
        }
      }
    }

    #pragma omp critical
    {
      jogadas.insert(jogadas.end(), local_jogadas.begin(), local_jogadas.end());
    }
  }
  if(jogadas.empty()){
    #pragma omp parallel
    {
      vector<Jogada> local_jogadas;

      #pragma omp for
      for (int i=0; i<8; i++){
        for (int j=0; j<8; j++){
          Ponto p = {i,j};
          Peca peca = getPeca(p);
          if (peca == normal || peca == dama){
            geraJogadasValidas(p, lado, local_jogadas);
          }
        }
      }

      #pragma omp critical
      {
        jogadas.insert(jogadas.end(), local_jogadas.begin(), local_jogadas.end());
      }
    }
  }
}

void Jogo::geraJogadasValidas(Ponto p, Lado lado, vector<Jogada> &jogadas){
  Peca peca = tabuleiro[p.x][p.y];

  if(peca == VAZIO){
    throw Falha();
  }


  if (peca == BRANCA || peca == PRETA){
    int mudarLinha = peca == PRETA ? -1 : 1;
    int novaLinha = p.x + mudarLinha;


    if(novaLinha >=0 && novaLinha < 8){
      int novaColuna = p.y + 1;

      if(novaColuna < 8 && getPeca(novaLinha, novaColuna) == VAZIO){

        jogadas.push_back({p, novaLinha, novaColuna});
      }
      novaColuna = p.y - 1;

      if(novaColuna >= 0 && getPeca(novaLinha, novaColuna) == VAZIO){


        jogadas.push_back({p, novaLinha, novaColuna});
      }
    }
  }else {
    int novaLinha = p.x + 1;
    if(novaLinha < 8){
      int novaColuna = p.y + 1;
      if(novaColuna < 8 && getPeca(novaLinha, novaColuna) == VAZIO){


        jogadas.push_back({p, novaLinha, novaColuna});
      }
      novaColuna = p.y - 1;
      if(novaColuna >= 0 && getPeca(novaLinha, novaColuna) == VAZIO){


        jogadas.push_back({p, novaLinha, novaColuna});
      }
    }
    novaLinha = p.x - 1;
    if(novaLinha >= 0){
      int novaColuna = p.y + 1;
      if(novaColuna < 8 && getPeca(novaLinha, novaColuna) == VAZIO){


        jogadas.push_back({p, novaLinha, novaColuna});
      }
      novaColuna = p.y - 1;
      if(novaColuna >= 0 && getPeca(novaLinha, novaColuna) == VAZIO){


        jogadas.push_back({p, novaLinha, novaColuna});
      }
    }
  }
}

void Jogo::geraJogadasDePuloValidas(Ponto p, Lado lado, vector<Jogada> &jogadas){
  Ponto inicio = p;
  vector<Ponto> movimentos_possiveis;
  if(lado == LADO_BRANCO && getPeca(p) == BRANCA){
    movimentos_possiveis.push_back({p.x + 2, p.y + 2});
    movimentos_possiveis.push_back({p.x + 2, p.y - 2});
  }else if(lado == LADO_PRETO && getPeca(p) == PRETA){
    movimentos_possiveis.push_back({p.x - 2, p.y + 2});
    movimentos_possiveis.push_back({p.x - 2, p.y - 2});
  }else if(getPeca(p) == DAMA_BRANCA || getPeca(p) == DAMA_PRETA){
    movimentos_possiveis.push_back({p.x + 2, p.y + 2});
    movimentos_possiveis.push_back({p.x + 2, p.y - 2});
    movimentos_possiveis.push_back({p.x - 2, p.y + 2});
    movimentos_possiveis.push_back({p.x - 2, p.y - 2});
  }
  int tam = movimentos_possiveis.size();

  // #pragma omp parallel for
  for(int i =0; i<tam; i++){
    Ponto temp = movimentos_possiveis[i];
    Jogada jogada = {inicio, temp};
    Ponto meio = getPecaMeio(jogada);

    if(temp.x >=0 && temp.x < 8 && temp.y >=0 && temp.y < 8 && getPeca(temp) == VAZIO && verificaEhPecaOponente(lado, getPeca(meio))){
      jogadas.push_back(jogada);
    }
  }
}

Ponto Jogo::getPecaMeio(Jogada jogada){
  int x = (jogada.origem.x + jogada.destino.x)/2;
  int y = (jogada.origem.y + jogada.destino.y)/2;
  Ponto p = {x, y};
  return p;
}

bool Jogo::verificaEhPecaOponente(Lado lado, Peca peca){
  if(lado == LADO_BRANCO){
    if(peca == PRETA || peca == DAMA_PRETA){
      return true;
    }
  }else{
    if(peca == BRANCA || peca == DAMA_BRANCA){
      return true;
    }
  }
  return false;
}

bool Jogo::verificaEhPecaAliada(Lado lado, Peca peca){
  if(lado == LADO_BRANCO){
    if(peca == BRANCA || peca == DAMA_BRANCA){
      return true;
    }
  }else{
    if(peca == PRETA || peca == DAMA_PRETA){
      return true;
    }
  }
  return false;
}

Status Jogo::fazJogada(Jogada jogada, Lado lado){
  if(jogada.origem.x == -1){
    return GAME_OVER;
  }

  Ponto inicio = jogada.origem;
  // int inicio_x = inicio.x;
  // int inicio_y = inicio.y;
  Ponto fim = jogada.destino;
  // int fim_x = fim.x;
  // int fim_y = fim.y;
  Peca pecaAtual = getPeca(inicio);

  if(!verificaEhPecaAliada(lado, pecaAtual)|| pecaAtual == VAZIO){
    return FAIL_PECA_INVALIDA;
  }

  vector<Jogada> jogadas_possiveis;
  geraTodosOsMovimentos(lado, jogadas_possiveis);
  bool jogada_na_lista = false;
  int tam = jogadas_possiveis.size();

  for(int i=0; i<tam && !jogada_na_lista; i++){
    jogada_na_lista = verificaJogadaIgual(jogada, jogadas_possiveis[i]);
  }

  if(jogada_na_lista){
    bool pulo = false;
    if(inicio.x + 1 == fim.x || inicio.x - 1 == fim.x){
      tabuleiro[fim.x][fim.y] = pecaAtual;
      tabuleiro[inicio.x][inicio.y] = VAZIO;
    }else{
      pulo = true;
      tabuleiro[inicio.x][inicio.y] = VAZIO;
      tabuleiro[fim.x][fim.y] = pecaAtual;
      Ponto meio = getPecaMeio(jogada);
      Peca pecaMeio = getPeca(meio);
      
      if(pecaMeio == PRETA){
        num_pecas_pretas--;
      }else if(pecaMeio == DAMA_PRETA){
        num_pecas_pretas--;
        num_damas_pretas--;
      }else if(pecaMeio == BRANCA){
        num_pecas_brancas--;
      }else if(pecaMeio == DAMA_BRANCA){
        num_pecas_brancas--;
        num_damas_brancas--;
      }
      tabuleiro[meio.x][meio.y] = VAZIO;
    }
    if(fim.x == 0 && lado == LADO_BRANCO){
      tabuleiro[fim.x][fim.y] = DAMA_BRANCA;
      num_damas_brancas++;
    }else if(fim.x == 7 && lado == LADO_PRETO){
      tabuleiro[fim.x][fim.y] = DAMA_PRETA;
      num_damas_pretas++;
    }
    return COMPLETO;
  }else{
    cout << "destino invalido" << endl;
    return FAIL_DESTINO_INVALIDO;
  }
}

bool Jogo::verificaJogadaIgual(Jogada jogada1, Jogada jogada2){
  if(jogada1.origem.x == jogada2.origem.x && jogada1.origem.y == jogada2.origem.y && jogada1.destino.x == jogada2.destino.x && jogada1.destino.y == jogada2.destino.y){
    return true;
  }
  return false;
}

Jogo* Jogo::clonaJogo(){
  Peca** tabuleiro_novo = new Peca*[8];

  for(int i = 0; i < 8; i++){
    tabuleiro_novo[i] = new Peca[8];
  }

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      tabuleiro_novo[i][j] = tabuleiro[i][j];
    }
  }
  Jogo* novo_jogo = new Jogo(tabuleiro_novo);
  return novo_jogo;
}

void Jogo::imprimeTabuleiro(){
  cout << "  0 1 2 3 4 5 6 7" << endl;
  for(int i = 0; i < 8; i++){
    cout << i << " ";
    for(int j = 0; j < 8; j++){
      if(tabuleiro[i][j] == PRETA){
        cout << "p ";
      }else if(tabuleiro[i][j] == BRANCA){
        cout << "b ";
      }else if(tabuleiro[i][j] == DAMA_PRETA){
        cout << "P ";
      }else if(tabuleiro[i][j] == DAMA_BRANCA){
        cout << "B ";
      }else{
        cout << "- ";
      }
    }
    cout << endl;
  }
  cout << "\n\n b = Peca Branca / p = Peca Preta / B = Dama Branca / P = Dama Preta" << endl;
}

Jogador::Jogador(Lado l, string n){
  lado = l;
  nome = n;
}

Status Jogador::fazJogada(Jogada jogada, Jogo jogo ){
  return jogo.fazJogada(jogada,lado);
}

Lado Jogador::getLado(){
  return lado;
}

Status IA::fazJogada(Jogo jogo){

  Jogada jogada = inicioMinimax(jogo, getLado(), true);
  cout << "Jogada do computador: ";
  printJogada(jogada);
  cout << endl;
  Status status = jogo.fazJogada(jogada, getLado());
  return status;
}

Jogada IA::inicioMinimax(Jogo jogo, Lado lado, bool maximizando){
  int alpha = -1000;
  int beta = 1000;

  vector<Jogada> jogadas_possiveis;
  jogo.geraTodosOsMovimentos(lado, jogadas_possiveis);
  vector<double> heuristicas;
  if(jogadas_possiveis.empty()){
    cout << "Sem movimentos possiveis" << endl;
    return {-1};
  }

  Jogo jogo_temp;
  int num_jogadas = jogadas_possiveis.size();
  // #pragma omp parallel for
  for(int i = 0; i<num_jogadas; i++){
    jogo_temp = *jogo.clonaJogo();
    Status status = jogo_temp.fazJogada(jogadas_possiveis[i], lado);
    heuristicas.push_back(minimax(jogo_temp, trocaLado(lado), profundidade, !maximizando, alpha, beta));
  }

  double max_heuristica = -1000;
  int num_heuristicas = heuristicas.size();
  for(int i = num_heuristicas -1; i >= 0; i--){
    if(heuristicas[i] >= max_heuristica){
      max_heuristica = heuristicas[i];
    }
  }

  // #pragma omp parallel for
  for (int i = 0; i < num_heuristicas; i++){
    if(heuristicas[i] < max_heuristica){
      heuristicas.erase(heuristicas.begin() + i);
      jogadas_possiveis.erase(jogadas_possiveis.begin() + i);
      i--;
      num_heuristicas--;
    }
  }

  num_jogadas = jogadas_possiveis.size();
  Jogada jogada_escolhida = jogadas_possiveis[rand() % num_jogadas];
  return jogada_escolhida;
}

double IA::minimax(Jogo jogo, Lado lado,int profundidade, bool maximizando , int alpha, int beta){
  if (profundidade == 0) {
    return calcHeuristica(jogo);
  }

  vector<Jogada> jogadas_possiveis;
  jogo.geraTodosOsMovimentos(lado, jogadas_possiveis);
  int tam = jogadas_possiveis.size();
  int inicial = maximizando ? -10000 : 10000; // Defina o valor inicial com base no modo (maximizando ou minimizando)
  Jogo jogo_temp;

  #pragma omp parallel for shared(jogadas_possiveis) private(jogo_temp)
  for (int i = 0; i < tam; i++) {
    jogo_temp = *jogo.clonaJogo();
    Status status = jogo_temp.fazJogada(jogadas_possiveis[i], lado);

    if (status != COMPLETO && status != GAME_OVER) {
      continue;
    }

    int resultado = minimax(jogo_temp, trocaLado(lado), !maximizando, profundidade - 1, alpha, beta);

    #pragma omp critical
    {
      // Atualize o valor inicial dentro da região crítica
      if (maximizando) {
        inicial = std::max(inicial, resultado);
        alpha = std::max(alpha, inicial);
      } else {
        inicial = std::min(inicial, resultado);
        beta = std::min(beta, inicial);
      }
    }

    // Verifique e aplique o cancelamento precocemente
    if (alpha >= beta) {
      // statement used with openmp for loop
      #pragma omp flush
    }
  }

  return inicial;
}

double IA::calcHeuristica(Jogo jogo){
  double peso_dama = 1.5;
  if(getLado() == LADO_PRETO){
    return (jogo.getNumPecasPretas() + peso_dama * jogo.getNumDamasPretas()) - (jogo.getNumPecasBrancas() - peso_dama * jogo.getNumDamasBrancas());
  }
  return (jogo.getNumPecasBrancas() + peso_dama * jogo.getNumDamasBrancas()) - (jogo.getNumPecasPretas() - peso_dama * jogo.getNumDamasPretas());
}

Lado IA::trocaLado(Lado lado){
  if(lado == LADO_BRANCO){
    return LADO_PRETO;
  }
  return LADO_BRANCO;
}

void printJogada(Jogada jogada){
  cout << "(" << jogada.origem.x << "," << jogada.origem.y << ") -> (" << jogada.destino.x << "," << jogada.destino.y << ")";
}

void clear(){
  system("clear");
}

struct InfoJogo{
  Jogada jogada;
  bool vez_jogador;
};

void Jogar(int rank){
  char p;
  Lado jogador, cpu;
  if(rank == 0){
    do{
      cout << "Escolha o lado: (b)ranco ou (p)reto" << endl;
      cin >> p;
    }while(p != 'b' && p != 'p' && p != 'B' && p != 'P');
  }
  clear();
  if(p == 'b' || p == 'B'){
    jogador = LADO_BRANCO;
    cpu = LADO_PRETO;
  }else{
    jogador = LADO_PRETO;
    cpu = LADO_BRANCO;
  }
  MPI_Bcast(&jogador, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(&cpu, 1, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Barrier(MPI_COMM_WORLD);

  Jogador j1(jogador, "Jogador");
  IA j2(cpu, "CPU");

  bool vez_jogador = true;
  Jogo jogo;
  Status status = COMPLETO;
  //comunicação distribuída aqui
  while(status != GAME_OVER){
    // clear();
    jogo.imprimeTabuleiro();
    // if(!vez_jogador){
    //   system("pause");
    // }
    MPI_Barrier(MPI_COMM_WORLD);

    if(vez_jogador && rank == 0){
      cout << "Sua vez" << endl;
      vector<Jogada> jogadas_jogador;
      jogo.geraTodosOsMovimentos(j1.getLado(), jogadas_jogador);

      if(jogadas_jogador.empty()){
        cout << "Sem movimentos possiveis" << endl;
        cout << "Voce perdeu" << endl;
        status = GAME_OVER;
        break;
      }else{
        int num_jogadas = jogadas_jogador.size();
        cout << "Jogadas possiveis: " << endl;
        for(int i = 0; i<num_jogadas; i++){
          printJogada(jogadas_jogador[i]);
          cout << endl;
        }
      }
      Jogada jogada;
      do{
        cout << "Digite a jogada (x1 y1 x2 y2): " << endl;
        cin >> jogada.origem.x >> jogada.origem.y >> jogada.destino.x >> jogada.destino.y;
        status = jogo.fazJogada(jogada, jogador);
        if(status == FAIL_DESTINO_INVALIDO){
          cout << "Jogada invalida" << endl;
        }else if(status == FAIL_PECA_INVALIDA){
          cout << "Peca escolhida invalida" << endl;
        }

      }while(status != COMPLETO && status != GAME_OVER);
      vez_jogador = !vez_jogador;

      InfoJogo info;
      info.jogada = jogada;
      info.vez_jogador = vez_jogador;

      MPI_Send(&info, sizeof(InfoJogo), MPI_BYTE, 1, 0, MPI_COMM_WORLD);

      MPI_Recv(&info, sizeof(InfoJogo), MPI_BYTE, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      Jogada jogada_computador = info.jogada;
      vez_jogador = info.vez_jogador;

      status = jogo.fazJogada(jogada_computador, j2.getLado());

      MPI_Barrier(MPI_COMM_WORLD);

    }else{
      // cout << "Vez do computador" << endl;
      vector<Jogada> jogadas_computador;
      Jogada jogada;
      InfoJogo info;

      MPI_Recv(&info, sizeof(InfoJogo), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

      jogada = info.jogada;
      vez_jogador = info.vez_jogador;

      status = j1.fazJogada(jogada, jogo);

      jogo.geraTodosOsMovimentos(j2.getLado(), jogadas_computador);
      if(jogadas_computador.empty()){
        cout << "Sem movimentos possiveis" << endl;
        cout << "Voce ganhou" << endl;
        break;
      }
      Jogada jogada_computador = j2.inicioMinimax(jogo, j2.getLado(), true);
      cout << "Jogada do computador: ";
      printJogada(jogada_computador);
      cout << endl;
      status = jogo.fazJogada(jogada_computador, j2.getLado());
      vez_jogador = !vez_jogador;

      info.jogada = jogada_computador;
      info.vez_jogador = vez_jogador;

      MPI_Send(&info, sizeof(InfoJogo), MPI_BYTE, 0, 0, MPI_COMM_WORLD);

      MPI_Barrier(MPI_COMM_WORLD);
    }
  }

}

int main(int argc, char** argv) {
  srand(time(NULL));

  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if(rank == 0){
    clear();
    cout << "Bem vindo ao jogo de damas" << endl;
    cout << "Desenvolvido por: " << endl;
    cout << "Thiago Scholl" << endl;
    cout << "Lucas Ferreira" << endl;
  }

  char jogar_novamente;
  do {

    Jogar(rank);  // Jogador principal

    MPI_Barrier(MPI_COMM_WORLD);

    // Comunicar se alguém deseja jogar novamente
    int jogar_novamente_local = (rank == 0) ? (jogar_novamente == 's' || jogar_novamente == 'S') : 0;
    MPI_Bcast(&jogar_novamente_local, 1, MPI_INT, 0, MPI_COMM_WORLD);

    jogar_novamente = (jogar_novamente_local == 1) ? 's' : 'n';

  } while (jogar_novamente == 's' || jogar_novamente == 'S');

  MPI_Finalize();
  return 0;
}