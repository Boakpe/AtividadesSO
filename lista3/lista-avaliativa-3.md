**Universidade Federal de Lavras**


**PPGCC**
**PCC508 – Sistemas Operacionais**

**Lista Avaliativa – 3**


1) Como pode-se obter exclusão mútua utilizando-se a TSL? E desabilitando interrupções? São
soluções boas? Quais as desvantagens?


2) Explique como funciona o método de exclusão mútua utilizando Mutexes, abordando sua
implementação na biblioteca Pthreads e também variáveis de condição.


3) Descreva, com exemplo, como funciona o escalonamento conhecido como “Round Robin”.


4) O algoritmo de escalonamento baseado em prioridades dinâmicas, onde a prioridade é calculada
com 1/f (f = fração do quantum utilizada), prioriza qual tipo de processo? Qual a vantagem de
priorizar esse tipo de processo?


5) Solução de Peterson. Nesse exercício, um contador compartilhado será acessado por 2 threads
simultaneamente. Elas devem pegar o valor do contador, imprimir na tela, executar thread_yield,
somar um no contador. Duas versões devem ser feitas: uma sem nenhum controle de condição de
corrida e outra utilizando a solução de Peterson para isso.


5) Um shell no Unix é resposável por receber comandos do usuário e executá-los, em linha de
comando. Criar um mini-shell que permita o usuário executar comandos que são executáveis no
Linux. Para isso, use as chamadas de sistema fork/execve. Permita também o usuário listar os
arquivos do diretório atual com ls, que nesse caso será implementado como comando interno.


6) Implementar o problema dos filósofos glutões com semáforos nomeados. Cada filósofo será
implementado como um processo. Para o controle de concorrência, serão utilizados uma série de
semáforos nomeados. O programa deverá receber um parâmetro que é o número de filósofos a
serem criados. Para criação de processos, a chamada fork deve ser utilizada.


