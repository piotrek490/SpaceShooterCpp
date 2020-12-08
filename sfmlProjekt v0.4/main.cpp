#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <stdlib.h>
#include <string>
using namespace std;
using namespace sf;
unsigned frame = 0;

RenderWindow W; //Obiekt okna SFML


///Niezbedne rzeczy do menu
Texture playTexture, exitTexture, logoTexture;    //Przyciski play i exit
Sprite playSprite, exitSprite, logoSprite;
Texture playTextureActive, exitTextureActive;    //Przyciski play i exit
Sprite playSpriteActive, exitSpriteActive;
bool activeMenu = true; //Czy menu jest aktywne??
void showMenu();    //Funkcja sluzy do wyswietlania menu
void loadMenu();    //Funkcja laduje textury i sprite do menu
int yBtn = 340; int yLogo = 200;
int xPlayBtn = 300; int xLogo = 250;
int xExitBtn = 440;
void menuKeyboardEvent(Event);  //funkcja do obslugi myszy/przyciskow w menu
bool activeBtnPlay = false; //czy myszka najechala na przycisk Play
bool activeBtnExit = false; //czy myszka najechala na przycisk Exit

void gameReset();   //Reset stanu gry

///Funkcje
void configureWindow();
void loadTexturesSprites();
void loadEnemy();
void enemySpawner(); int nextEnemyTime = 120, enemyTimer = 0, maxEnemyInWave  = 4; ///nextEnemyTime - ilosc cykli do spawny przeciwnika
int numberOfEnemy = 0; ///ilosc zespawnowanych wrogow                              ///enemyTimer - zwykly timer odliczajacy do spawnu
int actualEnemyIndex = -1;                                                                                   ///maxEnemyInWave - ile wrogow moze byc w fali

bool firstBlood = false;        ///do sprawdzania czy pierwszy enemy zostal pokonany
bool tutEnemy1 = true;      ///BOOLe fal tutorialowych
bool tutEnemy2 = false;
bool tutEnemy3 = false;

///Klasy
class Airplane;
class HeroAirplane;
class EnemyAirplane;
class Bullet;
class EnemyBullet;
class HeroBullet;
class Interface;
class Level;
void collisionDetector(HeroBullet**, int, EnemyAirplane**, int);
void collisionDetectorHero(EnemyBullet**, int, HeroAirplane*, int);

class SkyBg;
class Star;

HeroAirplane* heroAirplane;
Interface* interface;
EnemyAirplane* airplaneEnemyArray[100];

void resetEnemyAirplane();

///Deklaracje Klas

class Airplane{      ///Klasa statków do dziedziczenia
public:
    Texture texture;    //tekstura statku
    Sprite sprite;      //sprite statku
    int posX, posY;     //koordynaty aktualnie
    int docX, docY;     //koordynaty docelowe
    int posXBulletAdd;

    ///METODY
    Airplane( int , int );
    Sprite getSprite();
    int getX();
    int getY();
    int getX_end();
    int getY_end();
};


class HeroAirplane : public Airplane {      ///Klasa statku którym sterujemy
private:
    int hp;     //punkty zycia
    int ammo, maxAmmo;   //ilocs amunicji
    int bulletTimer , weaponSpeed;  //Ogranicznik strzalów, ¿eby nie strzelac za szybko
    bool readyToShoot;  //Czy jest gotowy do strzalu
public:
    bool active;
    int score; //zdobyte punkty

    HeroAirplane(int, int);
    void tick();
    void keyboardEvent(Event);      //obs³uga klawiatury i myszki

    HeroBullet* bulletArray[100];   //Tablica pocisków
    void drawBullets();     //Funkcja rysowania posicków na ekranie
    bool getHit(int);

    void checkWeaponTick(); //tick do sprawdzania mo¿liwoœci strzalu

    void setMaxAmmo(int);
    void updateMaxAmmo(int);

    int getHp();
    int getAmmo();
    int getMaxAmmo();

    void resetBullets();
    void resetStats();
};


class EnemyAirplane : public Airplane {     ///Klasa statków przeciwnika
private:
    int hp, maxHp; //punkty zdrowia
    int speed;     //predkosc poruszania sie
    int aiLevel; //poziom ai (1-mieso armatnie ("ujadacz")  |   2-podlot_zwykly    |    3-mało HP to w nogi)
    int bulletTimer , weaponSpeed;  //Ogranicznik strzalów, zeby nie strzelac za szybko
    bool goToPosition;
    bool readyToShoot;
public:
    bool active;
    bool dead;
    EnemyAirplane(int, int, int, int); //(level, score, docX, docY)
    void aiTick(HeroAirplane);  //tick AI wroga
    void tick();    //reszta obliczen i ustalen np. sprite.setPosition
    void spawn(int, int);   //zmiania BOOL ACTIVE -> TRUE  +  wskazuje koordynaty do spawnu
    bool getHit(int); //zwraca true jesli konczy sie to smiercia
    int getHp();
    int score; //iule punktow wypada z przeciwnika
    EnemyBullet* bulletArray[100];
    void checkWeaponTick(); //tick do sprawdzania mo¿liwoœci strzalu
    void drawBullets();     //Funkcja rysowania posicków na ekranie
private:    ///Zmienne dla AI
    bool podlot; //qktywne gdy przeciwnik podlatuje
    bool podlot_zwykly; //podlatuje w dol
    bool podlot_atakujacy; //podlatuje naprzeciw gracza
    bool powrot;    //aktywne gdy wraca na pozycje docX docY
    bool wycofanie; //cofa sie do gory (mozliwie za swoich kolegow)
    bool unikanie;  //przeciwnik rusza sie na boki
    bool unikanie_prawo; //unika w prawo
    bool unikanie_lewo; //unika w lewo
    bool kotwiczenie; //kotwica na graczu - statek porusza sie za statkiem gracza na osi X
    int podlot_Y, podlot_rand;
    int wycofanie_Y;
    bool strzelaj;
};

