#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define MAX_CLNT 10
#define HEIGHT 12
#define WIDTH 6

typedef struct player{
    int online;
    char field[HEIGHT][WIDTH];
    int score;
}player;

void * handle_clnt(void * arg);
void error_handling(char * msg);
void * rcv_and_snd(void * arg);

int clnt_cnt = 0;
int waiting_player = 0;
int clnt_socks[MAX_CLNT];
pthread_mutex_t mutx;

int main(int argc, char *argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_adr, clnt_adr;
    int clnt_adr_sz;
    pthread_t t_id;
    
    pthread_mutex_init(&mutx, NULL);
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    
    memset(&serv_adr , 0 , sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(7001);
    
    if(bind(serv_sock,(struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind error");
    if(listen(serv_sock, 5) == -1)
        error_handling("listen error");
    
    while(1){
        clnt_adr_sz = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_adr,&clnt_adr_sz);
        printf("client connect\n");
        pthread_mutex_lock(&mutx);
        clnt_socks[clnt_cnt++]= clnt_sock;
        waiting_player++;
        
        
        
        if(waiting_player ==2){

     
            pthread_create(&t_id, NULL, handle_clnt, NULL);
            pthread_detach(t_id);
         
            waiting_player = 0;
            printf("match start\n");
        }
        pthread_mutex_unlock(&mutx);
        }
    close(serv_sock);
    return 0;
    
}

void * handle_clnt(void * arg){
    pthread_t t1, t2;
    int i;
    pthread_mutex_lock(&mutx);
    int p1_socket_set[2];
    int p2_socket_set[2];
    p1_socket_set[0] = clnt_socks[clnt_cnt-1];
    p1_socket_set[1] = clnt_socks[clnt_cnt-2];
    p2_socket_set[0] = clnt_socks[clnt_cnt-2];
    p2_socket_set[1] = clnt_socks[clnt_cnt-1];
    pthread_mutex_unlock(&mutx);
    
    pthread_create(&t1,NULL,rcv_and_snd,(void*) p1_socket_set);
    pthread_create(&t2,NULL,rcv_and_snd,(void*) p2_socket_set);
    pthread_detach(t1);
    pthread_join(t2, NULL);
    usleep(100000);
    pthread_mutex_lock(&mutx);
    close(p1_socket_set[0]);
    close(p1_socket_set[1]);
   
    
    for(i=0; i < clnt_cnt ; i++){
        if(p1_socket_set[1] == clnt_socks[i]){
            while(i <= clnt_cnt - 1){
                clnt_socks[i] = clnt_socks[i+2];
                i++;
            }
            break;
        }
    }
    clnt_cnt -= 2;
    pthread_mutex_unlock(&mutx);
    printf("match finished\n\n");
    
    return NULL;
}
void * rcv_and_snd(void * arg){
    player temp_data;
    int rvc_socket;
    int snd_socket;
    rvc_socket = ((int*) arg)[0];
    snd_socket = ((int*) arg)[1];
    while(1){

        while(read(rvc_socket,(player *) &temp_data,sizeof(temp_data))<0);
        write(snd_socket, (player *)&temp_data, sizeof(temp_data));
        if (!temp_data.online)
            break;
    }
}


void error_handling(char * message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
    
}