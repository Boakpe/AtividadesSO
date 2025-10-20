Universidade Federal de Lavras


PPGCC

PCC508 – Sistemas Operacionais

Lista Avaliativa – 6


1) Explique como nomes longos de arquivos podem ser manipulados em um diretórios.


2) Como funciona o Journaling em um sistema de arquivos? Explique.


3) Explique como funciona a alocação de blocos de arquivos baseadas em Inodes.


4) Explique como funciona uma tabela FAT (File Allocation Table).


5) Quando um aplicação necessita de um controle mais aprimorado sobre as permissões associadas
a um determinado arquivo, as Access Control Lists (ACL) podem ser utilizadas para isso. Descrever
uma visão geral de como as ACLs funcionam.


6) A chamada _statvfs(…)_ é utilizada para obter-se informações sobre um sistema de arquivos
montado. Faça um programa que receba como parâmetro o caminho de um sistema de arquivos e
apresente as informações obtidas através desta chamada.


7) Criar dois programas. O primeiro cria um arquivo binário, com 30 _ints_, realizando uma contagem
(1,2,3,4...30). Isso significa que os 30 _ints_ devem ser armazenados em sequência no arquivo. O
segundo programa deve utilizar a função readv para fazer a leitura simultânea em múltiplos buffers
de 8 números do arquivo gerado pelo primeiro programa. Essa leitura deve ser feita somente com
uma instrução readv. Os números a serem lidos devem ser a partir do décimo armazenado (décimo
no buffer 1, décimo primeiro no buffer 2, e assim por diante). Imprimir os números na tela. Não
esqueça que cada int tem o seu tamanho em bytes fixo.