class Bullet {      ///Klasa pocisku
public:
    bool active;    //Do sprawdzania czy pocisk jest aktywnym uczestnikiem rozgrywki
    int power;      //ile zadaje obrazen

    Texture texture;
    Sprite sprite;

    int posX, posY; //koordynaty
    int speed;  //Szybkosc lotu pocisku
    Bullet(int, int, int);

    Sprite getSprite();
    int getX();
    int getY();
    int bulletWidth, bulletHeight;

    void setBullet(int, int, int);  //Funkcja do ustawiania i aktywacji pocisku
};

class EnemyBullet : public Bullet { ///Klasa pocisków wystrzelonych przez wrogów
public:
    void setTexture(String);    //Funkcja do ustawiania tekstury (ró¿ni wrogowie to ró¿ne posicki)
    void tick();
    EnemyBullet(int, int, int, String);
};

class HeroBullet : public Bullet {  ///Klasa pocisków wystrzelonych przez nasza postac
public:
    HeroBullet(int, int, int);
    void tick();
};

/*
class Level {   ///Klasa poziomow odpowiada sa kolejne fale przeciwnikow                *********ZMIANA PLANOW*********
public:                                                                                 Zamiast poziomow bedzie spawn
    bool finish; //czy poziom zostal ukonczony                                          przeciwnikow z odliczaniem ile
    int level;                                                                          czasu zostalo do spawnu nastepnego
                                                                                        wroga (po smierci poprzedniego).
    EnemyAirplane* airplaneEnemyArray[10];

};
*/

class Interface {   ///Klasa interfejsu
private:
    bool podjasnij, przyciemnij;
    bool podjasnijHp, przyciemnijHp;
    int reloadAlpha, ammoAlpha, hpAlpha;
    Texture texture;
    Sprite sprite;

public:
    Interface();
    Font font1, font2, font3;
    void showInterface(HeroAirplane*);
};

class SkyBg {   ///Klasa tła kosmosu
private:
    int timeToStar, timer;
    void throwStar();
public:
    SkyBg();
    Star* starArray[50];
    void showStars();
    void tick();
};

class Star {
private:
    Texture texture;
    Sprite sprite;

    int posX, posY;
    int speedTimer, timer;
public:
    Star(int);
    bool active;
    void spawn(int,int);
    Sprite getSprite();
    void tick();
    int getSpeedTimer(){return speedTimer;};
};

/*
Texture p2, p3, p4, p5;         ///TEST
Sprite sp2, sp3, sp4, sp5;      ///TEST
int p2x = 50;                   ///TEST
*/

