### 1)

As funções do Kernel podem ser divididas em cinco partes principais:

1.  **Gerenciamento de Processos:** O kernel é responsável pela criação e destruição de processos, bem como pela conexão deles com o mundo exterior (entrada e saída). Ele lida com a comunicação entre diferentes processos (através de sinais, pipes ou primitivas de comunicação interprocessos) e inclui o agendador (*scheduler*), que controla como os processos compartilham a CPU.
2.  **Gerenciamento de Memória:** O kernel constrói um espaço de endereçamento virtual para todos os processos sobre os recursos limitados disponíveis. Essa parte é crítica para o desempenho do sistema e interage através de chamadas de função para alocação e liberação de memória.
3.  **Sistemas de Arquivos:** Como o Unix trata quase tudo como um arquivo, o kernel constrói sistemas de arquivos estruturados em cima de um hardware não estruturado. O Linux suporta múltiplos tipos de sistemas de arquivos para organizar dados no meio físico (como discos).
4.  **Controle de Dispositivos:** Quase toda operação do sistema mapeia eventualmente para um dispositivo físico. O kernel deve conter *device drivers* (drivers de dispositivo) para cada periférico presente no sistema (discos rígidos, teclados, etc.) para controlar essas operações.
5.  **Rede:** O sistema operacional deve gerenciar as operações de rede (pacotes de dados) porque elas são eventos assíncronos não específicos a um processo. O kernel coleta, identifica e despacha pacotes de entrada, além de lidar com o roteamento e a resolução de endereços.