/*Goal: simplifies main + control stages*/
//Bullet kinds
SDL_Surface* myBullet = IMG_Load("img/bullet4.png");
SDL_Surface* enemyBullet1 = IMG_Load("img/bullet3.png");
SDL_Surface* enemyBullet2 = IMG_Load("img/bullet1.png");
SDL_Surface* enemyBullet3 = IMG_Load("img/bullet2.png");
Bullet* const B_mine = new Bullet(0, 0, 1, 1, myBullet);
Bullet* const B_enemy1 = new Bullet(0, 0, 1, 1, enemyBullet1);
Bullet* const B_enemy2 = new Bullet(0, 0, 1, 1, enemyBullet2);
Bullet* const B_enemy3 = new Bullet(0, 0, 1, 1, enemyBullet3);

/*-------------------------- class Boss ----------------------------------*/
// it will move <-> for endless times (use body's x to change direction(<- or ->))
//if Enemy::number==1: only boss body left
class Boss: public Enemy{
private:
    int lives = 3;
    bool toLeft = false;
    int circle = 0;
    int step = 0;
    Bullet* bs[3] = {B_enemy1, B_enemy2, B_enemy3};

public:
    friend Screen;
    friend Bullet;
    static SDL_Surface* deadImg;
    Boss(){};
    Boss(int life, int shootV_, char img[]);
    Boss(const Boss &b);
    bool dead();
    void beHit();
    void moving_();
    Bullet* shooting();
    bool crash(Player& player);
};
Boss::Boss(int life, int shootV_, char img[]){
    shootV = shootV_;
    source = IMG_Load(img);
    w = source->w;
    h = source->h;
    x = (WINDOW_W - w)/2;
    y = - h - 5;
    v = 1;
    lives = life;
}
Boss::Boss(const Boss &b){
    w = b.w;
    h = b.h;
    x = b.x;
    y = b.y;
    v = b.v;
    source = b.source;
    shootV = b.shootV;
    lives = b.lives;
}
bool Boss::dead(){
    if(lives<=0){
        source = Boss::deadImg;
    }
    return (lives<=0);
}
void Boss::moving_(){
    if(y<50){
        y += v;
    }else{
        if(toLeft){
            x -= v;
            if(x<0){
                x = 0;
                toLeft = false;
            }
        }else{
            x += v;
            if(x > WINDOW_W - w){
                x = WINDOW_W - w;
                toLeft = true;
            }
        }
    }
}
void Boss::beHit(){
    if(y>=45){
        lives -= 1;
    }
}
Bullet* Boss::shooting(){
    Bullet* b = new Bullet(bs[circle%3], x+w/2, y+h,
                           3.5*sin(circle), abs(3.5*cos(circle)) );      //set v with circle&step
    circle += 1;
    lastShoot = clock();
	return b;       //return enemy_bullet;
}
bool Boss::crash(Player& player){
    if( sqrt( pow( (float)(x - player.x) + (float)(w - player.w)/2, 2)
                + pow( (float)(y - player.y) + (float)(h - player.h)/2, 2) )
            <= (player.h + h)/2){   //!!! no '^' in C++
        return true;
    }else{
        return false;
    }
}
SDL_Surface* Boss::deadImg = nullptr;

/*---------------------- Bullet -----------------------*/
bool Bullet::shootAtPlayer(Player& player)
{
    if( sqrt( pow(x -player.x -player.w/2, 2) + pow(y -player.y -player.h/2, 2) ) <= player.h/2){   //!!! no '^' in C++
        return true;
    }else{
        return false;
    }
}
bool Bullet::shootAtEnemy(Enemy& enemy)
{
    if(sqrt(pow(x -enemy.x -enemy.w/2, 2) + pow(y -enemy.y -enemy.h/2, 2)) <= enemy.h/2)
        return true;
    else
        return false;
}
bool Bullet::shootAtBoss(Boss& boss){
    if(sqrt( pow(x-boss.x - boss.w/2, 2) + pow(y-boss.y-boss.h/2, 2)) <= boss.h/2)
        return true;
    else
        return false;
}

