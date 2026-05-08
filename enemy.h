#ifndef ENEMY_H
#define ENEMY_H


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <math.h>


// limite de base


#define MAX_ENEMIES        64
#define MAX_ENEMY_BULLETS  128
#define ENEMY_BULLET_SPEED 5.0f


//  type d'ennemi
// drone vol rectiligne droite→gauche, tir droit
//  sinus trajectoire sinusoïdale, tir droit
//  canon fixe sur le décor, vise le joueur
//  kamikaze fonce droit sur le joueur, pas de tir
//  zigzag rebondit verticalement entre 2 bornes,
//  tir en diagonale vers le joueur
/*typedef enum {
   ENEMY_BASE = 0,
   ENEMY_TIRE,
   ENEMY_KAMIKASE,
   ENEMY_TYPE_COUNT
} EnemyType;


//  etat d'un ennemi
typedef enum {
   ESTATE_ALIVE = 0,
   ESTATE_DYING, // animation d'explosion
   ESTATE_DEAD
} EnemyState;


// projectile
typedef struct {
   float x, y;  // position
   float vx, vy;  // vitesse
   int   damage;   // dégâts infligés au joueur
   bool  active;
} EnemyBullet;


// ennemi
typedef struct {
   EnemyType  type;
   EnemyState state;


   // Hitbox
   float x, y;    // centre de l'ennemi
   float w, h;   // largeur / hauteur


   // Stats
   int   hp;       // points de vie restants
   int   hp_max;
   int   score_value;// points donnés à la mort
   int   damage;// dégâts au contact joueur


   // Mouvement
   float vx, vy;  // itesse courante
   float speed; // vitesse de base


   // Paramètres spécifiques au type
   float timer;
   float origin_y;
   float amplitude;
   float frequency;
   float bounce_min;
   float bounce_max;
   float angle;
   float shoot_timer;
   float shoot_rate;


   // Animation mort
   float death_timer;
   float death_duration;


   // Graphisme à remplacer par le truc d'helene
   ALLEGRO_COLOR color;

   float hit_timer;
} Enemy;


//tableau ennemi
extern Enemy       g_enemies[MAX_ENEMIES];
extern EnemyBullet g_enemy_bullets[MAX_ENEMY_BULLETS];


// V1 (a adapter selon le code des autres ) 


// initialisation
void enemies_init(void);
void enemy_spawn(EnemyType type, float x, float y);


// Mise à jour

void enemies_update(float dt, float scroll_speed,
                    float player_x, float player_y,
                    float player_w, float player_h);
void enemy_bullets_update(float dt,int WIDTH,int HEIGHT);


// Dessin
void enemies_draw(void);
void enemy_bullets_draw(void);


// specif
void   enemy_take_damage(Enemy *e, int dmg);
bool   enemy_collides_point(const Enemy *e, float px, float py);
bool   enemy_bullet_hits_player(const EnemyBullet *b,
                               float px, float py,
                               float pw, float ph);

*/
#endif
