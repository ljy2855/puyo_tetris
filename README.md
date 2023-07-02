# PuyoPuyo Tetris
puyo puyo tetris based terminal CLI


release : https://puyo.cspc.me

## 게임 방법 
* 테트리스와 동일하게 블럭 쌓기

* 쌓인 블럭은 위에서 아래로 떨어진다.

* 4개의 동일한 색상의 블럭이 붙어 있으면 지워진다.

* 블럭이 지워지고 빈칸으로 남은 블럭이 떨어진다. 
  이때 블럭이 떨어진 상태에서 또 블럭이 지워진다면 추가 점수를 얻는다.

* 1 vs 1 멀티, 블럭을 지워 얻은 점수로 상대에게 방해 블럭을 보낸다.
  
## 실행 방법
### Terminal
* 싱글 플레이   
  $make    
  $./app/puyo_client


* 멀티 플레이 
  $chmod +x deploy.sh  
  $./deploy.sh
  $./puyo_client    
  두명이 접속하게  되면 게임 시작

  game server host to http://puyo_server and client host to http://localhost:7000 
### Web
* https://puyo.cspc.me


## Stack
### release
* Docker
* ttyd
### game
* C


  
