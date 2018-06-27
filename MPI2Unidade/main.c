#include <stdio.h>
#include <stdlib.h>
#include "funcoes.h"
#include <time.h>
#include <string.h>
#include <math.h>
#include <mpi.h>
 
int main(int argc, char* argv[]){
    FILE *fileIn, *fileOUT;
    int numeroDeCidades, populacao, numeroDeGeracoes,j;//i
    int **pais;
    int **geracao;
    float **matrizDeDistancia;
    float **matrizDeDistanciaGlobal;
    float distancia;
    srand(time(0));
    /* inicializacao mpi*/
    int id, tam, i;
    int Mv[numeroDeCidades * numeroDeCidades];
    int vec = 0;
    MPI_Status status;

    MPI_Init(&argc,&argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &id); 
    MPI_Comm_size (MPI_COMM_WORLD, &tam);

    if (id==0) {
    /*inicio leitura do arquivo*/
    fileIn = fopen(argv[1],"r");
    if(fileIn == NULL){
        printf("Arquivo em branco");
        getchar();
        return 1;
    }
    /*Arquivo de entrada tem que conter somente numero, por favor retirar nomes nos testes*/
    printf("Comecando a ler o arquivo\n");
    
    if(fscanf(fileIn,"%d",&numeroDeCidades)!=1){/*verificando o formato do arquivo de entrada*/
        printf("Formato de arquivo incorreto");
        getchar();
        return 1;
    }else{
        printf("Numero de Cidades: %d\n",numeroDeCidades);
        fscanf(fileIn, "%d", &populacao);
        printf("Numero de individuos na populacao: %d\n",populacao);
        fscanf(fileIn, "%d",&numeroDeGeracoes);
        printf("Melhor numero de geracoes: %d\n",numeroDeGeracoes);

        matrizDeDistancia = (float**)malloc(sizeof(float)*numeroDeCidades);
        //matrizDeDistanciaGlobal == (float**)malloc(numeroDeCidades*numeroDeCidades*sizeof(float));

        for(i=0;i<numeroDeCidades;i++){
            matrizDeDistancia[i]=(float*)malloc(sizeof(float)*(numeroDeCidades));
            for(j=0;j<numeroDeCidades;j++){
                fscanf(fileIn,"%f",&distancia);
                matrizDeDistancia[i][j]=distancia;
            }
        }
        for(i =0; i < numeroDeCidades; i++){
            for(j=0; j<numeroDeCidades; j++){
                Mv[vec] = matrizDeDistancia[i][j];
                vec = vec +1;
            }
        }

    }
    fclose(fileIn);

    //pais=(int**)malloc(sizeof(int*)*populacao);
    //geracao=(int**)malloc(sizeof(int*)*populacao);

    criaPais(pais,Mv,populacao);
    clasificacao(pais, numeroDeCidades, populacao, matrizDeDistancia);

    for( j=0;j<tam;j++){
        MPI_Send(&populacao,(numeroDeCidades *(populacao+1)), MPI_INT, j, 100, MPI_COMM_WORLD);
    }
}else{
        MPI_Recv(&populacao,(numeroDeCidades *(populacao+1)),MPI_INT, 0, 100,MPI_COMM_WORLD, &status);
    }
    fileOUT =fopen(argv[2],"w");
    for(i=0;i<numeroDeGeracoes;i++){
        propagacao(pais,geracao,populacao,numeroDeCidades);
        clasificacao(geracao,numeroDeCidades,populacao,matrizDeDistancia);

        if(i%1000==0)
        printf("O caminho mais curto em %d Geracao: %.0f\n",i,calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
        fprintf(fileOUT,"%f\n", calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
        mutacao(geracao,numeroDeCidades,populacao);
        novo(geracao,pais,populacao,numeroDeCidades);
    }
    fclose(fileOUT);
    printf("A melhor solucao de custo %f e:\n",calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
    printf("[ "); /*escrevendo a melhor solucao*/

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
    getchar();
    
    MPI_Finalize();
    return 0;
}