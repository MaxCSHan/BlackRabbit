
/*------------------------- input handle ---------------------------@*/
bool inputHandler(SDL_Event event, bool keyStatus[]){  //set 1 event keyStatus: return false if user quit the game
    if(event.type == SDL_QUIT){       //user quit the game: return false
        return false;
    }else if(event.type == SDL_KEYDOWN){        //User presses a key
        switch( event.key.keysym.sym ){        //which key is press?
            case SDLK_UP:
                keyStatus[0] = true;    //true means "press"
                break;
            case SDLK_DOWN:
                keyStatus[1] = true;
                break;
            case SDLK_LEFT:
                keyStatus[2] = true;
                break;
            case SDLK_RIGHT:
                keyStatus[3] = true;
                break;
            case SDLK_SPACE:
                keyStatus[4] = true;
            default:
                break;
        } //end switch
    }else if(event.type==SDL_KEYUP){
        switch( event.key.keysym.sym ){        //which key is release?
            case SDLK_UP:
                keyStatus[0] = false;           //false means "not press"
                break;
            case SDLK_DOWN:
                keyStatus[1] = false;
                break;
            case SDLK_LEFT:
                keyStatus[2] = false;
                break;
            case SDLK_RIGHT:
                keyStatus[3] = false;
                break;
            case SDLK_SPACE:
                keyStatus[4] = false;
                break;
            default:
                break;
        } //end switch
    } //end if else
    return true;
}


class Player{
private:
    int x, y, w, h, v, score=0, lives=0;
    SDL_Surface* source;
    Bullet* bullet;
    time_t lastShoot = 0;
    int shootV=350;      //how many ms can shoot 1 bullet
public:
    friend Screen;
    friend Bullet;
    friend Boss;
    char name[100];     //玩家的名字
    Player(){};
    Player(int x_, int y_, int v_, char name_[], char img[], Bullet* b);
    bool action(bool keyStatus[]);   //return false = user quit or soPlayerthing wrong
    void moving(bool direction[]);
    int setScore(int add);
    int life();
    int addLife(int add);
    bool canShoot();
    Bullet* shooting();
    bool collisionPlayer(Enemy& enemy);
    void setPosition(int x_, int y_);
};
Player::Player(int x_, int y_, int v_, char name_[], char img[], Bullet* b){
    x = x_;
    y = y_;
    v = v_;
    strcpy(name, name_);
    source = IMG_Load(img);
    w = source->w;
    h = source->h;
    bullet = b;
}
void Player::moving(bool keyStatus[]){     //move the plane
    if(keyStatus[0] && !keyStatus[1]){  //up
        y -= v;
    }
    if(!keyStatus[0] && keyStatus[1]){  //down
        y += v;
    }
    if(keyStatus[2] && !keyStatus[3]){  //left
        x -= v;
    }
    if(!keyStatus[2] && keyStatus[3]){  //right
        x += v;
    }

    //if out of screen, poll it back
    if(x < 0){  // <->
        x = 0;
    }else if(x > WINDOW_W - w){
        x = WINDOW_W - w;
    }
    if(y < 0){  //up & down
        y = 0;
    }else if(y > WINDOW_H - h){
        y = WINDOW_H - h;
    }
}
bool Player::action(bool keyStatus[]){       //get event & move, return false if user quit
    SDL_Event event;
    while(SDL_PollEvent(&event)!=0){        //if event queue is not empty
       if( !inputHandler(event, keyStatus) ){         //User control: false == userrequests quit
            return false;
        }
    } //end if(event)
    moving(keyStatus);
    return true;
}
int Player::setScore(int add){
    score += add;
    if(score>99999999){
        score = 99999999;
    }else if(score<0){
        score = 0;
    }
    return score;
}
int Player::life(){
    return lives;
}
int Player::addLife(int add){
    lives += add;
    return lives;
}
bool Player::canShoot(){
    if(clock() - lastShoot < shootV){
        return false;
    }else{
        return true;
    }
}
Bullet* Player::shooting()       //return Bullet*
{
    Bullet* b = new Bullet(bullet, x+w/2, y -bullet->h, 0, -v-1);     //bullet at plane's top, &bullet is faster than the plane
    lastShoot = clock();
	return b;       //return enemy_bullet;
}
bool Player::collisionPlayer(Enemy& enemy)
{
    if( sqrt( pow( (float)(x - enemy.x) + (float)(w - enemy.w)/2, 2)
                + pow((float)(y - enemy.y) + (float)(h - enemy.h)/2, 2) )
            <= (h + enemy.h)/2 -8){   //!!! no '^' in C++
        return true;
    }else{
        return false;
    }
}
void Player::setPosition(int x_, int y_){       //x_ y_: center point position
    x = x_ - w/2;
    y = y_ - h/2;
}


