#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

int desenhar (int intervalo);
int checkRepeticoes (int *cromossomo, int nn, int numero);
int *aleatorio (int numeroOf);
void criaPais (int **pais, int numeroDeCidades, int numeroPais);
float calcularCusto(int tamanho, int *solucao, float **matrizDeDistancia);
void clasificacao (int **solucoes, int numeroCidade, int populacao, float **matrizDeDistancia);
void propagacao (int **pais, int **geracao, int numeroPais, int tamanhoGene);
void mutacao (int **geracao,int comprimentoGene ,int populacao);
void novo (int **A, int **B, int nLinhas, int nColunas);

int main(int argc, char* argv[]){
    FILE *fileIn, *fileOUT;
    int numeroDeCidades, populacao, numeroDeGeracoes,i, j;
    int **pais;
    int **geracao;
    float **matrizDeDistancia;
    float distancia;

    fileIn = fopen(argv[1],"r");
    if(fileIn == NULL){
        printf("Arquivo em branco");
        getch();
        return 1;
    }
    printf("Come�ando a ler o arquivo\n");
    while(feof(fileIn) == 0){
        if(fscanf(fileIn,"%d",&numeroDeCidades)!=1){/*verificando o formato do arquivo de entrada*/
            printf("Formato de arquivo incorreto");
            getch();
            return 1;
        }else{
            printf("Numero de Cidades: %d\n",numeroDeCidades);
            fscanf(fileIn, "%d", &populacao);
            printf("Numero de individuos na populacao: %d\n",populacao);
            fscanf(fileIn, "%d",&numeroDeGeracoes);
            printf("Numero de geracoes: %d\n",numeroDeGeracoes);

            matrizDeDistancia = (float**)malloc(sizeof(float)*numeroDeCidades);

            for(i=0; i<numeroDeCidades;i++){
                matrizDeDistancia[i]=(float*)malloc(sizeof(float)*(numeroDeCidades));
                for(j=0;j<numeroDeCidades;j++){
                    fscanf(fileIn,"%f",&distancia);
                    matrizDeDistancia[i][j]=distancia;
                }
            }
        }
        fclose(fileIn);

        pais=(int**)malloc(sizeof(int*)*populacao);
        geracao=(int**)malloc(sizeof(int*)*populacao);

        criaPais(pais,numeroDeCidades,populacao);
        clasificacao(pais, numeroDeCidades, populacao, matrizDeDistancia);

        fileOUT =fopen(argv[2],"w");
        for(i=0;i<numeroDeGeracoes;i++){
            propagacao(pais,geracao,populacao,numeroDeCidades);
            clasificacao(geracao,numeroDeCidades,populacao,matrizDeDistancia);

            if(i%1000==0)
            printf("O caminho mais curto em %d \n Gera��o:%f",i,calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
            fprintf(fileOUT,"%f\n", calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
            mutacao(geracao,numeroDeCidades,populacao);
            novo(geracao,pais,populacao,numeroDeCidades);
        }
        fclose(fileOUT);
        printf("A melhor solu��o de custo %f �:\n",calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
        printf("[ "); /*escrevendo a melhor solu��o*/

        for(i=0;i<numeroDeCidades;i++){
            printf("%d ",pais[0][i]);
        }
        printf("]");

        for(i=0;i<numeroDeCidades;i++){
            free(matrizDeDistancia[i]);
        }
        free(matrizDeDistancia);
        for(i=0;i<populacao;i++){
            free(pais[i]);
            free(geracao[i]);
        }
        free(pais);
        free(geracao);
        getch();
    }
    return 0;
   }


/* uma fun��o que retorna um n�mero aleat�rio no intervalo <0;-1> */
int desenhar(int intervalo){
    return(rand()%intervalo);
}
/* uma fun��o que verifica se um gene repetitivo (n�mero da cidade) n�o � desenhado no cromossomo */
int checkRepeticoes(int *cromossomo, int nn, int numero){
    int i;

    if(nn <= 0)
        return 0;
    for(i=0; i<nn;i++){
        if(cromossomo[i] == numero)
            return 1;
    }
    return 0;
}
/* fun��o que retorna o ponteiro para a matriz de solu��es desenhadas 
 n�meros (ordem das cidades visitadas) n�o podem ser repetidos */
int *aleatorio(int numeroOf){
    int *nDesenhados;
    int sorteio;
    int numero;

    sorteio = 0; /* Contador de numeros sorteados */
    nDesenhados = (int*)malloc((sizeof(int))*numeroOf); /*aloca��o de mem�ria para tabelas num�ricas */
    do{ /* desenhando os pr�ximos n�meros e verificando se n�o h� duplicatas*/
        numero = desenhar(numeroOf);
        if(checkRepeticoes(nDesenhados, sorteio, numero)==0){
            nDesenhados[sorteio]=numero;
            sorteio++;
        }
    }while (sorteio);
    return nDesenhados;
}
/* a fun��o geradora da primeira gera��o de solu��es - pais */
void criaPais(int **pais, int numeroDeCidades, int numeroPais){
    int i;
    srand(time(NULL)); /*inicializa��o do embri�o para a fun��o rand*/
    for(i=0; i<numeroPais; i++){
        pais[i] =aleatorio(numeroDeCidades); /*e o pai aponta para a solu��o desenhada*/
    }
}
/* Fun��o calculando o custo da viagem entre cidades individuais 
de acordo com a ordem contida no cromossomo */
float calcularCusto(int numeroDeCidades, int *cromossomo, float **matrizDeDistancia){
    int i;
    float custo;

    custo = 0;

    for(i=0;i<(numeroDeCidades-1);i++){
        custo = custo + matrizDeDistancia[cromossomo[i]][cromossomo[i+1]];
    }
    custo = custo + matrizDeDistancia[cromossomo[i]][cromossomo[0]];
    return custo;

}
/* fun��o de sele��o de solu��es (cromossomos) de acordo com seus custos */
/* classifica��o por sele��o */
void clasificacao (int **solucoes, int numeroCidade, int populacao, float **matrizDeDistancia){
    int i, j, min;
    int *temp;

    for(j=0; j<(populacao-1); j++){
        min = j;
        for(i=j+1; i<populacao;i++){
            if(calcularCusto(numeroCidade, solucoes[i], matrizDeDistancia)< calcularCusto(populacao, solucoes[min], matrizDeDistancia)){
                min=i;
            }
            temp = solucoes[min];
            solucoes[min]=solucoes[j];
            solucoes[j]=temp;
        }
    }
}
/* fun��o de propaga��o, operando com base no cruzamento */
void propagacao (int **pais, int **geracao, int numeroPais, int tamanhoGene){
    int i,j,k,temp;
    //int l;
    int *aVisita, visitados;

    srand(time(NULL));
    aVisita = (int*)malloc(sizeof(int)*tamanhoGene);
    geracao[0] = (int*)malloc(sizeof(int)*tamanhoGene);
    for(i=0;i<tamanhoGene;i++) /*o primeiro indiv�duo de uma nova gera��o*/
        (geracao[0])[i] = (pais[0])[i]; /*torna-se o melhor indiv�duo da velha gera��o*/
    for(i=1;i<numeroPais;i++){ /*o resto da gera��o � criado pelo cruzamento*/
        visitados = 0;
        geracao[i]=(int*)malloc(sizeof(int)*tamanhoGene);
        k=(rand()%(numeroPais/2)); /*k � um n�mero aleat�rio no intervalo <0; (n�mero de nascimentos / 2) -1> para cruzar apenas indiv�duos melhores do conjunto pai*/
        /*l=((rand()%2)+3); //l � um n�mero aleat�rio*/
        for(j=0;j<(tamanhoGene/1);j++){
            (geracao[i])[visitados]= (pais[k])[j];
            aVisita[visitados] = (pais[k])[j];
            visitados++;
        }
        k=(rand()%(numeroPais/2));
        do{
            if(j==tamanhoGene){
                j=0;
            }
            temp=(pais[k])[j];
            if(checkRepeticoes(aVisita, visitados, temp)==0){
                (geracao[i])[visitados] = (pais[k])[j];
                aVisita[visitados] = (pais[k])[j];
                visitados++;
            }
            j++;
        }while(visitados<tamanhoGene);
    }
}
/*fun��o que faz uma muta��o*/
void mutacao (int **geracao,int comprimentoGene ,int populacao){
    int a, b, c, j, temp, los;
    srand(time(NULL));
    for(j=0;j<populacao;j++){
        los = rand()%2;
        if(los==1){/*a probabilidade de muta��o cromoss�mica � de 50%*/
            a = (rand()%(populacao-1)+1); /*desenhando um n�mero da faixa <1; popula��o-1>*/
            b = rand()%comprimentoGene; /*o melhor indiv�duo n�o ser� mutado!*/
            c = rand()%comprimentoGene;

            temp = (geracao[a])[b];
            (geracao[a])[b] = (geracao[a])[c];
            (geracao[a])[c] = temp;
        }
    }
}
/*uma fun��o que prescreve dados de um array bidimensional A para um array bidimensional B*/
void novo (int **A, int **B, int nLinhas, int nColunas){
    int i,j;
    for(i=0;i<nLinhas;i++){
        for(j=0;j<nColunas;j++){
            (B[i])[j]=(A[i])[j];
        }
    }
}
