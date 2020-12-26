#include "puyo.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
			case MENU_PLAY: play(); break;
            case MULTI_PLAY: multi = 1; play(); break;
			case MENU_EXIT: exit=1; break;
			default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;
    
    if(multi){
        pthread_mutex_init(&mutx, NULL);
        connect_server();
    }
	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;
    
    opPlayer.online = 0;
    memset(opPlayer.field,0,sizeof(opPlayer.field));
    opPlayer.score = 0;

	start_color();
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_BLUE,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_YELLOW,COLOR_BLACK);
	init_pair(5,COLOR_WHITE,COLOR_BLACK);
	nextBlock[0]=rand()%10;
	nextBlock[1]=rand()%10;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;
	num_of_chains = 0;
    op_score = 0;
    attack_flag = 0;
    attack_score = 0;
    if(multi){
        while(1){
            if(opPlayer.online)
                break;
            sleep(1);
            
        }
    }


	DrawOutline();
	DrawField(field);
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
    
    

}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+6);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+6,4,8);
    
    if(multi){
        DrawBox(0,WIDTH + 17, HEIGHT, WIDTH);
        move(9,WIDTH+ 30);
        printw("SCORE");
        DrawBox(10,WIDTH+30,1,8);   
    }
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9,WIDTH+6);
	printw("SCORE");
	DrawBox(10,WIDTH+6,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
		case KEY_UP:
			break;
		case KEY_DOWN:
			break;
		case KEY_LEFT:
			break;
		case KEY_RIGHT:
			break;
		case ' ':	/* space key*/
			/*fall block*/
			break;
		case 'q':
		case 'Q':
			command = QUIT;
			break;
		default:
			command = NOTHING;
			break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	if(process_flag)
		return NOTHING;
	switch(command){
		case QUIT:
			ret = QUIT;
			break;
		case KEY_UP:
			if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
				blockRotate=(blockRotate+1)%4;
			break;
		case KEY_DOWN:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
				blockY++;
			break;
		case KEY_RIGHT:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
				blockX++;
			break;
		case KEY_LEFT:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
				blockX--;
			break;
		default:
			break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(char f[HEIGHT][WIDTH]){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]){
                if(field[j][i] == XBLOCK){
                    attron(A_REVERSE);
                    printw("X");
                    attroff(A_REVERSE);
                }
                else{
                    attron(COLOR_PAIR(f[j][i]));
                    attron(A_REVERSE);
                    printw(" ");
                    attroff(A_REVERSE);
                    attron(COLOR_PAIR(5));
                }
            }
			else printw(" ");
		}
	}
}


