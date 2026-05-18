
#ifndef ENEMY_H
#define ENEMY_H
#include "fonctions.h"
#include "menu.h"

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
    bool dying;
    int dyingTimer;
    bool counted;
} EnemyNiv1;
typedef struct {
    float x;
    float y;
    bool active;
    int nmbVie;
    int laserCooldown;
    float wave_offset;
    bool dying;
    int dyingTimer;
    bool counted;
} EnemyNiv1BIS;
typedef struct {
    float x;
    float y;
    bool active;
    int nmbVie;
    int laserCooldown;
    float wave_offset;
    bool dying;
    int dyingTimer;
    bool counted;

    BulletsEnemy bullets[20];
    int tirCooldown;

} EnemyNiv2;
typedef struct {
    float x;
    float y;
    bool active;
    int nmbVie;
    int laserCooldown;
    float wave_offset;
    bool dying;
    int dyingTimer;
    bool counted;

    BulletsEnemy bullets[20];
    int tirCooldown;

} EnemyNiv2BIS;
typedef struct {
    float x;
    float y;
    bool active;
    int nmbVie;
    int laserCooldown;
    float wave_offset;
    bool counted;

    BulletsEnemy bullets[20];
    int tirCooldown;

    bool exploding;      // explosion en cours
    int explosionTimer;  // durée explosion

} EnemyNiv3;
typedef struct
{
    int score;
    int enemiesKilled;
    int enemiesMissed;

} GameStats;
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
void spawn_EnemyNiv2(EnemyNiv2 *e,int WIDTH, int HEIGHT);
void spawn_NIV_ENEMY2(EnemyNiv2 enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT);
void enemyNIV2_TIRE(EnemyNiv2 *e,
                float ship_x,
                float ship_y);
void render_NIV_ENEMY2(EnemyNiv2 enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1);
void update_NIV_ENEMY2(EnemyNiv2 enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[],int WIDTH, int HEIGHT);
void update_EnemyNiv2(EnemyNiv2 *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h);
void draw_enemyNIV2_bullets(EnemyNiv2 *e);
void draw_EnemyNiv2( EnemyNiv2 *e,ALLEGRO_BITMAP *EnemyNiv1);
void enemyNIV2_touche(EnemyNiv2 *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]);
void update_enemyNIV2_bullets(EnemyNiv2 *e,
                          int *vies,
                          float ship_x,
                          float ship_y,
                          int ship_w,
                          int ship_h,
                          int WIDTH,
                          int HEIGHT);
void spawn_EnemyNiv2BIS(EnemyNiv2BIS *e,int WIDTH, int HEIGHT);
void spawn_NIV_ENEMY2BIS(EnemyNiv2BIS enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT);
void enemyNIV2BIS_TIRE(EnemyNiv2BIS *e,
                float ship_x,
                float ship_y);
void render_NIV_ENEMY2BIS(EnemyNiv2BIS enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1);
void update_NIV_ENEMY2BIS(EnemyNiv2BIS enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[],int WIDTH, int HEIGHT);
void update_EnemyNiv2BIS(EnemyNiv2BIS *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h);
void draw_enemyNIV2BIS_bullets(EnemyNiv2BIS *e);
void draw_EnemyNiv2BIS( EnemyNiv2BIS *e,ALLEGRO_BITMAP *EnemyNiv1);
void enemyNIV2BIS_touche(EnemyNiv2BIS *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]);
void update_enemyNIV2BIS_bullets(EnemyNiv2BIS *e,
                          int *vies,
                          float ship_x,
                          float ship_y,
                          int ship_w,
                          int ship_h,
                          int WIDTH,
                          int HEIGHT);
void spawn_EnemyNiv3(EnemyNiv3 *e,int WIDTH, int HEIGHT);
void spawn_NIV_ENEMY3(EnemyNiv3 enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT);
void enemyNIV3_TIRE(EnemyNiv3 *e,
                float ship_x,
                float ship_y);
void render_NIV_ENEMY3(EnemyNiv3 enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1, float ship_x,
                    float ship_y,
                    int ship_w,
                    int ship_h,
                    int *vies);
void update_NIV_ENEMY3(EnemyNiv3 enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[],int WIDTH, int HEIGHT);
void update_EnemyNiv3(EnemyNiv3 *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h);
void draw_enemyNIV3_bullets(EnemyNiv3 *e);
void enemyNIV3_touche(EnemyNiv3 *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]);
void update_enemyNIV3_bullets(EnemyNiv3 *e,
                          int *vies,
                          float ship_x,
                          float ship_y,
                          int ship_w,
                          int ship_h,
                          int WIDTH,
                          int HEIGHT);
void draw_EnemyNiv3(EnemyNiv3 *e,
                    ALLEGRO_BITMAP *EnemyNiv1,
                    float ship_x,
                    float ship_y,
                    int ship_w,
                    int ship_h,
                    int *vies);
void draw_death_animation(float x, float y, int timer);
void count_enemy1_stats(
    EnemyNiv1 enemies[],
    int MAX_ENEMIES,
    GameStats *stats,ALLEGRO_SAMPLE *son_explosion
);
void count_enemy2_stats(
    EnemyNiv2 enemies[],
    int MAX_ENEMIES,
    GameStats *stats,ALLEGRO_SAMPLE *son_explosion
);
void count_enemy3_stats(
    EnemyNiv3 enemies[],
    int MAX_ENEMIES,
    GameStats *stats,ALLEGRO_SAMPLE *son_explosion
);
void count_enemy2BIS_stats(
    EnemyNiv2BIS enemies[],
    int MAX_ENEMIES,
    GameStats *stats,ALLEGRO_SAMPLE *son_explosion
);
void count_enemy1BIS_stats(
    EnemyNiv1BIS enemies[],
    int MAX_ENEMIES,
    GameStats *stats,ALLEGRO_SAMPLE *son_explosion
);
void detecter_perte_vie(int vies_actuelles, ALLEGRO_SAMPLE *son_hit);
void jouer_son(ALLEGRO_SAMPLE *son);

#endif
