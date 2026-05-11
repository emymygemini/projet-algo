#ifndef ENEMY_H
#define ENEMY_H
#include "fonctions.h"

typedef struct {
    float x, y;
    float vx, vy;  // 🔥 direction
    bool active;
} BulletsEnemy;

typedef struct {
    float x;
    float y;
    bool active;
    int nmbVie;
    int laserCooldown;
} EnemyNiv1;
typedef struct {
    float x;
    float y;
    bool active;
    int nmbVie;
    int laserCooldown;
    float wave_offset;
} EnemyNiv1BIS;
void draw_EnemyNiv1( EnemyNiv1 *e,ALLEGRO_BITMAP *EnemyNiv1);
void update_EnemyNiv1(EnemyNiv1 *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h);
void spawn_NIV_ENEMY1(EnemyNiv1 enemies[],int MAX_ENEMIES,int WIDTH,int HEIGHT);
void render_NIV_ENEMY1(EnemyNiv1 enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1);
void update_NIV_ENEMY1(EnemyNiv1 enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]);
void spawn_EnemyNiv1(EnemyNiv1 *e,int WIDTH, int HEIGHT);
void enemyNIV1_touche(EnemyNiv1 *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]);
void spawn_NIV_ENEMY1BIS(EnemyNiv1BIS enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT);
void spawn_EnemyNiv1BIS(EnemyNiv1BIS *e,int WIDTH, int HEIGHT);
void update_NIV_ENEMY1BIS(EnemyNiv1BIS enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]);
void update_EnemyNiv1BIS(EnemyNiv1BIS *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h);
void render_NIV_ENEMY1BIS(EnemyNiv1BIS enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1);
void enemyNIV1_toucheBIS(EnemyNiv1BIS *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]);
void draw_EnemyNiv1BIS( EnemyNiv1BIS *e,ALLEGRO_BITMAP *EnemyNiv1);


#endif
