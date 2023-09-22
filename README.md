# Projeto Jogo de Damas com Paralelização em OpenMP e MPI

Este projeto implementa um jogo de damas em C++ utilizando paralelização com OpenMP e processamento distribuído com MPI.

## Compilação

Para compilar o jogo de damas, utilize o seguinte comando:

```bash
mpicxx -fopenmp -o damas damas.cpp -lmpi
```

Este comando compila o código-fonte (`damas.cpp`) utilizando o compilador MPI com suporte a OpenMP.

## Execução

Para rodar o jogo de damas, utilize o comando a seguir:

```bash
mpirun -quiet -host localhost:2 ./damas
```

Este comando executa o programa utilizando `mpirun`, especificando o número de hosts e a aplicação a ser executada (`damas`). A flag `-quiet` suprime a saída dos hosts. Certifique-se de que a aplicação foi compilada corretamente antes de executar este comando.

## Funcionamento do Jogo

O jogo de damas é um jogo de tabuleiro para um jogador contra a máquina. As regras e a interface do jogo são implementadas no código-fonte (`damas.cpp`). Siga as instruções apresentadas durante a execução do jogo para jogar.
