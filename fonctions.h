
// Created by helen on 24/04/2026.
//

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "enemy.h"


#ifndef PROJETALGO2SPACE_FONCTIONS_H
#define PROJETALGO2SPACE_FONCTIONS_H
void instalationaudio(int *return1);

typedef struct {
    float x;
    float gap_y;
    float gap_height;
    float width;
} Grotte;
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
void vie(int* vies, ALLEGRO_BITMAP *vie, int *fin) ;
void draw_bulletsLASER(BulletLASER bullets[], int MAX_BULLETS,int WIDTH, float x, float y);
void fire_bulletLASER(BulletLASER bullets[], float x, float y, int MAX_BULLETS);
void update_bulletsLASER(BulletLASER bullets[], float x, float y, int MAX_BULLETS, int collision );
bool bullet_active(BulletLASER bullets[], int MAX);
void DrawNombreTirsLaser(int tirsLaser, ALLEGRO_BITMAP *tirLaser) ;
bool laser_hits_enemy(float laser_y, float laser_height, Enemy *e);
void draw_bullets(Bullet bullets[], int MAX_BULLETS);
void update_bullets(Bullet bullets[], int MAX_BULLETS, int BULLET_SPEED, int WIDTH);
void fire_bullet(Bullet bullets[], float x, float y, int MAX_BULLETS);
void fire_bulletSPRAY(BulletSPRAY bullets[], float x, float y, int MAX_BULLETS);
void draw_bulletSpray(BulletSPRAY bullets[], int MAX_BULLETS);
void update_bulletsSpray(BulletSPRAY bullets[], int MAX_BULLETS, int WIDTH, int HEIGHT, int BULLET_SPEED);
void DrawTempsEntreTir(int temps, ALLEGRO_BITMAP *tirSPRAY, int WIDTH, int HEIGHT, int TEMPSATTENTE);
void Stalactites_init(Stalactites s[], int MAX_GROTTE, int WIDTH, int HEIGHT);
void Stalactites_update(Stalactites s[], int MAX_GROTTE, int WIDTH, int HEIGHT);
void stalactique_collision(Stalactites s[], int MAX_GROTTE,float *x, float *y,int ship_w, int ship_h,int HEIGHT, int *vie) ;
void stalactique_render(Stalactites s[], int MAX_GROTTE,int HEIGHT, int WIDTH,ALLEGRO_BITMAP *grotte);
void niv1(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display );
void niv2(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  );
void niv3(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  ) ;
void niv2Grotte(int HEIGHT, int WIDTH ,int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  );
void nivBOSS(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  );
#endif //PROJETALGO2SPACE_FONCTIONS_H