int main() ///########################################   MAIN
{
    srand(time(NULL));
    setlocale(LC_ALL,"");

    ///Obiekty gry
    heroAirplane = new HeroAirplane(350, 500);
    interface = new Interface();
    SkyBg skyBg;

    loadEnemy();
    loadMenu();

    configureWindow(); ///Funkcja konfiguracji okna SFML
    loadTexturesSprites();  ///Tutaj ladowanie tekstur i spritesów


    /*
    sp2.setPosition(p2x,100);   ///TEST
    sp3.setPosition(200,100);   ///TEST
    sp4.setPosition(350,100);   ///TEST
    sp5.setPosition(500,100);   ///TEST
    */

    Event e;  //EVENT SFML

    while(W.isOpen()){  //tutaj petla okna SFML
        /*
        p2x+=1;                     ///TEST
        sp2.setPosition(p2x,100);   ///TEST
        */

        /**====================*
            SEKCJA TICKÓW GRY
          *====================*/
        if(!activeMenu){
            enemySpawner();

            heroAirplane->tick();
            heroAirplane->checkWeaponTick();

            for(int i = 0; i<100; i++)
            if(airplaneEnemyArray[i] -> active){ //dzialaj jesli wrog jest aktywny
                airplaneEnemyArray[i] -> aiTick(*heroAirplane);
                airplaneEnemyArray[i] -> tick();
                airplaneEnemyArray[i] -> checkWeaponTick();
                collisionDetectorHero(airplaneEnemyArray[i]->bulletArray, 100, heroAirplane, 1);
            }


            collisionDetector(heroAirplane->bulletArray, 100, airplaneEnemyArray, 100);


        }
        skyBg.tick();

        /**====================*
              SEKCJA EVENTOWA
          *====================*/

        while(W.pollEvent(e)){
            if(e.type==Event::Closed || (Keyboard::isKeyPressed(Keyboard::Escape) )) W.close();

            if(activeMenu) menuKeyboardEvent(e);
            else {
                heroAirplane->keyboardEvent(e);
            }
        }


        /**====================*
             SEKCJA RYSOWANIA
          *====================*/
        W.clear(Color(33,28,47));
        for(int i = 0; i<50; i++) if(skyBg.starArray[i]->active) W.draw(skyBg.starArray[i]->getSprite());

        if(!activeMenu){
            W.draw(heroAirplane->getSprite());
            heroAirplane->drawBullets();

            for(int i = 0; i<100; i++)if(airplaneEnemyArray[i] -> active) W.draw(airplaneEnemyArray[i] -> getSprite());
            for(int i = 0; i<100; i++) airplaneEnemyArray[i] -> drawBullets();
            //W.draw(sp2); ///TEST
            //W.draw(sp3); ///TEST
            //W.draw(sp4); ///TEST
            //W.draw(sp5); ///TEST
            interface->showInterface(heroAirplane);
        } else showMenu();
        W.display();

    }

    return 0;
}  ///########################################   MAIN




    /**==============================================*
            SEKCJA DO DALSZEJ IMPLEMENTACJI KLAS
      *==============================================*/

    Airplane :: Airplane( int posX, int posY){
        this->docX = posX;
        this->docY = posY;
    }

    Sprite Airplane :: getSprite(){
        return sprite;
    }

    int Airplane :: getX(){
        return posX;
    }

    int Airplane :: getY(){
        return posY;
    }

    int Airplane :: getX_end(){
        return posX + texture.getSize().x;
    }

    int Airplane :: getY_end(){
        return posY + texture.getSize().y;
    }


    HeroAirplane :: HeroAirplane(int posX, int posY) : Airplane( posX,  posY){
        hp = 100;
        ammo = 4;     maxAmmo=4;
        bulletTimer = 25;   //Timer
        weaponSpeed = 25;   //Co ile tickow mozna oddac strzal
        score = 0;

        this->posX = docX;
        this->posY = docY;


        if (!texture.loadFromFile("./src/img/airPlanes/P1.png")) {}  ///Wszytanie textury Hero
        sprite.setTexture(texture);

        for(int i=0; i<100; i++){   ///Tworzenie obiektów bullet do tablicy
            bulletArray[i] = new HeroBullet(-33,-33,0);
            bulletArray[i] -> active = false;
        }

        this->posXBulletAdd = texture.getSize().x/2 - bulletArray[0]->texture.getSize().x/2;

        active = true;

    }

    void HeroAirplane :: tick(){
        sprite.setPosition(posX,posY);
        if(this->hp <= 0){
            active = false;
            activeMenu = true;
        }
    }

    bool HeroAirplane :: getHit(int power){
         this->hp -= power;
         cout<<"[Otrzymano obrazenia!]"<<endl;
         if(hp <= 0){
            //active = false;
            return true;
         } else return false;

     }

    void HeroAirplane :: checkWeaponTick(){     ///Sprawdzanie dostêpnoœci strza³u
        //cout<<bulletTimer<<endl;
        if(this->bulletTimer > weaponSpeed){
            bulletTimer = 0;
            readyToShoot = true;
        } else bulletTimer ++;
    }

    void HeroAirplane :: drawBullets(){         ///Rysowanie wystrzelonych (aktywnych) pocisków bohatera
        for(int i=0; i<100; i++){
            if(bulletArray[i] -> active){
                //cout<<"bulletArray[" << i <<"] activate"<<endl;
                bulletArray[i]->tick();
                W.draw(bulletArray[i]->getSprite());
            }
            else;
        }
    }

    void HeroAirplane :: resetBullets(){         ///Rysowanie wystrzelonych (aktywnych) pocisków bohatera
        for(int i=0; i<100; i++){
            if(bulletArray[i] -> active){
                bulletArray[i]->active = false;
            }
            else;
        }
    }

    void HeroAirplane :: resetStats(){         ///Rysowanie wystrzelonych (aktywnych) pocisków bohatera
        hp = 100;
        ammo = 30;     maxAmmo=30;
        bulletTimer = 25;   //Timer
        weaponSpeed = 25;   //Co ile tickow mozna oddac strzal
        score = 0;
    }

    void HeroAirplane :: keyboardEvent(Event e){    ///Funkcja obs³ugi klawiatury dla klasy HeroAirplane
        /*if(e.type == Event::KeyPressed && e.key.code == Keyboard::Right){    //Poruszanie klawiszami (KLATKUJE)
            posX+=10;
            sprite.setPosition(posX,posY);
        }
        if(e.type == Event::KeyPressed && e.key.code == Keyboard::Left){
            posX-=10;
            sprite.setPosition(posX,posY);
        }*/
        if(e.type == Event::MouseMoved){            ///Poruszanie myszk¹
            posX = Mouse::getPosition(W).x - sprite.getGlobalBounds().width/2;
            if(posX < 0) posX = 0;
            if(posX > 800 - sprite.getGlobalBounds().width) posX = 800 - sprite.getGlobalBounds().width;
            sprite.setPosition(posX,posY);
        }

        if(e.type == Event::KeyPressed && e.key.code == Keyboard::Space && readyToShoot && ammo > 0){     ///Strzelanie [SPACJA]
            for(int i=0; i<100; i++){
                if(!bulletArray[i] ->active){
                    bulletArray[i] -> setBullet(posX + posXBulletAdd , posY , 5);
                    ammo--;
                    cout << "AMMO: " << ammo << endl ;
                    if(ammo == 0) cout << "TIME TO RELOAD!" << endl ;
                    break;
                }
                else;
            }
            readyToShoot = false;
        }

        if(e.type == Event::KeyPressed && e.key.code == Keyboard::R && ammo <= 0){     ///RELOADING! [R]
            cout << "[RELOADING]" << endl ;
            ammo = maxAmmo;
        }
    }

    int HeroAirplane :: getHp(){
        return hp;
    }

    int HeroAirplane :: getAmmo(){
        return ammo;
    }

    int HeroAirplane :: getMaxAmmo(){
        return maxAmmo;
    }

    void HeroAirplane :: setMaxAmmo(int value){
        maxAmmo = value;
        ammo = maxAmmo;
    }

    void HeroAirplane :: updateMaxAmmo(int value){
        maxAmmo += value;
    }

    EnemyAirplane :: EnemyAirplane(int lv, int score, int posX, int posY) : Airplane( posX,  posY){
        maxHp = 5;
        speed = 3;
        active = false;
        dead = false;
        goToPosition = false;
        this -> score = score;


        String path;

        if(lv==0){ bulletTimer = rand()%60; weaponSpeed = 80; path ="./src/img/airPlanes/BP2.png";}
        else if(lv==1){ bulletTimer = rand()%50; weaponSpeed = 75; path ="./src/img/airPlanes/BP3.png";}
        else if(lv==2){ bulletTimer = rand()%40; weaponSpeed = 75; path ="./src/img/airPlanes/BP4.png";}
        else if(lv==3){ bulletTimer = rand()%40; weaponSpeed = 55; path ="./src/img/airPlanes/BP5.png";}
        else if(lv==4){ bulletTimer = rand()%40; weaponSpeed = 50; path ="./src/img/airPlanes/BP5.png";}
        else { bulletTimer = rand()%50; weaponSpeed = 65; path ="./src/img/airPlanes/BP5.png";};

        if(lv==2) maxHp = 7;
        else maxHp = 5;
        hp = maxHp;

        for(int i=0; i<100; i++){   ///Tworzenie obiektów bullet do tablicy
            bulletArray[i] = new EnemyBullet(-33,-33,0, path);
            bulletArray[i] -> active = false;
        }

        aiLevel = lv;

        if (lv==0 && !texture.loadFromFile("./src/img/airPlanes/P2.png")) {}  ///Wczytanie textury Enemy
        if (lv==1 && !texture.loadFromFile("./src/img/airPlanes/P3.png")) {}  ///Wczytanie textury Enemy
        if (lv==2 && !texture.loadFromFile("./src/img/airPlanes/P4.png")) {}  ///Wczytanie textury Enemy
        if (lv==3 && !texture.loadFromFile("./src/img/airPlanes/P5.png")) {}  ///Wczytanie textury Enemy
        if (lv==4 && !texture.loadFromFile("./src/img/airPlanes/P5.png")) {}  ///Wczytanie textury Enemy
        sprite.setTexture(texture);

        podlot_Y = 440;     podlot_rand = (rand()%60) - 80;
        podlot = false;
        podlot_zwykly = false;
        podlot_atakujacy = false;
        powrot = false;
        wycofanie = false;   wycofanie_Y = rand()%80;
        unikanie = false;
        unikanie_prawo = false;
        unikanie_lewo = false;
        kotwiczenie = false;
        strzelaj = false;
        readyToShoot = false;

        posXBulletAdd = texture.getSize().x/2 - bulletArray[0]->texture.getSize().x/2;
    }

    void EnemyAirplane :: spawn(int x, int y){     ///  Metoda do Spawnowania przeciwnika na mapie
        cout<<"SPAWN ( " << x << " , " << y << " )" << endl;
        docX = x;
        docY = y;
        posY = y;
        if(docX<300) posX = -200;
        else if(docX>500) posX = 800;
        else {
            int o = rand() % 2; //losowanie od 0 do 1   0-lewy spawn    1-prawy spawn
            if(o==0) posX = -200;
            else posX = 800;
        }
        hp = maxHp;
        active = true;
        goToPosition = true;
    }

    void EnemyAirplane :: aiTick(HeroAirplane heroAirplane){     ///  AI przeciwnika
        if(active){
            if(!goToPosition){
                if(aiLevel==0){     ///Camper (Utrzymuje pozycje)
                    strzelaj = true;
                }
                else if(aiLevel==1){     ///UJADACZ (podlatuje przed Hero | nie myśli | mięso armatnie)
                    podlot_atakujacy = true;
                    strzelaj = true;
                }
                else if(aiLevel==2){     ///BLISKI (podlot | gdy mało HP -> wraca na pozycje)
                    if(hp > (maxHp/2)){
                        podlot_zwykly = true;
                        strzelaj = true;
                    }
                    else {
                        podlot_zwykly = false;
                        powrot = true;
                        strzelaj = true;
                    }
                }
                else if(aiLevel==3){     ///RUCHLIWY (Wycofuje sie i kotwiczy)
                    wycofanie = true;
                    kotwiczenie = true;
                    strzelaj = true;
                }
                else if(aiLevel==4){     ///RUCHLIWY+Ucieczka (Wycofuje sie i kotwiczy | 1/3 HP -> unikanie)
                    if(hp > (maxHp/3)){
                        wycofanie = true;
                        kotwiczenie = true;
                        strzelaj = true;
                    }
                    else {
                        kotwiczenie = false;
                        unikanie = true;
                        strzelaj = true;
                    }
                }
                else;


                if(podlot_atakujacy){
                    if(getY_end() < podlot_Y) posY += speed;
                    if(posX < (heroAirplane.posX + podlot_rand)
                        && (heroAirplane.posX + podlot_rand) - posX >= speed) posX += speed;
                    else if(posX >= (heroAirplane.posX + podlot_rand)
                        && posX - (heroAirplane.posX + podlot_rand) >= speed) posX -= speed;
                    else if(posX < (heroAirplane.posX + podlot_rand)
                        && (heroAirplane.posX + podlot_rand) - posX < speed) posX += 1;
                    else if(posX >= (heroAirplane.posX + podlot_rand)
                        && posX - (heroAirplane.posX + podlot_rand) < speed) posX -= 1;
                }

                if(podlot_zwykly){
                    if(getY_end() < podlot_Y) posY += speed;
                }

                if(powrot){
                    if(posY < docY && (docY - posY)>speed) posY += speed;
                    else if(posY > docY && (posY - docY)>speed) posY -= speed;
                    else if(posY < docY && (docY - posY)<speed) posY += 1;
                    else if(posY > docY && (posY - docY)<speed) posY -= 1;
                    else;
                }

                if(wycofanie){
                    if(posY > wycofanie_Y) posY -= speed;
                }

                if(unikanie){
                    if(!unikanie_lewo && !unikanie_prawo){  //wybor pierwszego kierunku unikania
                        int choose = rand()%2;
                        if(choose==0) unikanie_lewo = true;
                        else unikanie_prawo = true;
                    }

                    if(unikanie_lewo){
                        if(posX > 0){ //tutaj dalej jade w lewo
                            posX-=speed;
                        } else {    //tutaj odbijam w prawo
                            unikanie_lewo = false;
                            unikanie_prawo = true;
                        }
                    }
                    else if(unikanie_prawo){
                        if(getX_end() < 800){ //tutaj dalej jade w prawo
                            posX+=speed;
                        } else {    //tutaj odbijam w prawo
                            unikanie_prawo = false;
                            unikanie_lewo = true;
                        }
                    }
                    else;
                }

                if(kotwiczenie){
                    if(posX < (heroAirplane.posX + podlot_rand)
                        && (heroAirplane.posX + podlot_rand) - posX >= speed) posX += speed;
                    else if(posX >= (heroAirplane.posX + podlot_rand)
                        && posX - (heroAirplane.posX + podlot_rand) >= speed) posX -= speed;
                    else if(posX < (heroAirplane.posX + podlot_rand)
                        && (heroAirplane.posX + podlot_rand) - posX < speed) posX += 1;
                    else if(posX >= (heroAirplane.posX + podlot_rand)
                        && posX - (heroAirplane.posX + podlot_rand) < speed) posX -= 1;
                }

                if(strzelaj){   ///skrypt strzelania
                    if(readyToShoot){
                        for(int i=0; i<100; i++){
                            if(!bulletArray[i] ->active){
                                bulletArray[i] -> setBullet(posX + posXBulletAdd , this->getY_end() , 5);
                                break;
                            }
                            else;
                        }
                        readyToShoot = false;
                    }
                }

            }
            else{   ///Tutaj operacja przejscia do ustalonej pozycji przeciwnika (tutaj po porstu leci i nie uzywa AI)
                if (posX < docX && (docX - posX)>=speed ) posX+=speed;
                else if (posX < docX && (docX - posX)<speed ) posX+=1;
                else if (posX > docX && (posX - docX)>=speed ) posX-=speed;
                else if (posX > docX && (posX - docX)<speed ) posX-=1;
                else if (posX == docX) goToPosition = false; //tutaj wylacza protokol goToPosition i wlacza AI
            }
        }
    }

     void EnemyAirplane :: tick(){
         if(active){
            sprite.setPosition(posX, posY);
         }
         else;
     }

     void EnemyAirplane :: checkWeaponTick(){     ///Sprawdzanie dostepnosci strzalu
        if(this->bulletTimer > weaponSpeed){
            bulletTimer = 0;
            readyToShoot = true;
        } else bulletTimer ++;
    }

     bool EnemyAirplane :: getHit(int power){
         this->hp -= power;
        cout<<"JEST getHit!"<<endl;
         if(hp <= 0){
            active = false;
            dead = true;
            numberOfEnemy --;
            return true;
         } else return false;

     }

     void EnemyAirplane :: drawBullets(){         ///Rysowanie wystrzelonych (aktywnych) pocisków przeciwnikow
        for(int i=0; i<100; i++){
            if(bulletArray[i] -> active){
                //cout<<"bulletArray[" << i <<"] activate"<<endl;
                bulletArray[i]->tick();
                W.draw(bulletArray[i]->getSprite());
            }
            else;
        }
    }

     int EnemyAirplane :: getHp(){
         return hp;
     }


    Bullet :: Bullet(int x, int y, int speed){
        active = true;
        this->posX = x;
        this->posY = y;
        this->speed = speed;
        power = 1;

        bulletHeight = 17;
        bulletWidth = 17;
    }

    Sprite Bullet :: getSprite(){
        return sprite;
    }

    int Bullet :: getX(){
        return posX;
    }

    int Bullet :: getY(){
        return posY;
    }

    void Bullet :: setBullet(int x, int y, int speed){  ///Metoda ustawiaj¹ca pociski i aktywuj¹ca je
        active = true;
        this->posX = x;
        this->posY = y;
        this->speed = speed;
    }

    void EnemyBullet :: setTexture(String path){
        if (!texture.loadFromFile(path)) {}
        sprite.setTexture(texture);
    }

    HeroBullet :: HeroBullet(int x, int y, int speed) : Bullet (x, y, speed){
        if (!texture.loadFromFile("./src/img/airPlanes/BP1.png")) {}
        sprite.setTexture(texture);
    }

    void HeroBullet :: tick(){
        if(this->active){
            posY -= speed;
            sprite.setPosition(posX,posY);

            if(posY<0) this->active = false;
        }
    }

    EnemyBullet :: EnemyBullet(int x, int y, int speed, String path) : Bullet (x, y, speed){
        if (!texture.loadFromFile(path)) {}
        sprite.setTexture(texture);
    }


    void EnemyBullet :: tick(){
        if(this->active){
            posY += speed;
            sprite.setPosition(posX,posY);

            if(posY>600) this->active = false;
        }
    }

    Interface :: Interface(){
        this->font1.loadFromFile("./src/font/AVGARDD_2.TTF");
        this->font2.loadFromFile("./src/font/AVGARDDO_2.TTF");
        this->font3.loadFromFile("./src/font/AVGARDN_2.TTF");
        podjasnij = false;
        przyciemnij = false;
        podjasnijHp = false;
        przyciemnijHp = false;
        ammoAlpha=255; reloadAlpha=100; hpAlpha=255;

        if (!texture.loadFromFile("./src/img/HUD.png")) {}
        sprite.setTexture(texture);
        sprite.setPosition(10,520);

        /*Text txt("testowy font", font1);
        txt.setCharacterSize(15);
        txt.setFillColor(Color(69,120,189));
        txt.setPosition(0,0);
        W.draw(txt);*/
    }

    void Interface :: showInterface(HeroAirplane* heroAirplane){

        string ammo = to_string(heroAirplane->getAmmo()) + "/" + to_string(heroAirplane->getMaxAmmo());
        string hp = to_string(heroAirplane->getHp()) + "%";
        string score = "SCORE: " + to_string(heroAirplane->score);
        Text txtAmmo(ammo, font1);
        Text txtHp(hp, font1);
        Text txtReload("[R] RELOAD", font1);
        Text txtScore(score, font1);
        txtAmmo.setCharacterSize(24);
        txtAmmo.setPosition(150 - txtAmmo.getGlobalBounds().width , 530);  ///wyswietlanie od prawej strony okienka HUD
        txtHp.setCharacterSize(24);
        txtHp.setFillColor(Color(69,120,189));
        txtHp.setPosition(20,530);
        txtReload.setCharacterSize(13);
        txtReload.setPosition(85 - (txtReload.getGlobalBounds().width/2),565);
        txtScore.setCharacterSize(19);
        txtScore.setFillColor(Color(69,120,189));
        txtScore.setPosition(10,10);
        if(heroAirplane->getAmmo()==0) {
                ammoAlpha = 100;
                if(!podjasnij && !przyciemnij){
                    podjasnij = true;
                }
                else if(przyciemnij && reloadAlpha>100){
                    reloadAlpha-=2;
                }
                else if(przyciemnij && reloadAlpha<=100){
                    przyciemnij = false;
                    podjasnij = true;
                }
                else if(podjasnij && reloadAlpha<=240){
                    reloadAlpha+=2;
                }
                else if(podjasnij && reloadAlpha>240){
                    podjasnij = false;
                    przyciemnij = true;
                }
        } else {    ///Jest ammo
            przyciemnij = false;
            podjasnij = false;
            reloadAlpha = 100;
            ammoAlpha = 255;
        }


        if(heroAirplane->getHp()<=20) {    ///Miganie przy niskiej ilosci HP
                if(!podjasnijHp && !przyciemnijHp){
                    przyciemnijHp = true;
                }
                else if(przyciemnijHp && hpAlpha>100){
                    hpAlpha-=2;
                }
                else if(przyciemnijHp && hpAlpha<=100){
                    przyciemnijHp = false;
                    podjasnijHp = true;
                }
                else if(podjasnijHp && hpAlpha<=240){
                    hpAlpha+=2;
                }
                else if(podjasnijHp && hpAlpha>240){
                    podjasnijHp = false;
                    przyciemnijHp = true;
                }
        } else {    ///Jest Hp
            przyciemnijHp = false;
            podjasnijHp = false;
            hpAlpha = 255;
        }


        txtReload.setColor( sf::Color( 69,120,189, reloadAlpha ) );
        txtAmmo.setColor( sf::Color( 69,120,189, ammoAlpha ) );
        txtHp.setColor( sf::Color( 69,120,189, hpAlpha ) );
        W.draw(sprite);
        W.draw(txtAmmo);
        W.draw(txtHp);
        W.draw(txtReload);
        W.draw(txtScore);
    }


    SkyBg :: SkyBg(){
        for(int i = 0; i < 50; i++){
            starArray[i] = new Star(10 - rand()%10);
        }

        timeToStar = 60;
        timer = 0;
    }

    void SkyBg :: tick(){
        for(int i = 0; i<50; i++) if(starArray[i]->active) starArray[i]->tick();
        if(timer >= timeToStar){
            if(rand()%100 == 77){
                throwStar();
                timer = 0 - rand()%60;
            }
        }
        else timer ++;
    }

    void SkyBg :: throwStar(){
        for(int i = 0; i < 50; i ++){
            if(!starArray[i]->active){
                starArray[i]->spawn(rand()%790 , -3);
                cout<<"leciiiiiii  |  speed: " <<starArray[i]->getSpeedTimer()<<endl;
                break;
            }
        }
    }

    Star :: Star(int speed){
        this->speedTimer = speed;
        active = false;
        timer = 0;

        if (!texture.loadFromFile("./src/img/star.png")) {}
        sprite.setTexture(texture);

        sprite.setColor( sf::Color( 255, 255, 255, 250-speedTimer*12 ) );
        sprite.setPosition(-10,-10);
    }

    void Star :: spawn(int x, int y){
        posX = x;
        posY = y;
        active = true;
    }

    void Star :: tick(){
        if(active){
            if(timer>=speedTimer) {
                    posY+=1;
                    sprite.setPosition(posX, posY);
                    timer=0;
            }
            else timer ++;
        }
        if(posY>600) active = false;
    }

    Sprite Star :: getSprite(){
        return sprite;
    }


    /**======================================*
            SEKCJA FUNKCJI POMOCNICZYCH
      *======================================*/

