#ifndef _TETRIS_H_
#define _TETRIS_H_


// 색 종류 : 빨강, 파랑, 초록, 노랑
#include <utility>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <signal.h>
#include <string.h>
#include <unistd.h> 
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <netdb.h>
using namespace std;

#define RED 1
#define BLUE 2
#define GREEN 3
#define PURPLE 4
#define XBLOCK 'X'

#define WIDTH	6
#define HEIGHT	12
#define NOTHING	0
#define QUIT	'q'

#define NUM_OF_SHAPE 10
#define NUM_OF_ROTATE	4
#define BLOCK_HEIGHT	2
#define BLOCK_WIDTH	2
#define BLOCK_NUM	2

// menu number
#define MENU_PLAY '1'
#define MULTI_PLAY '2'
#define MENU_EXIT '4'

// 사용자 이름의 길이
#define NAMELEN 16

/**
 * @brief 플레이어 정보
 * 
 */
typedef struct player{
    int online;
    char field[HEIGHT][WIDTH];
    int score;
} player; 

player opPlayer, me;


/* [blockShapeID][# of rotate][][]*/
const char block[NUM_OF_SHAPE][NUM_OF_ROTATE][BLOCK_HEIGHT][BLOCK_WIDTH] ={
	{/*[0][][][]					R R */
		{/*[][0][][]*/
      {0, 1}, {0, 1}
		},
		{/*[][1][][]*/
      {1, 1}, {0, 0}
		},
		{/*[][2][][]*/
      {1, 0}, {1, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {1, 1}
		}
	},
	{/*[1][][][];					  B B */
		{/*[][0][][]				      */
		  {0, 2}, {0, 2}
		},
		{/*[][1][][]*/
      {2, 2}, {0, 0}
		},
		{/*[][2][][]*/
      {2, 0}, {2, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {2, 2}
		}
	},
	{/*[2][][][];					  G G*/
		{/*[][0][][]				  */
      {0, 3}, {0, 3}
		},
		{/*[][1][][]*/
      {3, 3}, {0, 0}
		},
		{/*[][2][][]*/
      {3, 0}, {3, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {3, 3}
		}
	},
	{/*[3][][][];					  P P*/
		{/*[][0][][]				    */
		  {0, 4}, {0, 4}
		},
		{/*[][1][][]*/
      {4, 4}, {0, 0}
		},
		{/*[][2][][]*/
      {4, 0}, {4, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {4, 4}
		}
	},
	{/*[4][][][];					  R B*/
		{/*[][0][][]				  */
			{0, 1}, {0, 2}
		},
		{/*[][1][][]*/
      {1, 2}, {0, 0}
		},
		{/*[][2][][]*/
      {2, 0}, {1, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {2, 1}
		}
	},
	{/*[5][][][];					  R G*/
		{/*[][0][][]				*/
			{0, 1}, {0, 3}
		},
		{/*[][1][][]*/
      {1, 3}, {0, 0}
		},
		{/*[][2][][]*/
      {3, 0}, {1, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {3, 1}
		}
	},
	{/*[6][][][];					R P*/
		{/*[][0][][]				   */
			{0, 1}, {0, 4}
		},
		{/*[][1][][]*/
      {1, 4}, {0, 0}
		},
		{/*[][2][][]*/
      {4, 0}, {1, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {4, 1}
		}
	},
  {/*[7][][][];					B G*/
		{/*[][0][][]				   */
			{0, 2}, {0, 3}
		},
		{/*[][1][][]*/
      {2, 3}, {0, 0}
		},
		{/*[][2][][]*/
      {3, 0}, {2, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {3, 2}
		}
	},
  {/*[8][][][];					B P*/
		{/*[][0][][]				   */
			{0, 2}, {0, 4}
		},
		{/*[][1][][]*/
      {2, 4}, {0, 0}
		},
		{/*[][2][][]*/
      {4, 0}, {2, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {4, 2}
		}
	},
  {/*[9][][][];					G P*/
		{/*[][0][][]				   */
			{0, 3}, {0, 4}
		},
		{/*[][1][][]*/
      {3, 4}, {0, 0}
		},
		{/*[][2][][]*/
      {4, 0}, {3, 0}
		},
		{/*[][3][][]*/
			{0, 0}, {4, 3}
		}
	}
};

const int dx[4] = { -1, 0, 1, 0};   /* 상하좌우 이동 좌표 */
const int dy[4] = { 0, -1, 0, 1};

int visited[HEIGHT][WIDTH];     /* 뿌요를 지울때 확인했는지 저장 */


vector<pair<int, int> > list;   /* 지워질 뿌요의 좌표를 저장 */


int multi = 0;                  /* 싱글, 멀티인지 확인하는 flag */
char field[HEIGHT][WIDTH];  	/* 테트리스의 메인 게임 화면 */
int nextBlock[BLOCK_NUM];	    /* 현재 블럭의 ID와 다음 블럭의 ID들을 저장; [0]: 현재 블럭; [1]: 다음 블럭 */
int blockRotate,blockY,blockX;	/* 현재 블럭의 회전, 블럭의 Y 좌표, 블럭의 X 좌표*/
int score;			            /* 점수가 저장*/
int gameOver=0;			        /* 게임이 종료되면 1로 setting된다.*/
int timed_out;                  /* 알람을 초기화하기 위해 확인하는 변수 */
int process_flag=0;             /* blockDown 함수가 실행중임을 확인하는 flag */
int num_of_chains;              /* 현재 연쇄 콤보를 저장 */
int sock;						/* 소켓 handler*/
int attack_flag;				/* 공격을 진행중인지 여부*/
int op_score;					/* 상대방의 점수*/
int attack_score;				/* 내 공격점수*/
pthread_mutex_t mutx;

/***********************************************************
 *	테트리스의 모든  global 변수를 초기화 해준다.
 *	input	: none
 *	return	: none
 ***********************************************************/
void InitTetris();

/***********************************************************
 *	테트리스의 모든  interface를 그려준다.
 *	input	: none
 *	return	: none
 ***********************************************************/
void DrawOutline();

/***********************************************************
 *	테트리스와 관련된 키입력을 받는다.
 *	input	: none
 *	return	: (int) 입력받은 command
 *		  KEY_DOWN : 방향키 아래
 *		  KEY_UP   : 방향키 위
 *		  KEY_RIGHT: 방향키 오른쪽
 *		  KEY_LEFT : 방향키 왼쪽
 *		  ' '	   : Space bar
 *		  'q'/'Q'  : quit
 ***********************************************************/
int GetCommand();

/***********************************************************
 *	GetCommand로 입력받은 command에 대한 동작을 수행한다.
 *	input	: (int) GetCommand로 받은 command
 *	return	: (int) quit에 대한 입력을 받을 경우 0,
 * 		  그외의 경우 1을 return한다.
 ***********************************************************/
int ProcessCommand(int command);

/***********************************************************
 *	블럭이 일정 시간(1초)마다 내려가도록 호출되는 함수
 *	더이상 내릴수 없을 경우,
 *		블럭을 field에 합친다.
 *		완전이 채워진 line을 지운다.
 *		next block을 current block으로 바꿔주고
 *		block의 좌표를 초기화 한다.
 *		다음 블럭을 화면에 그리고 갱신된 score를 
 *		화면에 display한다.
 *	input	: (int) sig
 *	return	: none
 ***********************************************************/
void BlockDown(int sig);

/***********************************************************
 *	입력된 움직임이 가능한지를 판단해주는 함수.
 *	input	: (char[][]) 블럭의 움직임을 확인할 필드
 *		  (int) 현재 블럭의 모양 ID
 *		  (int) 블럭의 회전 횟수
 *		  (int) 블럭의 Y좌표
 *		  (int) 블럭의 X좌표
 *	return	: (int) 입력에 대한 블럭 움직임이 가능하면 1
 *		  가능하지 않으면 0을 return 한다.
 ***********************************************************/
int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX);

/***********************************************************
 *	테트리스에서 command에 의해 바뀐 부분만 다시 그려준다.
 *	input	: (char[][]) command의해 바뀐 블럭을 확인할 필드
 *		  (int) 바뀌기 전 모양을 알기 위해 command를 입력으로 받는다.
 *		  (int) 현재 블럭의 모양 ID
 *		  (int) 블럭의 회전 횟수
 *		  (int) 블럭의 Y좌표
 *		  (int) 블럭의 X좌표
 *	return	: none
 ***********************************************************/
void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX);

/***********************************************************
 *	테트리스의 블럭이 쌓이는 field를 그려준다.
 *	input	: none
 *	return	: none
 ***********************************************************/
void DrawField(char f[HEIGHT][WIDTH]);

/***********************************************************
 *	떨어지는 블럭을 field에 더해준다.
 *	input	: (char[][]) 블럭을 쌓을 필드
 *		  (int) 현재 블럭의 모양 ID
 *		  (int) 블럭의 회전 횟수
 *		  (int) 블럭의 Y좌표
 *		  (int) 블럭의 X좌표
 *	return	: none
 ***********************************************************/
int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX);


/***********************************************************
 *	뿌요들을 밑으로 떨어트리고 여부를 반환한다
 *	input	: (char[][]) 뿌요의 정보를 포함한 필드
 *	return	: (int) 떨어지는지 여부
 ***********************************************************/

int CheckFall(char f[HEIGHT][WIDTH]);

/***********************************************************
 *	연쇄 가능한 상하좌우 뿌요를 탐색한다
 *  뿌요들의 좌표를 list에 저장한다
 *	input	: (char[][]) 뿌요의 정보를 포함한 필드, 뿌요의 종류와 좌표
 *	return	: void
 ***********************************************************/


void Chain(char f[HEIGHT][WIDTH], int y, int x, int puyo);


/***********************************************************
 *	뿌요가 지워질 때 점수를 계산하여 반환한다.
 *	input	: 지워진 뿌요의 수, 연쇄 콤보, 색의 종류
 *	return	: 획득한 점수
 ***********************************************************/
int CalScore(int num_of_puyo, int puyo[], int num_of_color);

/***********************************************************
 *	뿌요를 지우는 함수 더 이상 지워질 뿌요가 없을 때 까지 반복한다.
 *	input	: 현재 필드의 정보
 *	return	: void
 ***********************************************************/

void PuyoBomb(char f[HEIGHT][WIDTH]);


/***********************************************************
 *	커서의 위치를 입력된 x, y의 위치로 옮겨주는 역할을 한다.
 *	input	: (int) 커서의 이동할 y 좌표
 *		  (int) 커서의 이동할 x 좌표
 *	return	: none
 ***********************************************************/

void gotoyx(int y, int x);

/***********************************************************
 *	테트리스의 화면 오른쪽상단에 다음 나올 블럭을 그려준다..
 *	input	: (int*) 블럭의 모양에 대한 ID 배열
 *	return	: none
 ***********************************************************/
void DrawNextBlock(int *nextBlock);

/***********************************************************
 *	테트리스의 화면 오른쪽 하단에 Score를 출력한다.
 *	input	: (int) 출력할 점수
 *	return	: none
 ***********************************************************/
void PrintScore(int score);

/***********************************************************
 *	해당 좌표(y,x)에 원하는 크기(height,width)의 box를 그린다.
 *	input	: (int) 그리고자 하는 박스의 왼쪽 상단모서리의 y 좌표
 *		  (int) 왼쪽 상단 모서리의 x 좌표
 *		  (int) 박스의 높이
 *		  (int) 박스의 넓이
 *	return	: none
 ***********************************************************/
void DrawBox(int y,int x, int height, int width);

/***********************************************************
 *	해당 좌표(y,x)에 원하는 모양의 블록을 그린다.
 *	input	: (int) 그리고자 하는 박스의 왼쪽 상단모서리의 y 좌표
 *		  (int) 왼쪽 상단 모서리의 x 좌표
 *		  (int) 블록의 모양
 *		  (int) 블록의 회전 횟수
 *		  (char) 블록을 그릴 패턴 모양
 *	return	: none
 ***********************************************************/
void DrawBlock(int y, int x, int blockID,int blockRotate,char tile);

/***********************************************************
 *	블록이 떨어질 위치를 미리 보여준다.
 *	input	: (int) 그림자를 보여줄 블록의 왼쪽 상단모서리의 y 좌표
 *		  (int) 왼쪽 상단 모서리의 x 좌표
 *		  (int) 블록의 모양
 *		  (int) 블록의 회전 횟수
 *	return	: none
 ***********************************************************/
void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate);


/***********************************************************
 *	테트리스 게임을 시작한다.
 *	input	: none
 *	return	: none
 ***********************************************************/
void play();

/***********************************************************
 *	메뉴를 보여준다.
 *	input	: none
 *	return	: 사용자가 입력한 메뉴 번호
 ***********************************************************/
char menu();

/**
 * @brief 게임이 실행될 동안 상대의 데이터를 받아온다
 * 
 * @param arg none
 * @return void* 
 */
void * receiveData(void * arg);

/**
 * @brief 에러 발생시 stdout을 출력한다
 * 
 * @param message 
 */
void errorHandling(const string message);

/**
 * @brief 상대방의 field를 그린다.
 * 
 */
void drawOpField();

/**
 * @brief 서버와 socket 연결 및 receiveData 쓰레드를 생성한다.
 * 
 */
void connectServer();

/**
 * @brief 상대방의 점수를 그린다
 * 
 */
void printOpScore();

/**
 * @brief 해당 좌표의 방해 블럭을 지운다.
 * 
 * @param y 
 * @param x 
 */
void DeleteXBlock(int y, int x);

/**
 * @brief 상대방의 공격에 따라 내 필드에 방해블럭을 쌓는다
 * 
 * @param s 상대방의 공격점수
 */
void AddXblockToField(int s);

/**
 * @brief 서버에 내 필드정보, 공격정보를 보낸다
 * 
 */
void sendPlayerData();

/**
 * @brief hostname을 통하여 ip를 받아온다
 * 
 * @param hostname 
 * @param ip 
 * @return int 반환 성공여부
 */
int hostnameToIp(char * hostname , char* ip);

/**
 * @brief 게임 시작전 로딩 그래픽 출력
 * 
 */
void DrawLoadingGraphic();
#endif