/*------------------------ class Stage：關卡 ----------------------*/
class Stage{
private:
    const Boss* boss;
    char* name;                    //stage name
    char* introduction;          //introduction of the stage
    SDL_Surface* bg;            //background
    Mix_Music* bgMusic;      //background music
    int enemyKinds;              //how many enemy will be randomly choose
    const Enemy** enemies;             //possible enemies
    time_t lastWave =0;
    int waveSpeed = 2100;
    int waveMax = 4;            //max number of a wave of enemies
    void setting(const Boss* b, SDL_Surface* bg_,
                 char* name_, char* introduction_, int enemyKinds_);  //constructer
public:
    static int number;
    static int choose;
    Stage();
    //at last 2, up to 5, for all possible number of enemy kinds
    Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[],
          const Enemy* enemy1, const Enemy* enemy2);
    Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[],
          const Enemy* enemy1, const Enemy* enemy2, const Enemy* enemy3);
    Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[],
          const Enemy* enemy1, const Enemy* enemy2, const Enemy* enemy3, const Enemy* enemy4);
    Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[], const Enemy* enemy1,
          const Enemy* enemy2, const Enemy* enemy3, const Enemy* enemy4, const Enemy* enemy5);
    ~Stage();

    const Enemy* getEnemy();
    int enemyShouldAdd();
    void setWave(Enemy* enemies[]);
    const char* getName();
    const char* getInfo();
    SDL_Surface* getBg();
    Mix_Music* getMusic();
    const Boss* getBoss();
    void setMusic(Mix_Music*);
};
void Stage::setting(const Boss* b, SDL_Surface* bg_,
                    char name_[], char introduction_[], int enemyKinds_){
    boss = b;
    bg = bg_;
    //string
    name = new char[strlen(name_)+1];
    strcpy(name, name_);
    introduction = new char[strlen(introduction_)+1];
    strcpy(introduction, introduction_);
    //random enemy
    enemyKinds = enemyKinds_;
    enemies = new const Enemy*[enemyKinds];       //只存指標，所有敵機型號都存在別的地方
    //count++
    Stage::number += 1;
}
Stage::Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[],
             const Enemy* enemy1, const Enemy* enemy2){
    setting(b, bg_, name_, introduction_, 2);
    //random enemy
    enemies[0] = enemy1;
    enemies[1] = enemy2;
}
Stage::Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[],
             const Enemy* enemy1, const Enemy* enemy2, const Enemy* enemy3){
    setting(b, bg_, name_, introduction_, 3);
    //random enemy
    enemies[0] = enemy1;
    enemies[1] = enemy2;
    enemies[2] = enemy3;
}
Stage::Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[],
             const Enemy* enemy1, const Enemy* enemy2, const Enemy* enemy3, const Enemy* enemy4){
    setting(b, bg_, name_, introduction_, 4);
    //random enemy
    enemies[0] = enemy1;
    enemies[1] = enemy2;
    enemies[2] = enemy3;
    enemies[3] = enemy4;
}
Stage::Stage(const Boss* b, SDL_Surface* bg_, char name_[], char introduction_[], const Enemy* enemy1,
             const Enemy* enemy2, const Enemy* enemy3, const Enemy* enemy4, const Enemy* enemy5){
    setting(b, bg_, name_, introduction_, 5);
    //random enemy
    enemies[0] = enemy1;
    enemies[1] = enemy2;
    enemies[2] = enemy3;
    enemies[3] = enemy4;
    enemies[4] = enemy5;
}
Stage::~Stage(){
    delete[] name;
    delete[] introduction;
    delete[] enemies;
    Stage::number -= 1;
}
const Enemy* Stage::getEnemy(){     //return an enemy
    return enemies[ rand()%enemyKinds ];
}
const char* Stage::getName(){
    return name;
}
const char* Stage::getInfo(){
    return introduction;
}
SDL_Surface* Stage::getBg(){
    return bg;
}
Mix_Music* Stage::getMusic(){
    return bgMusic;
}
void Stage::setWave(Enemy* enemies[]){
    if(clock() - lastWave > waveSpeed){     //time to let a new wave in
        int r = rand()%waveMax +1;      //number of enemies this wave
        const Enemy* e = getEnemy();              //enemy kind this wave
        int rh = rand()%(int)(WINDOW_W*0.4) + WINDOW_W*0.12;   //head enemy's x position;
        int disx = rand()%60 + 55;   //enemies' x distance;
        int disy = rand()%60 + 55;   //enemies' y distance;
        bool isL = rand()%2;            //enemies move starting direction
        for(int i=0; i<r; i++){
            int en = Enemy::number;
            enemies[en] = new Enemy(*e, (rh+disx*i)%(int)(WINDOW_W - 300)-50, -100-disy*i);
            enemies[en]->left = isL;
        }

        lastWave = clock();
    }
}
const Boss* Stage::getBoss(){
    return boss;
}
void Stage::setMusic(Mix_Music* mu){
    bgMusic = mu;
}