void configureWindow(){ ///Funkcja do konfiguracji okna SFML
    W.create(VideoMode(800,600,32),"Projekt SFML");
    W.setActive(true);
    W.setKeyRepeatEnabled(true);
    W.setFramerateLimit(60);
    //W.setVerticalSyncEnabled(true);
    W.setPosition(Vector2i(10,10));
}

void loadTexturesSprites(){ ///Funkcja wczytywania Teksturek i Spritesów
    /*
    if (!p2.loadFromFile("./src/placeholdery/p2.png")) {}  ///TEST
    sp2.setTexture(p2);

    if (!p3.loadFromFile("./src/placeholdery/p3.png")) {}  ///TEST
    sp3.setTexture(p3);

    if (!p4.loadFromFile("./src/placeholdery/p4.png")) {}  ///TEST
    sp4.setTexture(p4);

    if (!p5.loadFromFile("./src/placeholdery/p5.png")) {}  ///TEST
    sp5.setTexture(p5);
    */
}


void collisionDetector(HeroBullet* bulletArray[], int bulletArraySize, EnemyAirplane* airplaneArray[], int airplaneArraySize){
    for(int x = 0; x < bulletArraySize; x++){   //Sprawdzam kazdy pocisk
            if(!bulletArray[x]->active) continue; //Nooo prawie kazdy... nie sprawdza tych nieaktywnych
        for(int y = 0; y < airplaneArraySize; y++){  //Sprawdzam go dla kazdego statku (chyba ze wczesniej wykryje kolizje to breakuje)
            if(!airplaneArray[y]->active) continue;
            if(   bulletArray[x] -> getX() + bulletArray[x] -> bulletWidth >= airplaneArray[y] -> getX()
               && bulletArray[x] -> getX() < airplaneArray[y] -> getX_end()
               && bulletArray[x] -> getY() + bulletArray[x] -> bulletHeight > airplaneArray[y]->getY()
               && bulletArray[x] -> getY() < airplaneArray[y] -> getY_end()  )  //sprawdzam czy jest kolizja
            {
                cout << "WYKRYTO KOLIZJE! bullet -> power: " << bulletArray[x] -> power << endl;
                cout<< "HP przeciwnika: " <<airplaneArray[y]->getHp()<<endl;
                if(airplaneArray[y] -> getHit(bulletArray[x] -> power)){    //Jesli wrog zostaje pokonany przyznaje punkty graczowi
                    heroAirplane->score += airplaneArray[y]->score;
                }
                bulletArray[x] ->active = false; //usuwa pocisk po trafieniu
                break; //przejscie do sprawdzenia nastepnego pocisku
            }
        }
    }
}


