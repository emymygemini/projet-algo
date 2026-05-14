
// Created by helen on 24/04/2026.
//
#ifndef FCT_H
#define FCT_H

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>


typedef enum {
    STATE_MENU,
    STATE_LEVEL_START,
    STATE_GAME,
    STATE_PAUSE,
    STATE_LEVEL_END,
    STATE_GAMEOVER
} GameState;


void instalationaudio(int *return1);

typedef struct {
    float x;
    float gap_y;
    float gap_height;
    float width;
} Grotte;
typedef struct {
    float x;
    float y;
    bool active;
} CoeurVie;
typedef struct {
    float x;
    float y;
    bool active;
} laserPlus;
typedef struct {
    float x;
    float gap_y;
    float gap_height;
    float width;
} Stalactites;
typedef struct {
    float x,y;
    bool active;
} Bullet;
typedef struct {
    float x, y;
    bool active;
    int timer; // durée restante
} BulletLASER;
typedef struct {
    float x, y;
    float vx, vy;  // 🔥 direction
    bool active;
} Bullets;
typedef struct {
    bool active;
    Bullets bullets[10];
} BulletSPRAY;
void grotte_render(Grotte g, int HEIGHT,int WIDTH,
            ALLEGRO_BITMAP *grotte );

void Updatescrolling(float *x, float *y,
            float *bgx,
            bool key[],
            int ship_w, int ship_h,
            int WIDTH, int HEIGHT,
            int SPEED, int SCROLL_SPEED,
            int bg_w);

void grotte_update(Grotte *g,
                   float *x, float *y,
                   int ship_w, int ship_h,
                   int HEIGHT,int WIDTH, int* vie, int *collision);


void render(float x, float y,
            float bgx,
            ALLEGRO_BITMAP *ship,
            ALLEGRO_BITMAP *background,
            int ship_w, int ship_h,
            int WIDTH, int HEIGHT,
            int bg_w);
void modeboss(float *x, float *y,
            ALLEGRO_BITMAP *ship,
           ALLEGRO_BITMAP *background,
           float bgx,
            bool key[],
            int ship_w, int ship_h,
            int WIDTH, int HEIGHT
            );
void vie(int* vies, ALLEGRO_BITMAP *vie, int *fin) ; // permet de dessiner les coeurs
void draw_bulletsLASER(BulletLASER bullets[], int MAX_BULLETS,int WIDTH, float x, float y); // fonction pour dessiner le laser
void fire_bulletLASER(BulletLASER bullets[], float x, float y, int MAX_BULLETS); //fonction qui gére si oui ou non on peut tirer un laser
void update_bulletsLASER(BulletLASER bullets[], float x, float y, int MAX_BULLETS, int collision ); // permet que le laser suive le vaisseau
bool bullet_active(BulletLASER bullets[], int MAX); // vérifie si un tir est actif ou pas
void DrawNombreTirsLaser(int tirsLaser, ALLEGRO_BITMAP *tirLaser) ; // affiche les tirs laser restant
//bool laser_hits_enemy(float laser_y, float laser_height, Enemy *e); // permet de savoir si un tir laser a touché un ennemi
void draw_bullets(Bullet bullets[], int MAX_BULLETS); // dessine les tirs simple
void update_bullets(Bullet bullets[], int MAX_BULLETS, int BULLET_SPEED, int WIDTH); // permet de faire défilier les tirs simple
void fire_bullet(Bullet bullets[], float x, float y, int MAX_BULLETS); // responsable de faire tirer le vaisseau
void fire_bulletSPRAY(BulletSPRAY bullets[], float x, float y, int MAX_BULLETS); //tire les tirs éparpillé
void draw_bulletSpray(BulletSPRAY bullets[], int MAX_BULLETS); //déssine les tirs épariller
void update_bulletsSpray(BulletSPRAY bullets[], int MAX_BULLETS, int WIDTH, int HEIGHT, int BULLET_SPEED); // met a jour les coordonné pour que ça défile
void DrawTempsEntreTir(int temps, ALLEGRO_BITMAP *tirSPRAY, int WIDTH, int HEIGHT, int TEMPSATTENTE); // barre de recharge des tirs
void Stalactites_init(Stalactites s[], int MAX_GROTTE, int WIDTH, int HEIGHT); // permet de dessiner les stalactiques
void Stalactites_update(Stalactites s[], int MAX_GROTTE, int WIDTH, int HEIGHT); // fait en sorte que ça défile
void spawn_coeur(CoeurVie *c,int WIDTH,Stalactites s[],int MAX_GROTTE); // permet de faire apparaitre un coeur a réculpere
void update_coeur(CoeurVie *c,int *vies,float ship_x,float ship_y,int ship_w,int ship_h); // vérifie si le joueur a récupérer un coeur et met a jour les coordonnées
void draw_coeur(CoeurVie *c,ALLEGRO_BITMAP *vie); // dessine le coeur
void spawn_laser(laserPlus *c,int WIDTH,int HEIGHT); //position aléatoire de laser que sans stalctiques
void draw_laserplus(laserPlus *c,ALLEGRO_BITMAP *laser);
void update_laser_Recup(laserPlus *c,int *maxBullet,float ship_x,float ship_y,int ship_w,int ship_h); //permet de récup laserPlus
void stalactique_collision(Stalactites s[], int MAX_GROTTE,float *x, float *y,int ship_w, int ship_h,int HEIGHT, int *vie) ; // vérifie si le vaisseau entre en collision avec les stalactiques
void stalactique_render(Stalactites s[], int MAX_GROTTE,int HEIGHT, int WIDTH,ALLEGRO_BITMAP *grotte);  // dessine les stalactiques
void niv1(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2 );
void niv2(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  );
void niv3(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  ) ;
void niv2Grotte(int HEIGHT, int WIDTH ,int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  );
void nivBOSS(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  );

#endif