int Stage::number = 0;
int Stage::choose = 0;

const double Pi = atan(1)*4.0;      //pi
/*---------------------------- load enemies, bullets, stages --------------------------------*/
SDL_Surface* Heart = IMG_Load("img/heart.png");;
//use those as paramiter of copy constructer (可用名字調用copy constructer)

//Enemy kinds
Enemy* const P_Lu = new Enemy(0,0, 1, 900, "img/lu.png", B_enemy2, 1);
Enemy* const P_Rich = new Enemy(0,0, 1, 900, "img/rich.png", B_enemy3, 0);
Enemy* const P_DOGS = new Enemy(0,0, 1, 1200, "img/PDOGS.png", B_enemy1, 1);
Enemy* const P_Sun = new Enemy(0,0, 1, 1500, "img/plane1.png", B_enemy1, 0);
Enemy* const P_Green = new Enemy(0, 0, 2, 1000, "img/green_plane.png", B_enemy1, 0);
Enemy* const P_Circle = new Enemy(0, 0, 2, 700, "img/circle_plane.png", B_enemy1, 1);
Enemy* const P_Helicopter = new Enemy(0, 0, 1, 1300, "img/helicopter.png", B_enemy1, 0);

//boss
Boss* const  Boss_wall = new Boss(50, 120, "img/boss_wall.png");
Boss* const  Boss_island = new Boss(30, 70, "img/boss_italy.png");
Boss* const  Boss_paris = new Boss(15, 40, "img/boss_paris.png");
Boss* const  Boss_forest = new Boss(15, 40, "img/boss_rainforest.png");
Boss* const  Boss_class = new Boss(30, 70, "img/boss_classppt.png");
Boss* const  Boss_code = new Boss(50, 120, "img/boss_code.png");

//stages
SDL_Surface* iceland = IMG_Load("img/iceland.jpg");
SDL_Surface* rainForest = IMG_Load("img/rain_forest.jpg");
SDL_Surface* greatWall = IMG_Load("img/greatWall.jpg");
SDL_Surface* city = IMG_Load("img/city.jpg");
SDL_Surface* code = IMG_Load("img/code.jpg");
SDL_Surface* classes = IMG_Load("img/class.jpg");

SDL_Surface* cover = IMG_Load("img/cover.png");
SDL_Surface* cloud = IMG_Load("img/cloud1.png");
SDL_Surface* engin = IMG_Load("img/engin.png");
SDL_Surface* chooseStageBg = IMG_Load("img/Map1.jpg");
SDL_Surface* winBg = IMG_Load("img/win.png");
SDL_Surface* loseBg = IMG_Load("img/lose.jpg");