void collisionDetectorHero(EnemyBullet* bulletArray[], int bulletArraySize, HeroAirplane* airplaneHero, int zmienna_bez_znaczenia){
    for(int x = 0; x < bulletArraySize; x++){   //Sprawdzam kazdy pocisk
            if(!bulletArray[x]->active) continue; //Nooo prawie kazdy... nie sprawdza tych nieaktywnych
            if(   bulletArray[x] -> getX() + bulletArray[x] -> bulletWidth >= airplaneHero -> getX()
               && bulletArray[x] -> getX() < airplaneHero -> getX_end()
               && bulletArray[x] -> getY() + bulletArray[x] -> bulletHeight > airplaneHero->getY()
               && bulletArray[x] -> getY() < airplaneHero -> getY_end()  )  //sprawdzam czy jest kolizja
            {
                cout << "WYKRYTO KOLIZJE! bullet -> power: " << bulletArray[x] -> power << endl;
                //cout<< "HP przeciwnika: " <<airplaneHero->getHp()<<endl;
                if(airplaneHero -> getHit(bulletArray[x] -> power)){    //Jesli wrog zostaje pokonany przyznaje punkty graczowi
                    //heroAirplane->score += airplaneHero[y]->score;
                }
                bulletArray[x] ->active = false; //usuwa pocisk po trafieniu
                break; //przejscie do sprawdzenia nastepnego pocisku
            }
    }
}

