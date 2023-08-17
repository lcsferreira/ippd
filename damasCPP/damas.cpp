#include <algorithm>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <time.h>
#include "damas.h"
using namespace std;

void printJogada(Jogada jogada);

Jogo::Jogo(){
  montarTabuleiro();
}

Jogo::Jogo(Peca** tabuleiro){
  num_pecas_brancas = 0;
  num_pecas_pretas = 0;
  num_damas_brancas = 0;
  num_damas_pretas = 0;
  tabuleiro = tabuleiro;

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
    for(int j = 0; j < 8; j++){
      if(i < 3){
        if((i + j) % 2 == 0){
          tabuleiro[i][j] = PRETA;
        }else{
          tabuleiro[i][j] = VAZIO;
        }
      }else if(i > 4){
        if((i + j) % 2 == 0){
          tabuleiro[i][j] = BRANCA;
        }else{
          tabuleiro[i][j] = VAZIO;
        }
      }else{
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

  for (int i=0; i<8; i++){
    for (int j=0; j<8; j++){
      Ponto p = {i,j};
      Peca peca = getPeca(p);
      if (peca == normal || peca == dama){
        geraJogadasDePuloValidas(p, lado, jogadas);
      }
    }
  }

  if(jogadas.empty()){
    cout<< "Sem pulos" << endl;
    for (int i=0; i<8; i++){
      for (int j=0; j<8; j++){
        Ponto p = {i,j};
        Peca peca = getPeca(p);
        if (peca == normal || peca == dama){
          geraJogadasValidas(p, lado, jogadas);
        }
      }
    }
  }
}

void Jogo::geraJogadasValidas(Ponto p, Lado lado, vector<Jogada> &jogadas){
  Peca peca = tabuleiro[p.x][p.y];

  if(peca == VAZIO){
    cout << "Ponto vazio" << endl;
    throw Falha();
  }

  cout << "Ponto: " << p.x << " " << p.y << endl;

  if (peca == BRANCA || peca == PRETA){
    int mudarLinha = peca == PRETA ? -1 : 1;
    int novaLinha = p.x + mudarLinha;
    // cout<< "Nova linha: " << novaLinha << endl;

    if(novaLinha >=0 && novaLinha < 8){
      int novaColuna = p.y + 1;
      cout << "Peca: " << getPeca(novaLinha, novaColuna) << endl;
      if(novaColuna < 8 && getPeca(novaLinha, novaColuna) == VAZIO){
        cout << "Jogada valida" << endl;
        jogadas.push_back({p, novaLinha, novaColuna});
      }
      novaColuna = p.y - 1;
      cout << "Peca: " << getPeca(novaLinha, novaColuna) << endl;
      if(novaColuna >= 0 && getPeca(novaLinha, novaColuna) == VAZIO){
        cout << "Jogada valida" << endl;

        jogadas.push_back({p, novaLinha, novaColuna});
      }
    }
  }else {
    int novaLinha = p.x + 1;
    if(novaLinha < 8){
      int novaColuna = p.y + 1;
      cout << "Peca: " << getPeca(novaLinha, novaColuna) << endl;
      if(novaColuna < 8 && getPeca(novaLinha, novaColuna) == VAZIO){
        cout << "Jogada valida" << endl;

        jogadas.push_back({p, novaLinha, novaColuna});
      }
      novaColuna = p.y - 1;
      cout << "Peca: " << getPeca(novaLinha, novaColuna) << endl;
      if(novaColuna >= 0 && getPeca(novaLinha, novaColuna) == VAZIO){
        cout << "Jogada valida" << endl;

        jogadas.push_back({p, novaLinha, novaColuna});
      }
    }
    novaLinha = p.x - 1;
    if(novaLinha >= 0){
      int novaColuna = p.y + 1;
      cout << "Peca: " << getPeca(novaLinha, novaColuna) << endl;
      if(novaColuna < 8 && getPeca(novaLinha, novaColuna) == VAZIO){
        cout << "Jogada valida" << endl;

        jogadas.push_back({p, novaLinha, novaColuna});
      }
      novaColuna = p.y - 1;
      cout << "Peca: " << getPeca(novaLinha, novaColuna) << endl;
      if(novaColuna >= 0 && getPeca(novaLinha, novaColuna) == VAZIO){
        cout << "Jogada valida" << endl;

        jogadas.push_back({p, novaLinha, novaColuna});
      }
    }
  }
  cout << "Jogadas validas: " << jogadas.size() << endl;
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

  for(int i=0; i<tam; i++){
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

Jogador::Jogador(Lado lado, string nome){
  lado = lado;
  nome = nome;
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
    return {-1,-1,-1};
  }

  Jogo jogo_temp;
  int num_jogadas = jogadas_possiveis.size();
  for(int i = 0; i<num_jogadas; i++){
    jogo_temp = *jogo.clonaJogo();
    Status status = jogo_temp.fazJogada(jogadas_possiveis[i], lado);
    heuristicas.push_back(minimax(jogo_temp, lado, profundidade, !maximizando, alpha, beta));
  }

  double max_heuristica = -1000;
  int num_heuristicas = heuristicas.size();
  for(int i = num_heuristicas -1; i >= 0; i--){
    if(heuristicas[i] >= max_heuristica){
      max_heuristica = heuristicas[i];
    }
  }

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
  if(profundidade == 0){
    return calcHeuristica(jogo);
  }

  vector<Jogada> jogadas_possiveis;
  jogo.geraTodosOsMovimentos(lado, jogadas_possiveis);
  int tam = jogadas_possiveis.size();
  int inicial = 0;
  Jogo jogo_temp;
  if(maximizando){
    for(int i =0; i< tam; i++){
      jogo_temp = *jogo.clonaJogo();
      Status status = jogo_temp.fazJogada(jogadas_possiveis[i], lado);
      if(status != COMPLETO && status != GAME_OVER){
        continue;
      }
      int resultado = minimax(jogo_temp, trocaLado(lado), !maximizando, profundidade - 1, alpha, beta);
      inicial = max(inicial, resultado);
      alpha = max(alpha, inicial);
      if(alpha >= beta){
        break;
      }
    }
  }else{
    inicial = 1000;
    for(int i =0; i<tam; i++){
      jogo_temp = *jogo.clonaJogo();
      Status status = jogo_temp.fazJogada(jogadas_possiveis[i], lado);
      if(status != COMPLETO && status != GAME_OVER){
        continue;
      }
      int resultado = minimax(jogo_temp, trocaLado(lado), !maximizando, profundidade - 1, alpha, beta);
      inicial = min(inicial, resultado);
      beta = min(beta, inicial);
      if(alpha >= beta){
        break;
      }
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

void Jogar(){
  char p;
  Lado jogador, cpu;
  do{
    cout << "Escolha o lado: (b)ranco ou (p)reto" << endl;
    cin >> p;
  }while(p != 'b' && p != 'p' && p != 'B' && p != 'P');
  clear();
  if(p == 'b' || p == 'B'){
    jogador = LADO_BRANCO;
    cpu = LADO_PRETO;
  }else{
    jogador = LADO_PRETO;
    cpu = LADO_BRANCO;
  }

  Jogador j1(jogador, "Jogador");
  IA j2(cpu, "CPU");

  bool vez_jogador = true;
  Jogo jogo;
  Status status = COMPLETO;
  while(status != GAME_OVER){
    clear();
    cout<< "Jogador: " << j1.getNome() << endl;
    cout << "CPU: " << j2.getNome() << endl;
    cout << "Jogador: " << jogo.getNumPecasBrancas() << " pecas brancas e " << jogo.getNumDamasBrancas() << " damas brancas" << endl;
    cout << "CPU: " << jogo.getNumPecasPretas() << " pecas pretas e " << jogo.getNumDamasPretas() << " damas pretas" << endl;
    jogo.imprimeTabuleiro();
    if(!vez_jogador){
      system("pause");
    }
    if(vez_jogador){
      cout << "Sua vez" << endl;
      vector<Jogada> jogadas_possiveis;
      jogo.geraTodosOsMovimentos(jogador, jogadas_possiveis);
      if(jogadas_possiveis.empty()){
        cout << "Sem movimentos possiveis" << endl;
        cout << "Voce perdeu" << endl;
        status = GAME_OVER;
        break;
      }else{
        int num_jogadas = jogadas_possiveis.size();
        cout << "Jogadas possiveis: " << endl;
        for(int i = 0; i<num_jogadas; i++){
          printJogada(jogadas_possiveis[i]);
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
    }else {
      cout << "Vez do computador" << endl;
      vector<Jogada> jogadas_possiveis;
      jogo.geraTodosOsMovimentos(cpu, jogadas_possiveis);
      if(jogadas_possiveis.empty()){
        cout << "Sem movimentos possiveis" << endl;
        cout << "Voce ganhou" << endl;
        break;
      }
      Status status = j2.fazJogada(jogo);
      vez_jogador = !vez_jogador;
    }
  }

}

int main(){
  srand(time(NULL));
  clear();
  cout << "Bem vindo ao jogo de damas" << endl;
  cout << "Desenvolvido por: " << endl;
  cout << "Thiago Scholl" << endl;
  cout << "Lucas Ferreira" << endl;
  char jogar_novamente;
  do{
    Jogar();
    do{
      cout << "Deseja jogar novamente? (s/n)" << endl;
      cin >> jogar_novamente;
    }while(jogar_novamente != 's' && jogar_novamente != 'n' && jogar_novamente != 'S' && jogar_novamente != 'N');
  }while(jogar_novamente == 's' || jogar_novamente == 'S');
  return 0;
}