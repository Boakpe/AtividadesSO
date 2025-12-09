Universidade Federal de Lavras


PPGCC

PCC508 – Sistemas Operacionais

Lista Avaliativa – 4


1) Explique como funciona a memória virtual, juntamente com a paginação.
2) Explique em detalhes como funciona a tradução de um endereço virtual para o endereço físico
em um sistema com memória virtual e paginação. Use um exemplo e considere que o endereço
virtual tem mais bits que o endereço físico. Isso é possível? Explique também qual a função da
MMU.
3) Explique a estrutura de uma tabela de páginas.
4) Explique o que é, para que serve e como funciona uma TLB (translation lookaside buffer).
5) Deve ser desenvolvido um programa que cria dois processos filhos. Esses processos devem
compartilhar uma área de memória criada com a chamada mmap. Vamos agora utilizar essa área
como um buffer compartilhado. Um dos processos será um produtor e deve gerar uma letra aleatória
em cada intervalo de tempo (configurável) e adicionar ao buffer. O outro processo deve consumir
esses itens (letras), uma a cada intervalo de tempo (configurável) e imprimir o item na tela. Não
esquecer o problema da condição de corrida. O processo pai não executa nenhuma tarefa, fica
bloqueado esperando os processos filhos.
6) No presente exercício, um programa de chat deve ser criado. Diversas instâncias do programa
(em uma mesma máquina) devem ser capaz de interagir através de um chat onde na tela de cada um
dos programas aparecem as mensagens do mesmo e dos outros. Para fazer a comunicação entre os
processos, memória compartilhada (com mmap) deve ser utilizada. Também deve ser utilizados
semáforos nomeados para possíveis sincronizações.