void loadEnemy(){

    for(int i=0; i<100; i++){
        if(i<10 && i!=3){
            airplaneEnemyArray[i] = new EnemyAirplane(0, 5, 200,100);
        }
        else if(i==3){
            airplaneEnemyArray[i] = new EnemyAirplane(1, 7, 200,100);
        }
        else if(i>=10 && i<20){
            int random = rand()%3;
            if(random == 1) airplaneEnemyArray[i] = new EnemyAirplane(0, 5, 200,100);
            else
            airplaneEnemyArray[i] = new EnemyAirplane(1, 7, 200,100);
        }
        else if(i>=20 && i<30){
            int random = rand()%6;
            if(random == 1) airplaneEnemyArray[i] = new EnemyAirplane(0, 5, 200,100);
            else if(random == 2) airplaneEnemyArray[i] = new EnemyAirplane(1, 7, 200,100);
            else
            airplaneEnemyArray[i] = new EnemyAirplane(2, 10, 200,100);
        }
        else if(i>=30 && i<40){
            int random = rand()%9;
            if(random == 1) airplaneEnemyArray[i] = new EnemyAirplane(0, 5, 200,100);
            else if(random == 2) airplaneEnemyArray[i] = new EnemyAirplane(1, 7, 200,100);
            else if(random == 6) airplaneEnemyArray[i] = new EnemyAirplane(2, 10, 200,100);
            else
            airplaneEnemyArray[i] = new EnemyAirplane(3, 15, 200,100);
        }
        else if(i>=40 && i<50){
            int random = rand()%12;
            if(random == 1) airplaneEnemyArray[i] = new EnemyAirplane(0, 5, 200,100);
            else if(random == 2) airplaneEnemyArray[i] = new EnemyAirplane(1, 7, 200,100);
            else if(random == 6) airplaneEnemyArray[i] = new EnemyAirplane(2, 10, 200,100);
            else if(random == 11) airplaneEnemyArray[i] = new EnemyAirplane(3, 15, 200,100);
            else
            airplaneEnemyArray[i] = new EnemyAirplane(4, 20, 200,100);
        }
        else{
            int lv = rand()%5;
            int score = 0;
            switch(lv){
                case 0:
                    score = 5;
                    break;
                case 1:
                    score = 7;
                    break;
                case 2:
                    score = 10;
                    break;
                case 3:
                    score = 15;
                    break;
                case 4:
                    score = 20;
                    break;
                default:
                    score = 25;
                    break;

            }
            airplaneEnemyArray[i] = new EnemyAirplane(lv, score, 200,100);
        }
    }

}

