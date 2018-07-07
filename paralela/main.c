#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
 
int desenhar (int intervalo);
int checkRepeticoes (int *cromossomo, int nn, int numero);
int *aleatorio (int numeroOf);
void criaPais (int **pais, int numeroDeCidades, int numeroPais);
float calcularCusto(int tamanho, int *solucao, float **matrizDeDistancia);
//void clasificacao (int **solucoes, int numeroCidade, int populacao, float **matrizDeDistancia);
void propagacao (int **pais, int **geracao, int numeroPais, int tamanhoGene);
void mutacao (int **geracao,int comprimentoGene ,int populacao);
void novo (int **A, int **B, int nLinhas, int nColunas);
 
/* uma funcao que retorna um numero aleatorio no intervalo <0;-1> */
int desenhar(int intervalo){return(rand()%intervalo);}
/* uma funcao que verifica se um gene repetitivo (numero da cidade) nao e desenhado no cromossomo */
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
/* funcao que retorna o ponteiro para a matriz de solucoes desenhadas 
 numeros (ordem das cidades visitadas) nao podem ser repetidos */
int *aleatorio(int numeroOf){
    int *nDesenhados;
    int sorteio;
    int numero;
 
    sorteio = 0; /* Contador de numeros sorteados */
    nDesenhados = (int*)malloc((sizeof(int))*numeroOf); /*alocacao de memoria para tabelas numericas */
    do{ /* desenhando os proximos numeros e verificando se nao ha duplicatas*/
        numero = desenhar(numeroOf);
        if(checkRepeticoes(nDesenhados, sorteio, numero)==0){
            nDesenhados[sorteio]=numero;
            sorteio++;
        }
    }while (sorteio < numeroOf);
    return nDesenhados;
}
/* A funcao geradora da primeira geracao de solucaes - pais */
void criaPais(int **pais, int numeroDeCidades, int numeroPais){
    int i;
    for(i=0; i<numeroPais; i++){
        pais[i] = aleatorio(numeroDeCidades); /*e o pai aponta para a solucao desenhada*/
    }
}
/* Funcao calculando o custo da viagem entre cidades individuais 
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
/* funcao de selecao de solucoes (cromossomos) de acordo com seus custos */
/* classificao por selecao */
void *clasificacao(void *arg){
    int i, j, min;
    int *temp;
    int id = *(int *)arg;
	int **solucoes;
	int numeroCidade, populacao; 
	float **matrizDeDistancia; 

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
    pthread_exit(NULL);
}
/* funcao de propagacao, operando com base no cruzamento */
void propagacao (int **pais, int **geracao, int numeroPais, int tamanhoGene){
    int i,j,k,temp;
    int *aVisita, visitados;
 
    srand(time(NULL));
    aVisita = (int*)malloc(sizeof(int)*tamanhoGene);
    geracao[0] = (int*)malloc(sizeof(int)*tamanhoGene);
    for(i=0;i<tamanhoGene;i++) /*o primeiro individuo de uma nova geracao*/
        (geracao[0])[i] = (pais[0])[i]; /*torna-se o melhor individuo da velha geracao*/
    for(i=1;i<numeroPais;i++){ /*o resto da geracao e criado pelo cruzamento*/
        visitados = 0;
        geracao[i]=(int*)malloc(sizeof(int)*tamanhoGene);
        k=(rand()%(numeroPais/2)); /*k e um numero aleatorio no intervalo <0; (numero de nascimentos / 2) -1> para cruzar apenas individuos melhores do conjunto pai*/
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
/*funcao que faz uma mutacao*/
void mutacao (int **geracao,int comprimentoGene ,int populacao){
    int a, b, c, j, temp, los;
    srand(time(NULL));
    for(j=0;j<populacao;j++){
        los = rand()%2;
        if(los==1){/*a probabilidade de mutacao cromossomica e de 50%*/
            a = (rand()%(populacao-1)+1); /*desenhando um numero da faixa <1; populacao-1>*/
            b = rand()%comprimentoGene; /*o melhor individuo nao ser e mutado!*/
            c = rand()%comprimentoGene;
 
            temp = (geracao[a])[b];
            (geracao[a])[b] = (geracao[a])[c];
            (geracao[a])[c] = temp;
        }
    }
}
/*uma funcao que prescreve dados de um array bidimensional A para um array bidimensional B*/
void novo (int **A, int **B, int nLinhas, int nColunas){
    int i,j;
    for(i=0;i<nLinhas;i++){
        for(j=0;j<nColunas;j++){
            (B[i])[j]=(A[i])[j];
        }
    }
}

int main(int argc, char* argv[]){
    FILE *fileIn, *fileOUT;
    int numeroDeCidades, populacao, numeroDeGeracoes,i, j;
    int **pais;
    int **geracao;
    float **matrizDeDistancia;
    float distancia;
    int id =1;
    pthread_t thread;
    pthread_attr_t attr;
    
    srand(time(0));

    fileIn = fopen(argv[1],"r");
    if(fileIn == NULL){
        printf("Arquivo em branco");
        getchar();
        return 1;
    }
    printf("Comecando a ler o arquivo\n");
    while(feof(fileIn) == 0){
        if(fscanf(fileIn,"%d",&numeroDeCidades)!=1){/*verificando o formato do arquivo de entrada*/
            printf("Formato de arquivo incorreto");
            getchar();
            return 1;
        }else{
            printf("Numero de Cidades: %d\n",numeroDeCidades);
            fscanf(fileIn, "%d", &populacao);
            printf("Numero de individuos na populacao: %d\n",populacao);
            fscanf(fileIn, "%d",&numeroDeGeracoes);
            printf("Numero de geracoes: %d\n",numeroDeGeracoes);
 
			matrizDeDistancia = (float**)malloc(sizeof(float *)*numeroDeCidades); 
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
        //clasificacao(pais, numeroDeCidades, populacao, matrizDeDistancia);
 
        fileOUT =fopen(argv[2],"w");
        for(i=0;i<numeroDeGeracoes;i++){
            propagacao(pais,geracao,populacao,numeroDeCidades);
            //clasificacao(geracao,numeroDeCidades,populacao,matrizDeDistancia);
            //pthread_mutex_init(&mutex, NULL);
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
			pthread_create(&thread, &attr, clasificacao, (void *)&id);
			pthread_join(thread, NULL);


 
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
		break;
    }
    return 0;
   }
 