Mix_Music* chooseStageMusic = NULL;
Mix_Music* paperPlaneMusic = NULL;
Mix_Music* pirateMusic = NULL;
Mix_Music* battleFieldMusic = NULL;
Mix_Music* rangingWavesMusic = NULL;
Stage* const AllStages[6] = {
    new Stage(Boss_wall, greatWall, "The great wall",
              "the long, long wall", P_Green, P_Sun, P_Helicopter, P_Lu, P_Rich),
    new Stage(Boss_class, classes, "In class",
              "programing design", P_Green, P_Green, P_DOGS, P_Circle, P_Rich),
    new Stage(Boss_island, iceland, "Ice Island",
              "Ice & Sea", P_Sun, P_Helicopter, P_Circle, P_Circle, P_Lu),
    new Stage(Boss_paris, city, "In the city",
              "Do you know where is it?", P_Green, P_Helicopter, P_Circle, P_Lu),
    new Stage(Boss_code, code, "Codes",
              "Compile Error / Accept!", P_DOGS, P_DOGS,  P_Lu, P_Rich),
    new Stage(Boss_forest, rainForest, "Rain Forest",
              "The Amazon forest", P_Circle,  P_Helicopter, P_Helicopter, P_Green, P_Rich)
};

bool loadMedia(){
    chooseStageMusic = Mix_LoadMUS("music/bg.wav");
    paperPlaneMusic = Mix_LoadMUS("music/Deemo_Paper_Planes_Adventure.wav");
    pirateMusic = Mix_LoadMUS("music/He_is_a_Pirate.wav");
    battleFieldMusic = Mix_LoadMUS("music/The_Battlefield.wav");
    rangingWavesMusic = Mix_LoadMUS("music/The_End_of_Raging_Waves.wav");
    if(chooseStageMusic==NULL || paperPlaneMusic==NULL || pirateMusic==NULL ||
       battleFieldMusic==NULL || rangingWavesMusic==NULL){
        return false;
    }
    AllStages[0]->setMusic(rangingWavesMusic);
    AllStages[1]->setMusic(rangingWavesMusic);
    AllStages[2]->setMusic(pirateMusic);
    AllStages[3]->setMusic(chooseStageMusic);
    AllStages[4]->setMusic(pirateMusic);
    AllStages[5]->setMusic(battleFieldMusic);
    Boss::deadImg = IMG_Load("img/dead.png");
    return true;
}

void printScore(Screen& screen, Player& player, SDL_Color color){        //print player score
    //show score
    int pt = player.setScore(0);
    char c[2] = {'\0','\0'};
    for(int i=1; i<6; i++){         //show 5 number: ex. 0000123
        if(pt<=0){
            c[0] = '0';
        }else{
            c[0] =pt%10 + 48;
            pt = pt/10;
        }
        screen.print(c, color, WINDOW_W - i*20 - 10, 7, FONTbig);
    }
    screen.paintNew();
}