void enemySpawner(){
    //cout<< "numberOfEnemy: "<<numberOfEnemy<<endl;
    if(numberOfEnemy < maxEnemyInWave   &&  !tutEnemy1 &&  !tutEnemy2  &&  !tutEnemy3){
        enemyTimer++;
        if(enemyTimer >= nextEnemyTime){
            ///tutaj spawnuje przeciwnika z losowymi koordynatami i dodaje do numberOfEmeny +1
            if(actualEnemyIndex == 100) actualEnemyIndex = 0;
            for(int i = 0; i<100 ; i++){
                if(!airplaneEnemyArray[i]->active  && i >= actualEnemyIndex){
                    cout<< "Aktywuje airplaneEnemyArray: "<<i<<endl;
                    airplaneEnemyArray[i]->spawn(rand()%750, rand()%200+100);
                    numberOfEnemy++;
                    actualEnemyIndex = i;
                    if(actualEnemyIndex == 99) maxEnemyInWave ++;
                    enemyTimer = 0;
                    break;
                }
                else;
            }
        }

    }
    else if(tutEnemy1){  ///Pierwszy wrog tutorialowy (po pokonaniu zwieksza maxAmmo)
        if(airplaneEnemyArray[0]->dead){
            heroAirplane -> setMaxAmmo(30);
            tutEnemy2 = true;
            tutEnemy1 = false;
        }
        else if(!airplaneEnemyArray[0]->active){
            airplaneEnemyArray[0]->spawn(rand()%750, rand()%200+100);
            numberOfEnemy++;
            actualEnemyIndex = 0;
        }
        else;
    }
    else if(tutEnemy2){  ///Druga fala tutorialowa
        if(airplaneEnemyArray[1]->dead && airplaneEnemyArray[2]->dead){
            tutEnemy3 = true;
            tutEnemy1 = false;
            tutEnemy2 = false;
        }
        else if(!airplaneEnemyArray[1]->active && !airplaneEnemyArray[2]->active){
            airplaneEnemyArray[1]->spawn(rand()%750, rand()%200+100);
            numberOfEnemy++;
            airplaneEnemyArray[2]->spawn(rand()%750, rand()%200+100);
            numberOfEnemy++;
            actualEnemyIndex = 2;
        }
        else;
    }
    else if(tutEnemy3){  ///Druga fala tutorialowa
        if(airplaneEnemyArray[3]->dead){
            tutEnemy1 = false;
            tutEnemy2 = false;
            tutEnemy3 = false;
        }
        else if(!airplaneEnemyArray[3]->active){
            airplaneEnemyArray[3]->spawn(rand()%750, rand()%200+100);
            numberOfEnemy++;
            actualEnemyIndex = 3;
        }
        else;
    }
}

