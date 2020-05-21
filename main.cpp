#include <iostream>
#include <ctime>
#include <cstring>
#include <cmath>
#include <stdio.h>
//Using SDL     引入SDL
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#define SDL_main main   //這一行(或#undef main)必須加！!否則跑不動~~唉嘿!
using namespace std;

/*global variance*//*啦啦啦~~*/
//about animate
const int FPS = 90;
const int FLIP_COUNT = CLOCKS_PER_SEC/FPS;
//about window
const int WINDOW_W = 500;    const int WINDOW_H = 666;                  //視窗的長寬
const char WINDOW_Title[] = "Planes War (New Year ver.) --by Black Rabbit Effect";               //視窗的標題
const int BossTime = 45;                //sec, how long boss will appear

/*include our own .h file*/
#include "bullet.h"
#include "enemy.h"
#include "player.h"
#include "screen.h"
#include "main.h"

/*---------------------------------------------- main ----------------------------------------*/
int main(){
    /*--------------------------- 資料載入&建立環境 ---------------------------*/
    cout<<clock()<<"ms | Finish loading file & image."<<endl;
    //preload data
    Player me(WINDOW_W/2-50, WINDOW_H*3/4, 5, "my plane", "img/myPlane.png", B_mine);
    Enemy* enemies[15] = {nullptr};
    Bullet* bullets[250] = {nullptr};
    Enemy::number = 0;
    Bullet::number = 0;

    //create window & screen
    Screen screen(WINDOW_W, WINDOW_H, WINDOW_Title, cover, engin, cloud);
    //set music
    if(!loadMedia()){
        cout<<"fail to load media! "<<Mix_GetError()<<endl;
    }
    Mix_VolumeMusic(20);
    Mix_PlayMusic(chooseStageMusic, -1);
    cout<<clock()<<"ms | Finish game setup."<<endl;
    //build event listener
    SDL_Event event;            //Event handler      事件監聽
    while( !(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE)){  //space key up -> inter the game
        if(event.type == SDL_QUIT){
            screen.close();
            return true;
        }
        SDL_PollEvent(&event);
    }

    /*----------------------------- a stage 關卡循環 -------------------------------*/
    while(true){
        if( beforeStage(screen, chooseStageBg, me, enemies, bullets)==-1 ){     // -1: user exit
            screen.close();
            return true;
        }
        clock_t lastFlip = clock();          //for flips handling
        clock_t stageStart = clock();
        bool keyStatus[5] = {0};        //for event handling
        Boss boss(* AllStages[Stage::choose]->getBoss());
        bool bossAp = false;

        /*-------------------------- game loop 操作循環 -------------------------------*/
        //game end: me.life<=0 || boss appear and dead
        while(me.life()>0 && !boss.dead()){        //a flip of action
            //flips control
            if(clock() < lastFlip+FLIP_COUNT){ continue; }        //for flip-per-second controlling: wait until time
            if(1000/(clock() - lastFlip) < FPS){
               cout<<"FPS out of range: "<<1000.00 / (double)(clock() - lastFlip)<<endl;
            }

            //boss
            if(clock() - stageStart > 1000*BossTime){       //time to set boss
                if(!bossAp){
                    cout<<"Boss appear!"<<endl;
                    screen.setWind(true);
                }
                bossAp = true;
            }

            //user controll
            if( !me.action(keyStatus)){                    //if user exit
                screen.close();
                return true;
            }
            if(me.canShoot()){       //shoot
                bullets[Bullet::number] = me.shooting();
            }

            //move & draw all enemies & bullets
            aFlip(screen, me, enemies, boss, bullets, bossAp);
            lastFlip = clock();
        } //end while(true) inside
        /*------------------------ end game loop -----------------------*/
        lastFlip = clock();
        time_t pause = clock();
        bool win = (me.addLife(0)>0);
        //pause music
        Mix_HaltMusic();
        Mix_PlayMusic(paperPlaneMusic,-1);

        //pause screen
        while(clock() - pause < 4000){
            if(clock() < lastFlip+FLIP_COUNT){ continue; }        //for flip-per-second controlling: wait until time
            //user controll
            if( !me.action(keyStatus)){
                screen.close();
                return true;
            }

            //draw
            screen.scroll(false);
            screen.clean();
            if(bossAp){
                boss.moving_();
                if(boss.canShoot() && !boss.dead()){               //time to shoot!
                    int n = Bullet::number;
                    bullets[n] = boss.shooting();
                }
                screen.draw(&boss);
            }else{
                for(int i=0; i<Enemy::number; i++){
                    if(enemies[i]!=nullptr){
                        enemies[i]->moving();
                        if(enemies[i]->canShoot()){               //time to shoot!
                            bullets[Bullet::number] = enemies[i]->shooting();
                        }
                        screen.draw(enemies[i]);       //draw
                    }
                }
                for(int i=0; i<Bullet::number; i++){
                    if(bullets[i]==nullptr){ continue; }
                    bullets[i]->move2();
                    screen.draw(bullets[i]);       //draw
                } //end for(all bullet)
            }
            if(me.life()>0){
                screen.draw(&me);
            }
            screen.paintNew();
            lastFlip = clock();
        }

        //過關，結算分數
        if( !afterStage(screen, me, winBg, loseBg, win) ){        //false == user exit
            screen.close();
            return true;
        }

    }   //end while(true) outside
    /*-------------------------- end stage loop ---------------------*/

    system("pause");
    screen.close();
    return 0;
}


//全部結束了！！恭喜恭喜～
