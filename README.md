# SO
Este é o repositório do trabalho de Sistemas Operacionais do grupo Cofee Lovers

>>>Instrução para a compilação 

As instruções para a compilação do programa no sistema operacional Linux seguem as seguintes etapas:
Primeiramente para compilar o programa é necessário a instalação da biblioteca POSIX threads, o que pode ser realizado em sistemas ubuntu com o seguinte comando:
sudo apt-get install libpthread-stubs0-dev

Após a instalação da biblioteca, vamos compilar o programa com o seguinte comando:
gcc -pthread <nomedoprograma.c> -o nome 

Ex: gcc -pthread SOprojeto.c -o so

Pegar o arquivo que contém a matriz NxM, que será gerada aleatoriamente a partir da execução de um programa que o professor disponibilizou no diretório no github da disciplina e copiar um número qualquer para a realização do teste do programa.
Ex: 2574284576,739285

Volte para o terminal e insira os seguintes comandos e tecle enter:
./nome qtd_linhas_do_arquivo qtd_colunas_do_arquivo num_threads valor_buscado nome_arquivo
   Ex: ./so  1000 1000 2 2574284576,739285 matrix.txt
A saída do programa será o seguinte:
Criando a thread numero: 1….
Criando a thread numero: n		
Obs: a saída dessa parte se altera de acordo com o num_thread escolhido
	Busca terminada, tempo gasto : xxx.xx s
	Linha: x, Coluna: y
