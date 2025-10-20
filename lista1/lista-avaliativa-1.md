Universidade Federal de Lavras


PPGCC
PCC508 – Sistemas Operacionais

Lista Avaliativa – 1


1) Explique o que é um sistema operacional, utilizando a visão do sistema como uma
máquina estendida e como um gerenciador de recursos.

2) Explique o que o modo supervisor e usuário, e em que momentos existe a troca de
modo do processador.

3) Explique o sistema de proteção de arquivos baseado em um código de 9 bits para cada
arquivo.

4) O que é um processo? Quando um processo não está em execução, onde as
informações sobre o mesmo são armazenadas para permitirem que ele continue
posteriormente sua execução do mesmo ponto onde foi parada?

5) Explique o que é espaço de endereçamento e memória virtual.

6) Explique uma forma de estruturar um sistema operacional chamado de micronúcleo.

7) Desenvolver um programa onde um processo pai cria cinco filhos. Cada um dos filhos
sorteia um número entre 0 e 200 e envia para o pai através de um pipe. O processo pai
imprime então qual foi o menor número recebido e também o PID do filho correspondente
ao menor número enviado.

8) No presente exercício, um processo pai irá criar 3 filhos. Cada filho imprimirá na tela
números sequenciais em intervalos de 1 segundo, juntamente com o seu PID. O processo
pai, após a criação dos filhos, irá permitir que somente 1 filho execute em cada momento
por um período de 10s. A cada período, o filho que está executando será parado com
SIGSTOP e o próximo será liberado para executar (com SIGCONT). Não esquecer que
após a criação dos 3 filhos, todos devem ser parados para dar início a essa sequência.

9) No presente exercício, um processo pai irá criar 10 processos filhos. Como um
processo filho herda as variáveis de ambiente do processo pai, existe a possibilidade de
se passar informação entre esses processos. Assim, através da criação de uma variável
de ambiente com o nome “num”, deve ser feita a comunicação entre o processo pai e
cada um dos filhos. Essa variável de ambiente conterá um número aleatório diferente para
cada um dos 10 processos criados com fork. Cada processo filho imprimirá na tela o seu
número recebido, retirado da variável de ambiente e terminará. Para lidar com as
variáveis de ambiente, poderão ser utilizadas as funções setenv e getenv.


