Universidade Federal de Lavras


PPGCC

PCC508 – Sistemas Operacionais

Lista Avaliativa – 2
1) Explique quais são as condições para um processo passar do estado bloqueado para o
executando.


2) Explique quais são as principais informações armazenadas na entrada de um processo na tabela
de processos. Indique o motivo destas informações.


3) Explique a diferença entre processos e threads, indicando quais itens são únicos por processo e
quais por threads.


4) As threads podem ser implementadas dentro do kernel ou como uma biblioteca no espaço de
usuário. Explique a diferença, as vantagens e desvantagens de cada um dos métodos.


5) Crie um programa que inicialmente cria 4 processos filhos (com fork), formando um conjunto de
5 processos (processo pai e 4 processos filhos). Cada processo filho cria 3 threads. Cada thread
dorme um tempo aleatório entre 2 e 10 segundos e termina. O processo filho deve esperar todas as
threads terminarem, enviar o seu pid para o processo pai e aí também terminar. O processo pai deve
receber o pid dos 3 filhos, esperar todos os processos filhos terminarem e imprimir na tela os pids
recebidos e o tempo total transcorrido desde o início. Após isso, deve também terminar. A
comunicação dos filhos com o pai será feita utilizando um pipe.


6) O código encontrado no arquivo codigo_exercicio.c lê uma imagem de um arquivo, aplica um
filtro de convolução em cada canal de cor da imagem e salva novamente a imagem em um outro
arquivo. O código utiliza a biblioteca stb e deve ser compilado com: gcc programa.c -lm
Modifique o código para que o processamento de cada um dos canais seja processado por uma
thread independente. Utilize para isso as threads POSIX.
Observação: a biblioteca stb deve estar instalada para que seja possível compilar o código. Consulte
o professor para maiores detalhes.


