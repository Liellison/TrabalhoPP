#include <stdio.h>
#include <stdlib.h>
#include "funcoes.h"
#include <time.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char* argv[]){
    FILE *fileIn = NULL, *fileOUT = NULL;
    int numeroDeCidades, populacao, numeroDeGeracoes,j;
    int **pais = NULL;
    /*int N =8;*/
    int **geracao = NULL;
    float **matrizDeDistancia = NULL;
    float distancia;
    float ResultPrimeira, ResultSegunda;
    /* inicializacao mpi*/
    int id, tam, i;
    /*MPI_Status status;*/

    MPI_Init(&argc,&argv); 
    MPI_Comm_rank(MPI_COMM_WORLD, &id); 
    MPI_Comm_size (MPI_COMM_WORLD, &tam);

    srand(time(0)+id);
    /*int local_nrows = N / tam; */

    if (id==0) {
        /*inicio leitura do arquivo*/
        fileIn = fopen(argv[1],"r");
        if(fileIn == NULL){
            printf("Arquivo em branco");
            system ("PAUSE");
            return 1;
        }
        /*Arquivo de entrada tem que conter somente numero, por favor retirar nomes nos testes*/
        printf("Comecando a ler o arquivo\n");
        /*verificando o formato do arquivo de entrada*/
        if(fscanf(fileIn,"%d",&numeroDeCidades)!=1){
            printf("Formato de arquivo incorreto");
            return 1;
        }
        printf("Numero de Cidades: %d\n",numeroDeCidades);
        fscanf(fileIn, "%d", &populacao);
        printf("Numero de individuos na populacao: %d\n",populacao);
        fscanf(fileIn, "%d",&numeroDeGeracoes);
        printf("Melhor numero de geracoes: %d\n",numeroDeGeracoes);

        matrizDeDistancia =  (float**)malloc(sizeof(float *)*numeroDeCidades);

        for(i=0;i<numeroDeCidades;i++) {
            matrizDeDistancia[i]= (float*)malloc(sizeof(float)*numeroDeCidades);
            for(j=0;j<numeroDeCidades;j++) {
                fscanf(fileIn,"%f",&distancia);
                matrizDeDistancia[i][j]=distancia;
            }
        }
        
        fclose(fileIn);

        pais = (int **) malloc(sizeof(int *) * populacao);

        criaPais(pais, populacao, numeroDeCidades);
        clasificacao(pais, numeroDeCidades, populacao, matrizDeDistancia);

        /* Como somente o processo de id = 0 lê o arquivo, ele deve enviar os dados lidos para os demais processos */
        MPI_Bcast(&numeroDeCidades, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&populacao, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&numeroDeGeracoes, 1, MPI_INT, 0, MPI_COMM_WORLD);
        for(i = 0; i < numeroDeCidades; i++)
            MPI_Bcast(matrizDeDistancia[i], numeroDeCidades, MPI_FLOAT, 0, MPI_COMM_WORLD);
        for(i = 0; i < populacao; i++)
            MPI_Bcast(pais[i], numeroDeCidades, MPI_INT, 0, MPI_COMM_WORLD);
        
    }else{ /* Se somente o processo de id = 0 lê o arquivo, os demais processos devem receber os dados enviados por ele */

        MPI_Bcast(&numeroDeCidades, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&populacao, 1, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(&numeroDeGeracoes, 1, MPI_INT, 0, MPI_COMM_WORLD);
        matrizDeDistancia =  (float**)malloc(sizeof(float *)*numeroDeCidades);

        for(i=0;i<numeroDeCidades;i++) {
            matrizDeDistancia[i]= (float*)malloc(sizeof(float)*(numeroDeCidades));
            MPI_Bcast(matrizDeDistancia[i], numeroDeCidades, MPI_FLOAT, 0, MPI_COMM_WORLD);
        }

        pais = (int **)malloc(sizeof(int *)*populacao);
        for(i = 0; i < populacao; i++) {
            pais[i] = (int *)malloc(sizeof(int)*numeroDeCidades);
            MPI_Bcast(pais[i], numeroDeCidades, MPI_INT, 0, MPI_COMM_WORLD);
        }
            
    }

    fileOUT =fopen(argv[2],"w");

    geracao = (int **)malloc(sizeof(int *)*populacao);
    if (id==0) {
	    for(i=0;i<numeroDeGeracoes/2;i++){

	        propagacao(pais,geracao,populacao,numeroDeCidades);
	        clasificacao(geracao,numeroDeCidades,populacao,matrizDeDistancia);

	        if(i%10000==0)
	            printf("O caminho mais curto em %d Geracao do primeiro: %.0f\n",i,calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
	        fprintf(fileOUT,"%f\n", calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
	        ResultPrimeira = calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia);
	        mutacao(geracao,numeroDeCidades,populacao);
	        novo(geracao,pais,populacao,numeroDeCidades);

	        MPI_Bcast(&ResultPrimeira, 1, MPI_INT, 0, MPI_COMM_WORLD);
	    }
	}else{
	    for(i=numeroDeGeracoes/2;i<numeroDeGeracoes;i++){

	        propagacao(pais,geracao,populacao,numeroDeCidades);
	        clasificacao(geracao,numeroDeCidades,populacao,matrizDeDistancia);

	        if(i%10000==0)
	            printf("O caminho mais curto em %d Geracao do segundo: %.0f\n",i,calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
	        fprintf(fileOUT,"%f\n", calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
	        ResultSegunda = calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia);
	        mutacao(geracao,numeroDeCidades,populacao);
	        novo(geracao,pais,populacao,numeroDeCidades);

   	        MPI_Bcast(&ResultSegunda, 1, MPI_INT, 0, MPI_COMM_WORLD);

	    }
	}

    fclose(fileOUT);
    if (id==0) {
    	if (ResultPrimeira >= ResultSegunda)
    	{
    		printf("A melhor solucao de custo %.0f é:\n", &ResultSegunda);
    	}else{
    		printf("A melhor solucao de custo: %.0f é:\n", &ResultPrimeira);
    	}
    	printf("[ "); /*escrevendo a melhor solucao*/

	    for(i=0;i<numeroDeCidades;i++){
	        printf("%d ",pais[0][i]);
	    }
	    printf("]\n");
 	}
    //printf("A melhor solucao de custo %f e:\n",calcularCusto(numeroDeCidades,geracao[0], matrizDeDistancia));
    
    for(i=0;i<numeroDeCidades;i++){
        free(matrizDeDistancia[i]);
        break;
    }
    free(matrizDeDistancia);
    for(i=0;i<populacao;i++){
        free(pais[i]);
        free(geracao[i]);
        break;
    }
    free(pais);
    free(geracao);

    MPI_Finalize();
    return 0;
}