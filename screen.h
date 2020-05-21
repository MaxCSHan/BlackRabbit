 /*視窗相關
    程式目的：開視窗、畫面繪製&刷新
*/

/*global variance*/
//about words
TTF_Font* FONTbig = NULL;
TTF_Font* FONTsmall = NULL;
//about colors
SDL_Color Black = {0,0,0};
SDL_Color White = {255,255,255};

class Screen{       //處理視窗、螢幕
private:
    float v=0.5;        //背景捲動速度
    float wv;       //wind speed
    float bg_x=0, bg_y=0, wx=0, wy=0;       //background position
    int bg_h,bg_w;
    SDL_Window* window = NULL;        //The window we create                      我們開的視窗
    SDL_Surface* bg = NULL;                  //我們的背景圖
    SDL_Surface* weather = NULL;
    SDL_Surface* head = NULL;
    SDL_Surface* header = NULL;
    SDL_Surface* screen = NULL;              //螢幕，所有圖片都要投影到這上面
public:
    Screen();
    Screen(int width,int height, const char title[], SDL_Surface* bg_, SDL_Surface* head_, SDL_Surface* weather_);
    void close();                               //全部關閉，釋放記憶體

    void changeBg(SDL_Surface* background);
    void clean();
    void setWind(bool isStrong);
    void scroll(bool);

    template<typename Item>   //自定義一個叫Item的type(實際傳來的可能是任何東西)
    void draw(const Item* it);   //這樣，這個函式可以接許多種不同的class(ex. Enemy, Bullet...)
    void draw(SDL_Surface* s, int x, int y);

    void print(const char* str, const SDL_Color textColor, int x_,int y_, TTF_Font* font);
    void paintNew();
    void addHeader();
    void setHeader(int lives, SDL_Surface* heart);
 };
