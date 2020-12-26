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

typedef struct _match{
    player p1,p2;
    int p1_socket;
    int p2_socket;    
}match;

void * handle_clnt(void * arg);
void send_data(int rcv_sock,player play_temp, int len);
void error_handling(char * msg);


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
        pthread_mutex_unlock(&mutx);
        
        
        if(waiting_player ==2){
            pthread_create(&t_id, NULL, handle_clnt, NULL);
            pthread_detach(t_id);
            pthread_mutex_lock(&mutx);
            waiting_player = 0;
            pthread_mutex_unlock(&mutx);
            printf("match start\n");
            sleep(1);
        }
        
    }
    close(serv_sock);
    return 0;
    
}

void * handle_clnt(void * arg){
    
    match Match;
    int data_size =0, i;
    pthread_mutex_lock(&mutx);
    Match.p1_socket = clnt_socks[clnt_cnt-1];
    Match.p2_socket = clnt_socks[clnt_cnt-2];
    Match.p1.online = 0;
    Match.p2.online = 0;
    pthread_mutex_unlock(&mutx);
    
    /*
    while((data_size = read(clnt_sock,(player *) &play_temp , sizeof(play_temp)))!= 0)
        send_data(clnt_sock,play_temp,data_size);
        */
    while(1){
        read(Match.p1_socket,(player *) &Match.p1, sizeof(Match.p1));
        if(!Match.p1.online){
            send_data(Match.p2_socket,Match.p1,sizeof(Match.p1));
            break;
        }
        read(Match.p2_socket,(player *) &Match.p2, sizeof(Match.p2));
        if(!Match.p2.online ){
            send_data(Match.p1_socket,Match.p2,sizeof(Match.p2));
            break;
        }
        send_data(Match.p1_socket,Match.p2,sizeof(Match.p2));
        send_data(Match.p2_socket,Match.p1,sizeof(Match.p1));
    }
    usleep(100000);
    close(Match.p1_socket);
    close(Match.p2_socket);
    pthread_mutex_lock(&mutx);
    for(i=0; i < clnt_cnt ; i++){
        if(Match.p2_socket == clnt_socks[i]){
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
void send_data(int rcv_sock,player play_temp, int len){

    pthread_mutex_lock(&mutx);
    write(rcv_sock, (player*) &play_temp, len);
    pthread_mutex_unlock(&mutx);


}
void error_handling(char * message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
    
}