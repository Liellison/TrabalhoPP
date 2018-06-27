#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

void primeira_ninhada(int tam, int population_size, int MainVect[], int population[]){
B
	//printf("\n\n-- Criando a primeira população aleatória -> "); //pode ser apagado...

    int origin_city, actual_city;
    int cont, i, j;

    for(cont=0; cont<population_size; cont++)
	{
       population[(cont * (tam+1)) + tam] = 0;
       origin_city = -1;
       //printf("1.  no laço para geração de população! \n");
       //preenchendo o vetor de 100 posi��es. //modificar pra fazer os vetores das cidades
        for(i=0; i<tam; i++)
        {
            
            actual_city = (rand()%tam);

            if(origin_city != -1){
                while( origin_city != -1 && MainVect[(tam * origin_city) + actual_city] == 0) {
                   actual_city = (rand()%tam);
                }

                //verificando se o n�mero j� existe, afim de evitar elementos repetidos
                for(j=0; j<i; j++)
                {
                    if(population[(cont * (tam+1)) + j] == actual_city)
                    {
                        actual_city = (rand()%tam);
                        j=0;
                    }
                }
        
                // printf("--- origin: %d, destiny: %d =>", origin_city, actual_city);
                // printf(" distancy: %d ---\n", MainVect[origin_city][actual_city]);
                population[(cont * (tam+1)) + tam] += MainVect[(tam * origin_city) + actual_city];
            }
            population[(cont * (tam+1)) + i] = actual_city;
            origin_city = actual_city;
        }                     
    }
}
