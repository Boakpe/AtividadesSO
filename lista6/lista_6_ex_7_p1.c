#include <stdio.h>
#include <stdlib.h>

int main()
{
    const char *nome_arquivo = "numeros.bin";

    FILE *arquivo = fopen(nome_arquivo, "wb");

    if (arquivo == NULL)
    {
        perror("Erro ao abrir o arquivo para escrita");
        return 1;
    }

    printf("Arquivo '%s' aberto para escrita.\n", nome_arquivo);

    for (int i = 1; i <= 30; i++)
    {
        if (fwrite(&i, sizeof(int), 1, arquivo) != 1)
        {
            fprintf(stderr, "Erro ao escrever o número %d no arquivo.\n", i);
            fclose(arquivo);
            return 1;
        }
    }

    printf("30 números inteiros foram escritos com sucesso no arquivo '%s'.\n", nome_arquivo);

    fclose(arquivo);

    return 0;
}