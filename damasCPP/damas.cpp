#include <algorithm>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <time.h>
#include "damas.h"
using namespace std;

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

int main(){
  Jogo jogo;
  jogo.montarTabuleiro();
  jogo.imprimeTabuleiro();
  return 0;
}