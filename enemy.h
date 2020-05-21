class Enemy{
private:
    int x, y, w, h, v;
    int moveType = 0;
    Bullet* bullet;
    SDL_Surface* source = NULL;
    time_t lastShoot = 0;
    int shootV;      //how many ms can shoot 1 bullet
public:
    bool left = false;
    friend Screen;
    friend Bullet;
    friend Player;
    friend Boss;
    static int number;
    Enemy(){};
    Enemy(const Enemy& copy);
    Enemy(const Enemy& copy, int x_, int y_);
    Enemy(int x_, int y_, int v, int shootV_, char img[], Bullet* bullet_, int moveType_);
    ~Enemy();
	bool canShoot();
	Bullet* shooting();
    bool moving();
};
Enemy::Enemy(int x_, int y_, int v_, int shootV_, char img[], Bullet* bullet_, int moveType_){
    x = x_;
    y = y_;
    v = v_;
    shootV = shootV_;
    moveType = moveType_;
    source = IMG_Load(img);
    w = source->w;
    h = source->h;
    bullet = bullet_;
    Enemy::number += 1;
}
bool Enemy::moving( )		// pass true or false to main function to determine eliminate or not
{
    y += v;
    if(moveType==0){
        float a = log10(y*3)/2.7;
        if(x >= WINDOW_W - w/2){
            left = true;
        }else if(x <= -w/2){
            left = false;
        }
        if(left){
            x -= 2 + pow(2,y/250);
        }else{
            x += 2 + pow(2,y/250);
        }

    }else if(moveType==1){
        x += 3*round(cos(y/50));
        if(x > WINDOW_W - w/2){
            x = WINDOW_W - w/2;
        }else if(x < -w/2){
            x = -w/2;
        }
    }

	if(y > WINDOW_H)
		return false;
	else
        return true;
}

Enemy::Enemy(const Enemy& copy)
{
	x = copy.x;
	y = copy.y;
	v = copy.v;
	shootV = copy.shootV;
	w = copy.w;
	h = copy.h;
	moveType = copy.moveType;
	source = copy.source;  //the SDL_Surface is the same, so don't need to load it again
	bullet = copy.bullet;
	Enemy::number += 1;     //we are copying a new Enemy
}
Enemy::Enemy(const Enemy& copy, int x_, int y_)
{
	x = x_;
	y = y_;
	v = copy.v;
	moveType = copy.moveType;
	shootV = copy.shootV;
	w = copy.w;
	h = copy.h;
	source = copy.source;  //the SDL_Surface is the same, so don't need to load it again
	bullet = copy.bullet;
	Enemy::number += 1;     //we are copying a new Enemy
}
Enemy::~Enemy(){
    Enemy::number -= 1;
}
bool Enemy::canShoot(){
    if(clock() - lastShoot < shootV){
        return false;
    }else{
        return true;
    }
}
Bullet* Enemy::shooting()       //return Bullet*
{
    Bullet* b = new Bullet(bullet, x+w/2, y+h, 0, v+bullet->yVel);     //bullet @ plane's top, &bullet is faster than the plane
    lastShoot = clock();
	return b;       //return enemy_bullet;
}
int Enemy::number = 0;