Screen::Screen(){
    //Initialize SDL                         初始化SDL
    if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 )
    {
        cout<<"SDL could not initialize! SDL Error: "<<SDL_GetError()<<endl;
    }
     //Initialize different picture type          載入PNG、JPG支援
    int png = IMG_INIT_PNG, jpg = IMG_INIT_JPG;
    if( !( IMG_Init( png ) & png ) || !( IMG_Init( jpg ) & jpg )){
        cout<<"SDL_image could not initialize! SDL_image Error: %s\n"<<IMG_GetError()<<endl;
    }
     //Initialize SDL_ttf                               載入文字字型支援
    if( TTF_Init() == -1 ){
        cout<<"SDL_ttf could not initialize! SDL_ttf Error: %s\n"<<TTF_GetError()<<endl;
    }
    //Initialize SDL_mixer                           載入音樂支援
     if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0){
        cout<<"SDL_mixer could not initialize! SDL_mixer Error: "<<Mix_GetError()<<endl;
    }
    cout<<clock()<<"ms | SDL has initallized."<<endl;
}
Screen::Screen(int width,int height, const char title[], SDL_Surface* bg_, SDL_Surface* head_, SDL_Surface* weather_){
    Screen();
    //設定字體
    FONTbig = TTF_OpenFont( "font/IMMORTAL.ttf", 28 );       //設定字體&字型大小(注意檔案路徑)
    FONTsmall = TTF_OpenFont( "font/IMMORTAL.ttf", 18 );
    if( FONTbig == NULL || FONTsmall == NULL){
        cout<<"Failed to load fonts! SDL_ttf Error: "<<TTF_GetError()<<endl;
    }
    //設定視窗
    window = SDL_CreateWindow( title,    //視窗標題(const char*)
                              SDL_WINDOWPOS_UNDEFINED,      //開視窗的位置，預設：SDL_WINDOWPOS_UNDEFINED
                              SDL_WINDOWPOS_UNDEFINED,
                              width,height,  SDL_WINDOW_SHOWN );        //width、height :  int
    screen = SDL_GetWindowSurface( window );
    //背景
    weather = weather_;
    changeBg(bg_);
    //head(engin)
    head = head_;
    header = SDL_CreateRGBSurface(0, 500, 84, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);

    SDL_Rect rect;
    rect.x = 12; rect.y = 3;
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid( FONTbig, "lives",Black);       //"lives"
    SDL_BlitSurface(textSurface, NULL, head, &rect);
    SDL_FreeSurface(textSurface);
    textSurface = TTF_RenderUTF8_Solid( FONTbig, "score",Black);      //"score"
    rect.x = WINDOW_W*0.62;
    SDL_BlitSurface(textSurface, NULL, head, &rect);
    SDL_FreeSurface(textSurface);

    SDL_BlitSurface(head, NULL, header, NULL);
    SDL_BlitSurface(head, NULL, header, NULL);

    SDL_BlitSurface(bg, NULL, screen, NULL);
    paintNew();
    cout<<clock()<<"ms | Window setup."<<endl;
}
void Screen::close(){
    //Destroy surfaces
    SDL_FreeSurface(bg);
    SDL_FreeSurface(screen);
    SDL_FreeSurface(head);
    SDL_FreeSurface(header);
    screen = NULL;
    bg = NULL;
    //Destroy window
    SDL_DestroyWindow( window );
    window = NULL;
    cout<<"Window is closed.";
    //Quit SDL subsystems
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}
void Screen::changeBg(SDL_Surface* background){     //改背景並重新定位
    bg_x = 0;
    bg_y = 0;
    bg = background;
    bg_h = bg->h;
    bg_w = bg->w;
    setWind(false);
    wy = rand()%WINDOW_H - (int)(1.5*weather->h);
    wx = rand()%(WINDOW_W + 2*weather->w) - (int)(1.5*weather->w);
    paintNew();
}
void Screen::clean(){     //用背景全屏填滿
    //建立方塊
    SDL_Rect rect;
    rect.x = 0;
    //繪製螢幕下部(主要部分)
    rect.y = WINDOW_H - bg_h + (int)bg_y;
    SDL_BlitSurface(bg, NULL, screen, &rect);
    //繪製螢幕上部
    if(rect.y > 0){
        rect.y = rect.y - bg_h;
        SDL_BlitSurface(bg, NULL, screen, &rect);
    }
    //weather
    rect.y = (int)wy;
    rect.x = (int)wx;
    SDL_BlitSurface(weather, NULL, screen, &rect);
}
void Screen::setWind(bool isStrong){
    if(isStrong){
        wv *= -1.5;
    }else{
         wv = float(rand()%100) / 100;
        bool neg = (rand()%2);
        if(neg){
            wv *= -1;
        }
    }
}
void Screen::scroll(bool weatherOnly){
    if(!weatherOnly){
        bg_y += v;
        if(bg_y >= bg_h){
            bg_y = 0;
        }
        wy += 4.5*v;
        wx += wv;
    }else{
        wy += wv/4;
        wx += wv/4;
    }
    //weather out of screen
    if(wy >= WINDOW_H ||
       (wv>0 && wx > WINDOW_W) || (wv<=0 && wx < -weather->w) ){
        wy = rand()%(weather->h/4) - 1.25*weather->h; //y-axis unvisitable
        wx = rand()%WINDOW_W - 0.5*weather->w;
    }
}
template<typename Item> void Screen::draw(const Item* it){      //印出圖
    if(it->source == NULL || it->source==nullptr){
        return;
    }
     //建立方塊，以調整圖片大小
    SDL_Rect rect;
    rect.x = it->x;        rect.y = it->y;
    //繪製
    SDL_BlitSurface(it->source, NULL, screen, &rect);
}
void Screen::draw(SDL_Surface* s, int x, int y){
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    SDL_BlitSurface(s, NULL, screen, &rect);
}
void Screen::print(const char* str, const SDL_Color textColor, int x_, int y_, TTF_Font* font){  //印文字
    //draw the text
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid( font, str, textColor );    //build text as surface
    if( textSurface == NULL || textSurface==nullptr){
        cout<<"Unable to render text surface! SDL_ttf Error: %s\n"<<TTF_GetError()<<endl;
        return;
    }
    //build print area
    SDL_Rect rect;
    rect.x = x_ - (textSurface->w)/2;        //x_ = center of the text
    rect.y = y_;
    //draw
    SDL_BlitSurface(textSurface, NULL, screen, &rect);
    //Get rid of old surface
    SDL_FreeSurface( textSurface );
}
void Screen::paintNew(){        //更新畫面
     SDL_UpdateWindowSurface( window );
}
void Screen::addHeader(){
    //engin
    SDL_Rect rect;
    rect.y = -2;
    rect.x = -1;
    SDL_BlitSurface(header, NULL, screen, &rect);
}
void Screen::setHeader(int lives, SDL_Surface* heart){
    SDL_FreeSurface(header);
    header = SDL_CreateRGBSurface(0, 500, 84, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
    SDL_BlitSurface(head, NULL, header, NULL);
    SDL_BlitSurface(head, NULL, header, NULL);
    SDL_Rect rect;
    rect.x = 51;
    rect.y = -7;
    for(int i=0; i<lives; i++){
        SDL_BlitSurface(heart, NULL, header, &rect);
        rect.x += 26;
        rect.y = -7;
    }
}



//Program end