/*------------------------------------ paint a flip --------------------------------------*/
bool aFlip(Screen& screen, Player& me, Enemy* enemies[], Boss& boss, Bullet* bullets[], bool bossAppear){
    //background
    screen.clean();
    screen.scroll(false);
    //Enemy
    for(int i=0; i<Enemy::number; i++){
        if( !(enemies[i]->moving()) ){        //move & detective out of screen
            delete enemies[i];
            enemies[i] = enemies[Enemy::number];        //put the last enemy into the empty space
            enemies[Enemy::number] = nullptr;
        }else if(enemies[i]->canShoot()){               //time to shoot!
            bullets[Bullet::number] = enemies[i]->shooting();
        }
        if(enemies[i]!=nullptr){
            screen.draw(enemies[i]);       //draw
        }
    }
    //random new enemy
    if(!bossAppear){            //if boss appear, stop random new enemy
        AllStages[Stage::choose] -> setWave(enemies);
    }else if(Enemy::number==0){                          //boss appear, let the boss move
        boss.moving_();
        if(boss.canShoot() && Bullet::number<200){               //time to shoot!
            int n = Bullet::number;
            bullets[n] = boss.shooting();
        }

        if(boss.crash(me)){         //crash between me and boss: me.life--, boss do nothing
            me.addLife(-1);
        }
        screen.draw(&boss);
    }

    //Bullet
    for(int i=0; i<Bullet::number; i++){
        if(bullets[i]!=nullptr){
            screen.draw(bullets[i]);       //draw
        }

        if( !(bullets[i]->move2()) ){        //move, and than detective it is out of screen or not
            delete bullets[i];
            bullets[i] = bullets[Bullet::number];       //put the last bullet into the empty space
            bullets[Bullet::number] = nullptr;
        }else if(bullets[i]->shootAtPlayer(me)){            //bullet touch player plane!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //delete bullet
            delete bullets[i];
            bullets[i] = bullets[Bullet::number];       //put the last bullet into the empty space
            bullets[Bullet::number] = nullptr;
            //player's life --
            cout<<"lives now: "<<me.addLife(-1)<<endl;
            screen.setHeader(me.life(), Heart);
        }else{                                                              //bullet touch enemy!!!!!!!!!!!!!!!!!!!!!!!!
            for(int j=0; j<Enemy::number; j++){
                if(bullets[i]->shootAtEnemy(*enemies[j]) && bullets[i]->isMine()){        //hit & bullet is mine
                    //delete bullet
                    delete bullets[i];
                    bullets[i] = bullets[Bullet::number];       //put the last bullet into the empty space
                    bullets[Bullet::number] = nullptr;
                    //delete enemy
                    delete enemies[j];
                    enemies[j] = enemies[Enemy::number];       //put the last enemy into the empty space
                    enemies[Enemy::number] = nullptr;
                    me.setScore(10);        //my score += 10
                    break;
                }
            }
            if(bossAppear && bullets[i]->shootAtBoss(boss) && bullets[i]->isMine()){   //hit boss  & bullet is mine
                //delete bullet
                delete bullets[i];
                bullets[i] = bullets[Bullet::number];       //put the last bullet into the empty space
                bullets[Bullet::number] = nullptr;
                //boss life --
                boss.beHit();
                me.setScore(20);
            }
        }
    } //end for(all bullet)

    // are there planes crash?
    for(int j=0; j<Enemy::number; j++){
        if(me.collisionPlayer(*enemies[j])){
            me.addLife(-1);
            //delete enemy
            delete enemies[j];
            enemies[j] = enemies[Enemy::number];       //put the last enemy into the empty space
            enemies[Enemy::number] = nullptr;
            break;
        }
    }

    //Player
    if(me.addLife(0)>0){
        screen.draw(&me);
    }

    //text
    screen.addHeader();
    printScore(screen, me, Black);

    //new the screen
    screen.paintNew();
    return true;
}

