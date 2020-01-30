#include <iostream>
#include <list>
#include <pthread.h>
using namespace std;

void *produce(void * arg);
void *consume(void * arg);

int *buff;
int pos;
int v, t;
pthread_t *producer;
pthread_t *consumer;
pthread_mutex_t lock;
pthread_cond_t condP;
pthread_cond_t condiC;

int main(int argc, char *argv[]){
  int p, c;
  srand(time(NULL));

  if(argc == 1){  
        printf("Digite o número de interações do produtor:\n");
        scanf("%d", &v);
        printf("Digite o número de Produtores e logo em seguida o número de consumidores:\n");
        scanf("%d %d",&p, &c);
        printf("Digite o tamanho desejado do buffer:\n");
        scanf("%d", &t);
    }
    else{
        if(argc == 5){
            v = atoi(argv[1]);
            p = atoi(argv[2]);
            c = atoi(argv[3]);
            t = atoi(argv[4]);
        }
        else{
            printf("Parâmetros incorretos, porfavor verifque a entrada.\n");
            exit(1);
        }
    }

  producer = (pthread_t *) malloc(p * sizeof(pthread_t));
  consumer = (pthread_t *) malloc(c * sizeof(pthread_t));
  buff = (int *) malloc(t * sizeof(int)); // Buffer alocado dinâmicamente

  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&condP, NULL);
  pthread_cond_init(&condiC, NULL);

  // Dispara os threads produtores
  for(int i = 0; i < p; i++){
    pthread_create(&producer[i], NULL, produce, &buff);
  }
  // Dispara os threads consumidores
  for(int i = 0; i < c; i++){
    pthread_create(&consumer[i], NULL, consume, &buff);
  }
  //Retorna as threads disparadas para a main
  for(int i = 0; i < p; i++){
    pthread_join(producer[i], NULL);
  }
  //Coloca -1 como flag para o consumidor parar de consumir
  for(int i = 0; i < c ; i++){
    buff[pos] = -1;
    pos++;
    pthread_cond_signal(&condP);
  }

  for(int i = 0; i < c; i++){
    pthread_join(consumer[i], NULL);
  }

  return 0;
}
//V = número de interações(número que cada produtor tera que executar para criar um item)
void *produce(void * arg){
  for(int i = 0; i < v; i++){
    //Locka o mutex para os produtores (e apenas eles) utilizarem o buffer dinâmico
    pthread_mutex_lock(&lock);
    while(pos == t-1){
      //Espera o buffer criar uma celula de memória para produzir
      pthread_cond_wait(&condiC, &lock);
    }
  buff[pos] = rand();
  pos++;
  //Sinal que o Produtor da para o consumidor indicando que tem items no buffer
  pthread_cond_signal(&condP);
  pthread_mutex_unlock(&lock);
  }
  return NULL;
}

void *consume(void * arg){
  int value;
  bool prime;

  while(1){
    //Locka o mutex para os consumidores (e apenas eles) utilizarem o buffer dinâmico
    pthread_mutex_lock(&lock);
    //Enquanto o buffer estiver vazio, não se pode consumir nada
    while(pos == 0){
      pthread_cond_wait(&condP, &lock);
    }
  value = buff[0];
  pos--;
  buff[0] = 0;

  //Atualiza o buffer retirando o primeiro item, seguindo a política fifo 
  for (int i = 0; i < pos; i++){
    buff[i] = buff[i + 1];
  }

  if(value == -1){
    pthread_mutex_unlock(&lock);
    break;
    } else{
      //Lógica de número primo
      prime = true;
      for(int i = 2; i <= (int)(value/2); i++){ 
        if(value % i == 0){
          prime = false;
          break;
        }
      }
      if(prime){
        cout << "[" << pthread_self() << ":" << value << "]" << endl;
      }
  }
  pthread_cond_signal(&condiC);
  pthread_mutex_unlock(&lock);
  }
  return NULL;
}
