#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

typedef struct s_linha
{
    double dado;
    int linha, coluna;//para identificar o número da linha e coluna;
    struct s_linha *prox;//como o acesso só ocorre em uma direção, não precisa usar ponteiro para cima, baixo e esquerda
}t_linha;

typedef struct s_indice
{
/*Esta estrutura é formada por vários nós cada um deles contendo o início de uma linha*/
    t_linha *inicio;
    struct s_indice *prox;
}t_indice;

typedef struct s_encontrou
{
/*essa estrutura guarda as linhas e colunas em que o valor foi encontrado*/
    int linha;
    int coluna;
    struct s_encontrou *prox;
}t_encontrou;


t_indice *ti_inicio;//inicio do tipo indice
t_encontrou *te_inicio;//inicio do tipo encontrou
int quanthreads;//quantidade de threads
int linhas;//quantidade de linhas
int colunas;//quantidade de colunas
double valor;//valor que será buscado
//pthread_mutex_t mutex;//semáforo

/*========escopo das funções========*/
int inicializar(char *nome);
void *buscar(void *num);
/*==================================*/

int main(int argc, char *argv[])
{
    char *nome;//para armazenar o nome do arquivo
    clock_t start, end;
    /*variáveis para medir o tempo, encontrei no seguinte site:
    https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/*/
    long int l;//variavel de loop
    unsigned int erro1=0;
    int erro2;//erro1 e erro2 para detectar erro usando o retorno das funções
    t_encontrou *percorre;//auxiliar para percorrer a lista encadeada

    void *status;
    pthread_attr_t atributos;
    pthread_attr_init(&atributos);
    pthread_attr_setdetachstate(&atributos, PTHREAD_CREATE_JOINABLE);

    switch(argc)
    //trata os inputs da linha de comando
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
        erro2=inicializar(nome);//inicializa o programa carregando os dados do arquivo na memória
        if(erro2==1)
        {
            printf("Nao foi possivel abrir o arquivo ou nome do arquivo invalido \a\n");
            pthread_exit(0);
        }

        //alocação do vetor das threads:
        pthread_t *threads;
        threads=(pthread_t *)malloc(quanthreads*sizeof(pthread_t));

        start=clock();//começa a contar o tempo
        for(l=0;l<quanthreads;l++)
        //loop de criação das threads
        {
            printf("Criando a thread numero: %ld\n", l+1);
            erro1=pthread_create(&threads[l], &atributos, buscar, (void *) (l+1));//as threads executam a função "buscar"
            if(erro1)//tratamento de erro do pthread_create
            {
                printf("Nao foi possivel criar a %ld thread\n", l+1);
                exit(-1);
            }
        }

        for(l=0;l<quanthreads; l++)
        {
            pthread_join(threads[l], &status);
            //espera todas as threads terminarem
        }
        end=clock();//termina a contagem de tempo
        printf("Busca terminada, tempo gasto: %lf s\n\n",(double)(end-start)/(double)(CLOCKS_PER_SEC));
        pthread_attr_destroy(&atributos);

        percorre=te_inicio;
        if(te_inicio==NULL)
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

    pthread_exit(0);
}

int inicializar(char *nome)
{
    FILE *arq;
    arq=fopen(nome, "r");//abre o arquivo
    te_inicio=NULL;
    if(arq==NULL)//trata erro ao abrir o arquivo
    {
        return 1;
    }
    int c;//variavel de loop para alocação da linha
    int l;//variavel de loop para alocação do indice
    t_linha *novo, *tl_percorre;
    t_indice *ti_percorre, *ti_novo;
    char temp;//variável para receber o '/n' do fscanf

    for(l=1; l<=linhas; l++)
    //esse loop aloca o tipo indice
    {
        ti_novo=(t_indice*)malloc(sizeof(t_indice));
        ti_novo->inicio=NULL;
        ti_novo->prox=NULL;
        if(ti_inicio==NULL)
        {
            ti_inicio=ti_novo;
        }
        else
        {
            ti_percorre=ti_inicio;
            while(ti_percorre->prox!=NULL)
            /*esse loop coloca o ponteiro na ultima posição, para inserir o novo nó no fim,
            isso deixa o código menos eficiente em termos de desempenho mas mantém a ordem
            da matriz na memória igual a que está no arquivo, sem isso teria que inserir o
            novo nó no inicio*/
            {
                ti_percorre=ti_percorre->prox;
            }
            ti_percorre->prox=ti_novo;
        }
        for(c=1; c<=colunas; c++)
        //loop para alocar a linha
        {
            novo=(t_linha*)malloc(sizeof(t_linha));
            fscanf(arq," %lf ", &novo->dado);
            //printf("%lf ", novo->dado);
            novo->linha=l;
            novo->coluna=c;
            novo->prox=NULL;
            if(ti_novo->inicio==NULL)
            {
                ti_novo->inicio=novo;
            }
            else
            {
                tl_percorre=ti_novo->inicio;
                while(tl_percorre->prox!=NULL)
                {
                    tl_percorre=tl_percorre->prox;
                }
                tl_percorre->prox=novo;
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
    long id= (long) num;//numero para identificar a thread (é diferente do thread_id)
    t_indice *ti_percorre;
    ti_percorre=ti_inicio;
    int nlinha=1;//isso é para identificar qual é a proxima linha que a thread irá executar
    t_linha *tl_percorre;
    t_encontrou *te_percorre;
    t_encontrou *novo;
    int proxlinha=id;//indica qual será a proxima linha que a thread lerá

    while(ti_percorre!=NULL)
    {
        if(nlinha==proxlinha)
        {
            tl_percorre=ti_percorre->inicio;
            while(tl_percorre!=NULL)//percorre dentro da linha
            {
                if(tl_percorre->dado==valor)
                {
                   novo=(t_encontrou*)malloc(sizeof(t_encontrou));
                    novo->linha=tl_percorre->linha;
                    novo->coluna=tl_percorre->coluna;
                    novo->prox=NULL;
                    if(te_inicio==NULL)
                    {
                        te_inicio=novo;
                    }
                    else
                    {
                        te_percorre=novo;
                        while(te_percorre->prox!=NULL)
                        {
                            te_percorre=te_percorre->prox;
                        }
                    te_percorre->prox=novo;
                    }
                }

                tl_percorre=tl_percorre->prox;
            }
            proxlinha+=quanthreads;
        }
        nlinha++;
        ti_percorre=ti_percorre->prox;
    }
    pthread_exit(num);
}