void PrintScore(int score){
	move(11,WIDTH+7);
	printw("%8d",score);
}
void printOpScore(){
    move(11,WIDTH+31);
    printw("%8d",opPlayer.score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < BLOCK_HEIGHT; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < BLOCK_WIDTH; j++ ){
			if( block[nextBlock[1]][0][i][j]){
				attron(COLOR_PAIR(block[nextBlock[1]][0][i][j]));
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
				attron(COLOR_PAIR(5));
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<BLOCK_HEIGHT;i++)
		for(j=0;j<BLOCK_WIDTH;j++){
			if(block[blockID][blockRotate][i][j] && i+y>=0){
				move(i+y+1,j+x+1);
				attron(COLOR_PAIR(block[blockID][blockRotate][i][j]));
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
				attron(COLOR_PAIR(5));
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
    
    
    
	do{
		if(timed_out==0 && !process_flag){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();

		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2,1,10);
			move(HEIGHT/2,WIDTH/2 +2);
			printw("Good-bye!!");
			refresh();
			getch();
            if(multi){
                pthread_mutex_lock(&mutx);
                me.online = 0;
                pthread_mutex_unlock(&mutx);
                write(sock,(player *)&me,sizeof(me));
                sleep(1);
                close(sock);
                
            }
			return;

		}
        if(multi&&!opPlayer.online)
            break;
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
    if(multi){
        move(HEIGHT/2,WIDTH/2-4);
	    printw("Other player exit!!");
        pthread_mutex_lock(&mutx);
        me.online = 0;
        pthread_mutex_unlock(&mutx);
        write(sock,(player *)&me,sizeof(me));
        
        close(sock);
    }
    else{
        move(HEIGHT/2,WIDTH/2-4);
	    printw("GameOver!!");
    }
    refresh();
	getch();

}

char menu(){
	printw("1. single play\n");
	printw("2. multi play \n");
	printw("4. exit\n");
	return wgetch(stdscr);
}



int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i,j;
	for(i=0 ; i < BLOCK_HEIGHT ; i++){
		for(j=0 ; j < BLOCK_WIDTH ; j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(blockX + j <= -1 || blockX + j >= WIDTH)
					return 0;
				else if(blockY + i >= HEIGHT)
					return 0;
				else if(f[i+blockY][j+blockX])
					return 0;
			}
		}
	}
	return 1;
}

int CheckFall(char f[HEIGHT][WIDTH]){
	int i,flag,j,top,k,gap,p;
	flag = 0;

	for(i=0; i < WIDTH ; i++){
		top = -1;
		gap = 0;
		for(j=0; j < HEIGHT ; j++){
			if(f[j][i]&&top == -1){
				top = j;
			}
			if(!f[j][i] && top != -1){
				flag = 1;
				gap = 0;
				for(k = j ; k < HEIGHT ; k++,gap++){
					if(f[k][i]){
						break;
					}

				}
				for(p=0; p < j - top; p++){
					k--;
					f[k][i] = f[k-gap][i];
					f[k-gap][i] = 0;
				}
				top += gap;

			}

		}




	}
	return flag;
	// 1 -> 블럭 떨어짐

}

void Chain(char f[HEIGHT][WIDTH], int y, int x, char puyo){
	for(int i = 0; i < 4; i++){
		int nx = dx[i] + x;
		int ny = dy[i] + y;

		if(0<= nx && nx < 6 && 0 <= ny && ny < 12){
			if(!visited[ny][nx] && f[ny][nx] == puyo){
				list.push_back(make_pair(ny,nx));
				visited[ny][nx] = 1;
				Chain(f, ny, nx, puyo);
			}
		}
	}
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	DrawField(f); 
	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);

}

void PuyoBomb(char f[HEIGHT][WIDTH]){


	int flag = 0;
	int color_count = 0;
	int num_of_all_puyo = 0;
	int puyo[4];

	list.clear();
	for(int i = 0 ; i < HEIGHT ; i++)
		memset(visited[i],0,sizeof(int)*WIDTH);
	memset(puyo,0,sizeof(puyo));
	for(int i = 0 ; i < 12; i++){
		for( int j = 0; j < 6 ;j++){
			if(!visited[i][j] && f[i][j]){
				list.push_back(make_pair(i,j));
				visited[i][j] = 1;
				Chain(f,i,j,f[i][j]);

				if(list.size() >= 4){
					color_count++;
					flag = 1;
					num_of_all_puyo += list.size();
					puyo[f[list[0].first][list[0].second]]++;

					for(int k = 0 ; k < list.size() ; k++){
						f[list[k].first][list[k].second] = 0;
                        DeleteXBlock(list[k].first,list[k].second);
					}

				}
				list.clear();

			}   
		}
	}
	CheckFall(f);
	DrawField(f);
	if(flag){
		num_of_chains++;
		score += CalScore(num_of_all_puyo,puyo,color_count);
		move(15,2);
		printw("chain : %d",num_of_chains);
		PuyoBomb(f);

	}


}

void DeleteXBlock(int y, int x){
    int ny,nx,i;
    for(i=0 ; i < 4 ; i++){
        ny = y + dy[i];
        nx = x + dx[i];
        if(ny >= 0 && ny < HEIGHT && nx >= 0 && nx < WIDTH){
            if(field[ny][nx] == XBLOCK){
                field[ny][nx] = 0;
            }
        }
    }
}