void loadMenu(){
    if (!playTexture.loadFromFile("./src/img/btnPlay.png")) {}
        playSprite.setTexture(playTexture);
    if (!exitTexture.loadFromFile("./src/img/btnExit.png")) {}
        exitSprite.setTexture(exitTexture);
    if (!logoTexture.loadFromFile("./src/img/menuLogo.png")) {}
        logoSprite.setTexture(logoTexture);
    if (!playTextureActive.loadFromFile("./src/img/btnPlayActive.png")) {}
        playSpriteActive.setTexture(playTextureActive);
    if (!exitTextureActive.loadFromFile("./src/img/btnExitActive.png")) {}
        exitSpriteActive.setTexture(exitTextureActive);

    playSprite.setPosition(xPlayBtn , yBtn);
    exitSprite.setPosition(xExitBtn , yBtn);
    logoSprite.setPosition(xLogo , yLogo);

    playSpriteActive.setPosition(xPlayBtn , yBtn);
    exitSpriteActive.setPosition(xExitBtn , yBtn);
}


void showMenu(){
    if(heroAirplane->score!=0){
        Font font1;
        font1.loadFromFile("./src/font/AVGARDD_2.TTF");
        string score = "LAST SCORE: " + to_string(heroAirplane->score);
        Text txtScore(score, font1);
        txtScore.setCharacterSize(19);
        txtScore.setFillColor(Color(69,120,189));
        txtScore.setPosition(10,10);
        W.draw(txtScore);
    }

    W.draw(logoSprite);
    if(activeBtnPlay) W.draw(playSpriteActive);
        else W.draw(playSprite);
    if(activeBtnExit) W.draw(exitSpriteActive);
        else W.draw(exitSprite);

}

void menuKeyboardEvent(Event e){
    if(e.type == Event::MouseButtonPressed){
        if(   Mouse::getPosition(W).x >= xPlayBtn       ///Obsluga przycisku PLAY
           && Mouse::getPosition(W).x <= xPlayBtn+60
           && Mouse::getPosition(W).y >= yBtn
           && Mouse::getPosition(W).y <= yBtn+60)
           {
                activeMenu = false;
                gameReset();
           }

        else if(   Mouse::getPosition(W).x >= xExitBtn       ///Obsluga przycisku EXIT
           && Mouse::getPosition(W).x <= xExitBtn+60
           && Mouse::getPosition(W).y >= yBtn
           && Mouse::getPosition(W).y <= yBtn+60)
           {
                W.close();
           }

        else;
    } else {

        if(   Mouse::getPosition(W).x >= xPlayBtn       ///Obsluga przycisku PLAY Bez kliku
           && Mouse::getPosition(W).x <= xPlayBtn+60
           && Mouse::getPosition(W).y >= yBtn
           && Mouse::getPosition(W).y <= yBtn+60)
           {
                activeBtnPlay = true;
                activeBtnExit = false;
           }

        else if(   Mouse::getPosition(W).x >= xExitBtn       ///Obsluga przycisku EXIT Bez kliku
           && Mouse::getPosition(W).x <= xExitBtn+60
           && Mouse::getPosition(W).y >= yBtn
           && Mouse::getPosition(W).y <= yBtn+60)
           {
                activeBtnPlay = false;
                activeBtnExit = true;
           }
        else {
            activeBtnPlay = false;
            activeBtnExit = false;
        }

    }
}

void gameReset(){
    bool firstBlood = false;
    bool tutEnemy1 = true;
    bool tutEnemy2 = false;
    bool tutEnemy3 = false;

    nextEnemyTime = 120; enemyTimer = 0; maxEnemyInWave  = 4;
    numberOfEnemy = 0;
    actualEnemyIndex = -1;

    heroAirplane->posX = 350;
    heroAirplane->posY = 500;
    heroAirplane->resetBullets();
    heroAirplane->resetStats();
    resetEnemyAirplane();
    heroAirplane->active = true;
}

void resetEnemyAirplane(){
    for(int i = 0; i < 100; i++){
        airplaneEnemyArray[i]->dead = false;
        airplaneEnemyArray[i]->active = false;
    }
}
