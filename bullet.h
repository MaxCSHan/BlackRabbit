class Screen;
class Enemy;
class Player;
class Boss;

//The bullet that will move on the screen
class Bullet
{
private:
    //position
    int x, y, w, h;
    SDL_Surface* source = nullptr;

    //The velocity
    int xVel, yVel;

public:
    friend Screen;
    friend Enemy;
    friend Player;
    static int number;
    Bullet();
    Bullet(int x_, int y_, int xVel_, int yVel_, SDL_Surface* source_);
    Bullet(const Bullet* copy);
    Bullet(const Bullet* copy, int x_, int y_);
    Bullet(const Bullet* copy, int x_, int y_, int xVel_, int yVel_);
    ~Bullet(){
        Bullet::number -= 1;
    }
    bool move1();
    bool move2();
    bool shootAtPlayer(Player& player);
    bool shootAtEnemy(Enemy& enemy);
    bool shootAtBoss(Boss& boss);
    bool isMine();
};
int Bullet::number = 0;
Bullet::Bullet()
{
    x = 0;
    y = 0;
    xVel = 0;
    yVel = 0;
    source = nullptr;
}

Bullet::Bullet(int x_, int y_, int xVel_, int yVel_, SDL_Surface* source_)
{
    //Initialize the velocity
    xVel = xVel_;
    yVel = yVel_;

    x = x_;
    y = y_;
    source = source_;
    w = source_->w;
    h = source_->h;
}
Bullet::Bullet(const Bullet* copy)
{
    x = copy->x;
    y = copy->y;
    xVel = copy->xVel;
    yVel = copy->yVel;
    source = copy->source;
    w = source->w;
    h = source->h;
    Bullet::number += 1;
}
Bullet::Bullet(const Bullet* copy, int x_, int y_)
{
    xVel = copy->xVel;
    yVel = copy->yVel;
    source = copy->source;
    w = source->w;
    h = source->h;
    x = x_ - w/2;
    y = y_;
    Bullet::number += 1;
}
Bullet::Bullet(const Bullet* copy, int x_, int y_, int xVel_, int yVel_)
{
    y = y_;
    xVel = xVel_;
    yVel = yVel_;
    source = copy->source;
    w = source->w;
    h = source->h;
    x = x_ - w/2;
    Bullet::number += 1;
}
// move straight
bool Bullet::move1()
{
    y += yVel;
    if(x > WINDOW_W || y > WINDOW_H || x < 0 || y < 0)
        return false;
    else
        return true;
}
//move slantly
bool Bullet::move2()
{
    x += xVel;
    y += yVel;
    if(x > WINDOW_W || y > WINDOW_H || x < -10 || y < -10)
        return false;
    else
        return true;
}
//the bullet is from my plane?
bool Bullet::isMine(){
    return (yVel<0);
}

//bool Bullet::shootAtPlayer(Player& player)
//{
//    if(sqrt((x-player.x)^2+(y-player.y)^2) <= player.h/2)
//        return true;
//    else
//        return false;
//}
//
//bool Bullet::shootAtEnemy(Enemy& enemy)
//{
//    if(sqrt((x-enemy.x)^2+(y-enemy.y)^2) <= enemy.h/2)
//        return true;
//    else
//        return false;
//}