/*---------------------------- beforeStage() 進關卡前置作業 -------------------------------*/
//return stage index
int beforeStage(Screen& screen, SDL_Surface* bg, Player& player, Enemy** enemies, Bullet** bullets){
    //initalize
    if(Mix_PlayingMusic()==0){
        Mix_PlayMusic(chooseStageMusic, -1);
    }

    player.addLife(5 - player.addLife(0));     //add lives to 5
    screen.setHeader(player.addLife(0), Heart);
    player.setScore(-99999999);         //set score to 0
    for(int i=Enemy::number; i>=0; i--){     //delete all enemies
        delete enemies[i];
        enemies[i] = nullptr;
    }
    for(int i=Bullet::number; i>=0; i--){     //delete all bullets
        delete bullets[i];
        bullets[i] = nullptr;
    }

    cout<<"start choose stage.Please press \"space\" to start."<<endl;
    screen.changeBg(bg);
    //player.setPosition(WINDOW_W/2, WINDOW_H*7/11);      //center of the 6
    player.setPosition(WINDOW_W/2 + WINDOW_H*2/11*sin(Stage::choose),
                       WINDOW_H*7/11 - WINDOW_H*2/11*cos(Stage::choose));

    //claim variables
    SDL_Event event;
    bool keyStatus[5] = {false,false,false,false,false};     //check if key is press

    while(true){       //haven't choosen (click key space) yet
        bool ori_keys[5] = {keyStatus[0], keyStatus[1], keyStatus[2], keyStatus[3], keyStatus[4]};     //origin key press status
        screen.clean();
        screen.scroll(true);
        screen.print("press left/right to change stage", Black, WINDOW_W/2, 615, FONTsmall);
        screen.print("press space to start", Black, WINDOW_W/2, 635, FONTsmall);
        SDL_PollEvent(&event);
        if( !inputHandler(event, keyStatus) ){     //user quit or not
            return -1;
        }

        //user not quit: check if click once (press and release)
        if(!keyStatus[3] && ori_keys[3]){      //key 'right' is clicked
            if(Stage::choose==Stage::number-1){       //at the last stage
                Stage::choose = 0;  //set to first stage
            }else{
                Stage::choose += 1;
            }
        }
        if(!keyStatus[2] && ori_keys[2]){      //key 'left' is clicked
            if(Stage::choose==0){       //at the first stage
                Stage::choose = Stage::number-1;  //set to last stage
            }else{
                Stage::choose -= 1;
            }
        }
        if(!keyStatus[4] && ori_keys[4]){                                               //key 'space' is clicked
            break;
        }

        //draw
        screen.print(AllStages[Stage::choose]->getName(), White, WINDOW_W/2, 150, FONTbig);
        screen.print(AllStages[Stage::choose]->getInfo(), White, WINDOW_W/2, 190, FONTsmall);
        player.setPosition(WINDOW_W/2 + WINDOW_H*2/11*sin((double)Stage::choose*Pi/3),
            WINDOW_H*7/11 - WINDOW_H*2/11*cos((double)Stage::choose*Pi/3) );
        screen.draw(&player);
        screen.paintNew();
    } //end while(have event & not choose)

    player.setPosition(WINDOW_W/2, WINDOW_H - 100);
    Mix_HaltMusic();
    Mix_PlayMusic(AllStages[Stage::choose]->getMusic(), -1);
    screen.changeBg(AllStages[Stage::choose]->getBg());    //re-paint
    cout<<"inter stage: "<<AllStages[Stage::choose]->getName()<<endl;
    return Stage::choose;
}

bool afterStage(Screen& screen, Player& player, SDL_Surface* winBg, SDL_Surface* loseBg, bool win){
    //claim variables
    SDL_Event event;

    if(win){                                                        //win
        screen.changeBg(winBg);
        cout<<"Congratulations! You win!"<<endl;
    }else{                                                                          //lose
        screen.changeBg(loseBg);
        cout<<"You are dead..."<<endl;
    }

    screen.paintNew();

    //out of this page
    bool spaceDown = false;
    bool lastSpace = true;
    while(true){       //haven't choosen (click key space) yet
        SDL_PollEvent(&event);
        if( event.type == SDL_QUIT ){     //user quit or not
            return false;
        }else if(event.type == SDL_KEYUP && event.key.keysym.sym == SDLK_SPACE){   //space up
            if(spaceDown){
                break;
            }
            lastSpace = false;
        }else if(event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_SPACE){  //space down
            if(!lastSpace){
                spaceDown = true;
            }
            lastSpace = true;
        }else{
            lastSpace = false;
        }

        screen.scroll(true);
        screen.clean();
        if(win){    //win
            //print score
            screen.print("score", Black, WINDOW_W*0.62, 5, FONTbig);
            printScore(screen, player, Black);
        }else{
            screen.print("You are dead . . . ", White, WINDOW_W/2, 280, FONTbig);
            screen.print("This is the reality.", White, WINDOW_W/2, 340, FONTsmall);
            //print score
            screen.print("score", White, WINDOW_W*0.62, 5, FONTbig);
            printScore(screen, player, White);
        }

        screen.paintNew();
    } //end while(have event & not choose)

    Mix_HaltMusic();
    return true;
}





