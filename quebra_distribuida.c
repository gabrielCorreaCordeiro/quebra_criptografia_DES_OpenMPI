#include <stdio.h>
#include <mpi.h>
#include <string.h>

#include <stdlib.h>

#include <string.h>
#include <stdbool.h>
#include <unistd.h>

void salvacao();



int main(int argc, char** argv) {
    // inicializa o MPI
    MPI_Init(&argc, &argv);

    // numero de processos
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // recupera o rank dos processos
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // se for o mestre, gera as chaves para todos os escravos
    if(world_rank == 0){
      register int count = 0;
      while(true){
        for(register int i = 1; i < world_size; i++){
          char wr[30];
          sprintf(wr, "%i", i);
          strcat(wr,"_");
          char wk[120000];
          sprintf(wk, "%i",count);
          char concat[90000] = "./exec -g ./chaves/";
          strcat(concat, wr);
          strcat(concat, wk);
          strcat(concat, ".key");
          const char *param = concat;

          system(param);
          count++;
        }
      }
    }else{
      salvacao(world_rank);
      MPI_Abort(MPI_COMM_WORLD, 0);
      exit(1);
    }
}




void salvacao(int world_rank){
      sleep(1);
  // variavel para abrir o arquivo encriptaddo
      FILE *fp;

      // variavel para abrir o arquivo à ser testado
      FILE *fp2;

      // transformo o world_rank em vetor de char
      char string_rank[30];
      register unsigned int count = 0;
      char string_count[3000];
      while(true){
        sprintf(string_count, "%i", count);
        sprintf(string_rank, "%i", world_rank);

        char concac[6000] = "./exec -d ./chaves/";
        strcat(concac,string_rank);
        strcat(concac, "_");
        strcat(concac, string_count);
        strcat(concac,".key encrip.enc ./saida/");
        strcat(concac,string_rank);
        strcat(concac, "_saida.txt");
        const char *param1 = concac;
        // abordagem pensada para desencriptar, onde eu tento desencriptar com minha chave aleatória, e encripto novamente para futuramente comparar os 2 arquivos encriptados
        system(param1);
        

        char concac2[6000] = "./exec -e ./chaves/";
        strcat(concac2, string_rank);
        strcat(concac2, "_");
        strcat(concac2, string_count);
        strcat(concac2, ".key ./saida/");
        strcat(concac2, string_rank);
        strcat(concac2, "_saida.txt");
        strcat(concac2, " ./saida/tentativa/");
        strcat(concac2, string_rank);
        strcat(concac2, "_tentativa.enc");
        const char *param2 = concac2;
        
        // encriptografo de novo para tentar resolver o problema       
        system(concac2);
        
        
        // abro o arquivo encriptado para ler os bits
        fp = fopen("./encrip.enc", "rb");

        
        // abro o arquivo que tentei desencriptar
        char veri1[8096];
        register int f = 0;
        long size1;
        fseek(fp, 0, SEEK_END);
        size1 = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        for(register int i=0; i < size1 ; i++){
            f = fgetc(fp);
            veri1[i] = f;
        }
        fclose(fp);

        
        //printf("--------------------------------------------- \n");
        // gambiarra para pegar o valor do world_rank e colocar na string para ler o arquivo correspondente à thread
        char gambiarra[8000] = "";
        strcat(gambiarra,string_rank);
        strcat(gambiarra,"_tentativa.enc");
        
        char diretorio[60] = "./saida/tentativa/";
        strcat(diretorio, gambiarra);
        const char* path = diretorio;

        fp2 = fopen(path, "rb");

        

        // abro o arquivo que tentei desencriptar
        char veri2[8096];
        int j = 0;
        long size2;
        fseek(fp2, 0, SEEK_END);
        size2 = ftell(fp2);
        fseek(fp2, 0, SEEK_SET);
        for(register int i=0; i < size2 ; i++){
            j = fgetc(fp2);
            veri2[i] = j; 
            
        }
        fclose(fp2);

        
        
        register int teste = 0;
        //printf(" %d == %d",size1,size2);
        if(size1 == size2){
            teste = 1;
            for(register int i =0; i < size1; i++){
              //printf("%X == %X \n",veri1[i], veri2[i]);
                if(veri1[i] != veri2[i]){
                    teste = 0;
                }
            }
        }



        if(teste == 1){
          FILE *saida;

          
          char verifi[100] = "./saida/";
          strcat(verifi, string_rank);
          strcat(verifi, "_saida.txt");


          char verificado[100] = "./saida/";
          strcat(verificado, string_rank);
          strcat(verificado, "_teste.txt");

          char sai[700] = "file ";
          strcat(sai, verifi);
          strcat(sai, " | grep -i -E 'Non-ISO extended-ASCII text|data' > ");
          strcat(sai, verificado);
          const char* s = sai;
          
          system(s);

          char open[] = "./saida/";
          strcat(open, string_rank);
          strcat(open, "_teste.txt");

          const char* f = open;
          saida = fopen(f, "r");

          teste = 1;

          unsigned long sizefinal;
          fseek(saida, 0, SEEK_END);
          sizefinal = ftell(saida);
          fclose(saida);

          if(sizefinal != 0){
            teste = 0;
          }
          
          if(teste == 1){
            char respons[] = "";
            strcat(respons, string_rank);
            strcat(respons, "_");
            strcat(respons, string_count);
            strcat(respons, ".key");
            printf("A chave correta é a %s \n", respons);
            break;

          }

        }
        count++;
      }
}