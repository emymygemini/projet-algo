//
// Created by frede on 06/05/2026.
//

#ifndef BOSSCODE_BOSS_H
#define BOSSCODE_BOSS_H
#ifndef BOSS_H
#define BOSS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdbool.h>
#include <math.h>

// ─── Dimensions ───────────────────────────────────────────────────────────────
#define BOSS_W            200       // largeur corps phase 1
#define BOSS_HEIGHT       300       // hauteur corps phase 1
#define BOSS_X_POS        870.0f    // position X fixe (côté droit)

#define PART_W            70        // largeur d'une partie sensible
#define PART_H            60        // hauteur d'une partie sensible
#define PART_HP_MAX       20        // coups nécessaires pour détruire une partie

#define BOSS_MINI_W       35       // largeur phase 2
#define BOSS_MINI_H       35       // hauteur phase 2
#define BOSS_MINI_HP      2         // coups pour tuer phase 2

// ─── Projectiles boss ─────────────────────────────────────────────────────────
#define MAX_BOSS_BULLETS  128
#define BOSS_BULLET_SPEED 5.0f

// ─── Laser boss ───────────────────────────────────────────────────────────────
#define LASER_WARN_FRAMES   120     // frames d'avertissement (clignotement)
#define LASER_ACTIVE_FRAMES  45     // frames où le laser est actif

// ─── Timing des attaques (frames) ─────────────────────────────────────────────
#define BOSS_ATTACK_MIN   150       // intervalle min entre attaques
#define BOSS_ATTACK_MAX   270       // intervalle max entre attaques

// ─── Dégâts laser joueur sur le boss ──────────────────────────────────────────
#define LASER_HIT_COOLDOWN 8        // frames entre deux dégâts du laser joueur

#define BOSS_MINI_W  30
#define BOSS_MINI_H  30
// ─── Structures ───────────────────────────────────────────────────────────────

typedef struct {
    float x, y;
    float vx, vy;
    bool  active;
} BossBullet;

typedef struct {
    float x, y;          // centre de la partie
    int   hp;
    bool  alive;
    int   hit_flash;     // frames de flash rouge lors d'un coup
    int   laser_cooldown;// protection contre les dégâts répétés du laser joueur
} BossPart;

typedef enum {
    BOSS_PHASE1 = 0,    // 3 parties à détruire
    BOSS_PHASE2,        // boss miniature, 2 HP
    BOSS_DEAD
} BossPhase;

typedef enum {
    ATTACK_NONE = 0,
    ATTACK_LASER         // attaque laser (état machine à états interne)
} BossAttackState;

typedef struct {
    BossPhase      phase;

    // Corps
    float          x, y;        // centre du boss
    float          w, h;        // dimensions courantes
    ALLEGRO_BITMAP *sprite;
    float          vx, vy;      // vitesse phase 2
    int            hp;          // HP pour phase 2

    // Flottement sinusoïdal
    float          float_timer;
    float          origin_y;
    float          float_amplitude;
    float          float_speed;   // incrément angle par frame (rad)

    // 3 parties sensibles
    BossPart       parts[3];

    // Machine à états des attaques
    BossAttackState attack_state;
    int             attack_timer;     // compte à rebours avant prochaine attaque

    // Laser boss
    float           laser_y;          // ligne Y du laser
    bool            laser_warning;    // true = avertissement, false = actif
    int             laser_timer;      // frames restantes (warn OU actif)

    // Flash de dégât (phase 2)
    int             hit_flash;

    // Mort
    bool            dying;
    int             death_timer;      // animation de mort

    // Projectiles
    BossBullet      bullets[MAX_BOSS_BULLETS];
} Boss;

// ─── Tableau global ───────────────────────────────────────────────────────────
extern Boss g_boss;

// ─── API publique ─────────────────────────────────────────────────────────────
void boss_init(int WIDTH, int HEIGHT);

// Appelé chaque frame dans l'événement ALLEGRO_EVENT_TIMER
void boss_update(float player_x, float player_y, int WIDTH, int HEIGHT);

// Appelé dans le bloc de rendu
void boss_draw(int WIDTH, int HEIGHT);

// Détection de collision : projectiles joueur → boss
// Inclure fonctions.h avant boss.h pour avoir Bullet, BulletLASER, BulletSPRAY
#include "fonctions.h"
#include <allegro5/allegro_image.h>

void boss_check_player_bullets  (Bullet      bullets[], int max);
void boss_check_player_laser    (BulletLASER bullets[], int max);
void boss_check_player_spray    (BulletSPRAY bullets[], int max);

// Détection de collision : projectiles boss / laser boss → joueur
// Retourne true si le joueur est touché (vous gérez la perte de vie)
bool boss_bullet_hits_player (float px, float py, float pw, float ph);
bool boss_laser_hits_player  (float px, float py, float pw, float ph);

// Statut
bool boss_is_dead(void);
bool boss_is_active(void);   // true si pas BOSS_DEAD



#endif // BOSS_H
#endif //BOSSCODE_BOSS_H