void BlockDown(int sig){

	if(process_flag)
		return;

	if(CheckToMove(field,nextBlock[0],blockRotate, blockY+1, blockX)){
		process_flag=0;
		blockY++;
		DrawField(field);
		DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);

	}
	else{
		alarm(10);
		num_of_chains = 0;
		process_flag = 1;

		if(blockY == -1)
			gameOver = 1;
		AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);

		CheckFall(field);

		PuyoBomb(field);

		nextBlock[0] = nextBlock[1];
		nextBlock[1] = rand() % 10;

		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH/2 -2;

        Attack(attack_score);
        CheckFall(field);
		DrawNextBlock(nextBlock);
		DrawField(field);
		PrintScore(score);




	}
	process_flag = 0;
	timed_out = 0;

}
void Attack(int s){
    if(!attack_flag)
        return;
    int block_cnt,i,j,div,re;
    
    block_cnt = attack_score / 200;
    div = block_cnt / WIDTH;
    re = block_cnt % WIDTH;
    for(i = 0 ; i < div ; i++){
        for(j = 0 ; j < WIDTH ; j++)
            field[i][j] = XBLOCK;
    }
    for( j = 0; j < re ; j++){
        int x = rand() % WIDTH;
        if(field[i][x] == 0){
            field[i][x] = XBLOCK;
        }
        else
            --j;
        
    }
    attack_flag = 0;
    return;
    
}
int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){

	int i,j;
	for(i=0; i < BLOCK_HEIGHT ; i++){
		for(j=0 ; j < BLOCK_WIDTH ; j++){
			if(block[currentBlock][blockRotate][i][j]){
				f[i+blockY][j+blockX] = block[currentBlock][blockRotate][i][j];
			}
		}
	}
	return 0;
	//Block이 추가된 영역의 필드값을 바꾼다.

}

///////////////////////////////////////////////////////////////////////////


void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){

	DrawBlock(y,x,blockID,blockRotate,' ');

}
int CalScore(int num_of_puyo, int puyo[], int num_of_color){
	int chain_score;
	int connect_score =0;
	int color_score;
    

	switch(num_of_chains){
		case  1:
			chain_score = 0;
			break;
		case 2:
			chain_score = 8;
			break;
		case 3:
			chain_score = 16;
			break;
		case  4:
			chain_score = 32;
			break;
		case 5:
			chain_score = 64;
			break;
		case 6:
			chain_score = 96;
			break;
		case  7:
			chain_score = 128;
			break;
		case 8:
			chain_score = 160;
			break;
		case 9:
			chain_score = 192;
			break;
	}
	switch(num_of_color){
		case 1:
			color_score =0;
			break;
		case 2:
			color_score = 3;
			break;
		case 3:
			color_score = 6;
			break;
		case 4:
			color_score = 12;
			break;
	}
	for(int i = 0; i < 4 ; i++){
		switch(puyo[i]){
			case 0:
				connect_score += 0;
				break;
			case 4:
				connect_score += 0;
				break;
			case 5:
				connect_score += 2;
				break;
			case 6:
				connect_score += 3;
				break;
			case 7:
				connect_score += 4;
				break;
			case 8:
				connect_score += 5;
				break;
			case 9:
				connect_score += 6;
				break;
			case 10:
				connect_score += 7;
			default:
				connect_score += 10;
		}
	}
	return num_of_puyo *( chain_score + connect_score + color_score) *10;
}

void connect_server(){
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void * thread_return;
    sock = socket(PF_INET,SOCK_STREAM, 0);
    
    memset(&serv_addr, 0 , sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(7001);
    
    if(connect(sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        error_handling("connect error");
    
    me.online = 1;
    pthread_create(&snd_thread,NULL,send_data, (void*)&sock);
    pthread_create(&rcv_thread,NULL,recv_data, (void*)&sock);
    pthread_detach(snd_thread);
    pthread_detach(rcv_thread);
    return;
}

void * send_data(void * arg){
    int socket = *((int*)arg);
    
    while(1){
        memcpy(me.field,field,sizeof(field));
        me.score = score;
        write(socket,(player *)&me,sizeof(me));
        usleep(100000);
    }
    return NULL;
}
void * recv_data(void * arg){
    int socket = *((int*)arg);
    while(1){
        read(socket,(player*)&opPlayer,sizeof(opPlayer));
        DrawOpField();
        printOpScore();
        if(opPlayer.score != op_score && !attack_flag){
            attack_flag = 1;
            attack_score = opPlayer.score  - op_score;
            op_score = opPlayer.score;
        }
        
    }
}

void error_handling(char * message){
    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
    
}

void DrawOpField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1+WIDTH + 17);
		for(i=0;i<WIDTH;i++){
			if(opPlayer.field[j][i]){
				attron(COLOR_PAIR(opPlayer.field[j][i]));
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
				attron(COLOR_PAIR(5));
			}
			else printw(" ");
		}
	}
}
