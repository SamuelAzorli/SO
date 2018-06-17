#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

typedef struct lnha
{
    double dado;
    int linha, coluna;//para identificar o número da linha e coluna;
    struct lnha *prox;//como o acesso só ocorre em uma direção, não precisa usar ponteiro para cima, baixo e esquerda
}lin;

typedef struct coluna
{
/*Esta estrutura é formada por vários nós cada um deles contendo o início de uma linha*/
    lin *inicio;
    struct coluna *prox;
}col;

typedef struct acho
{
/*essa estrutura guarda as linhas e colunas em que o valor foi encontrado*/
    int linha;
    int coluna;
    struct acho *prox;
}achados;


col *inicio;
achados *aini;
int quanthreads;
int linhas;
int colunas;
double valor;

int inicializar(char *nome);
void *buscar(void *num);

int main(int argc, char *argv[])
{

    FILE *arq;
    char *nome;
    clock_t start, end;
    long l;//variavel de loop
    unsigned int erro=0;
    //int tam;
    int erro2;//para detectar erro
    achados *percorre;
    switch(argc)
    {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        printf("Argumentos insuficientes!/n");
        printf("Uso: <numero de linhas> <numero de colunas> <numero de threads> <valor a ser buscado> <nome do arquivo>\n\a");
        break;
    case 6:
        nome=argv[5];
        linhas=atoi(argv[1]);
        colunas=atoi(argv[2]);
        quanthreads=atoi(argv[3]);
        valor=atof(argv[4]);
        erro2=inicializar(nome);
        if(erro2==1)
        {
            printf("Nao foi possivel abrir o arquivo ou nome do arquivo invalido \a\n");
            pthread_exit(0);
        }

        pthread_t *thds;
        thds=(pthread_t *)malloc(quanthreads*sizeof(pthread_t));
        start=clock();
        for(l=0;l<quanthreads;l++)
        {
            printf("Criando a thread numero: %ld\n", l);
            erro=pthread_create(&thds[l], NULL, buscar, (void *) l);
            if(erro)
            {
                printf("Nao foi possivel criar a %ld thread\n", l+1);
                exit(-1);//está no tutorial em ingles que foi passado
            }
        }

        for(l=0;l<quanthreads; l++)
        {
            pthread_join(thds[l], NULL);//espera todas as threads terminarem
        }
        end=clock();
        printf("Busca terminada, tempo gasto: %lf s\n\n",(double)(end-start)/(double)(CLOCKS_PER_SEC));
        percorre=aini;
        if(aini==NULL)
        {
            printf("Nao existe o valor na matriz!\a\n");
        }
        else
        while(percorre!=NULL)
        {
            printf("Linha: %d, Coluna: %d; \n", percorre->linha, percorre->coluna);
            percorre=percorre->prox;
        }
        break;
    default:
        printf("Muitos parametros!/n");
        printf("Uso: <numero de linhas> <numero de colunas> <numero de threads> <valor a ser buscado> <nome do arquivo>\n\a");
        break;
    }

    /*printf("Insira o nome do arquivo:");
    fgets(nome, 50, stdin);
    tam=strlen(nome);
    nome[tam-1]='\0';
    printf("Insira a quantidade de threads:");
    scanf("%d", &quanthreads);
    printf("Insira a quantidade de linhas:");
    scanf("%d", &linhas);
    printf("Insira a quantidade de colunas:");
    scanf("%d", &colunas);
    printf("Insira o valor a ser buscado:");
    scanf("%lf", &valor);*/

    pthread_exit(0);
}

int inicializar(char *nome)
{
    FILE *arq;
    arq=fopen(nome, "r");
    aini=NULL;
    if(arq==NULL)
    {
        return 1;
    }
    int c;
    int l;
    lin *novo, *aux;
    col *percorre, *colnovo;
    char temp;//isso foi para resolver o warning do fscanf para ler o "\n"

    for(l=1; l<=linhas; l++)
    {
        colnovo=(col*)malloc(sizeof(col));
        colnovo->inicio=NULL;
        colnovo->prox=NULL;
        if(inicio==NULL)
        {
            inicio=colnovo;
        }
        else
        {
            percorre=inicio;
            while(percorre->prox!=NULL)
            {
                percorre=percorre->prox;
            }
            percorre->prox=colnovo;
        }
        for(c=1; c<=colunas; c++)
        {

            novo=(lin*)malloc(sizeof(lin));
            fscanf(arq," %lf ", &novo->dado);
            //printf("%lf ", novo->dado);
            novo->linha=l;
            novo->coluna=c;
            novo->prox=NULL;
            if(colnovo->inicio==NULL)
            {
                colnovo->inicio=novo;
            }
            else
            {
                aux=colnovo->inicio;
                while(aux->prox!=NULL)
                {
                    aux=aux->prox;
                }
                aux->prox=novo;
            }
        }
        fscanf(arq," %c", &temp);//ler o '\n'
        //printf("\n");
    }
    fclose(arq);
    return 0;
}
/*====================================================================================*/
void *buscar(void *num)
{
    long id= (long) num;
    col *percorre;
    percorre=inicio;
    int nlinha=1;
    lin *aux;
    achados *achx;
    achados *novo;
    int proxlinha=id;//indica qual será a proxima linha que a thread lerá

    while(percorre!=NULL)
    {
        if(nlinha==proxlinha)
        {
            aux=percorre->inicio;
            while(aux!=NULL)//percorre dentro da linha
            {
                if(aux->dado==valor)
                {
                   novo=(achados*)malloc(sizeof(achados));
                    novo->linha=aux->linha;
                    novo->coluna=aux->coluna;
                    novo->prox=NULL;
                    if(aini==NULL)
                    {
                        aini=novo;
                    }
                    else
                    {
                        achx=novo;
                        while(achx->prox!=NULL)
                        {
                            achx=achx->prox;
                        }
                    achx->prox=novo;
                    }
                }

                aux=aux->prox;
            }
            proxlinha+=id;
        }
        nlinha++;
        percorre=percorre->prox;
    }
    pthread_exit(num);
}
