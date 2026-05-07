#include <stdio.h>
#include "fonctions.h"

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "enemy.h"

#include <stdlib.h>
#include <math.h>

 /* 5 types d'ennemis :
 *   ENEMY_DRONE    : rectiligne, tir horizontal
 *   ENEMY_SINUS    : trajectoire sinusoïdale, tir horizontal
 *   ENEMY_TURRET   : fixe, vise le joueur et tire
 *   ENEMY_KAMIKAZE : fonce sur le joueur, pas de tir
 *   ENEMY_ZIGZAG   : rebondit verticalement, tir en diagonale
 */



 //  DONNEES GLOBALES
Enemy       g_enemies[MAX_ENEMIES];
EnemyBullet g_enemy_bullets[MAX_ENEMY_BULLETS];

 // OUTILS INTERNES

// Cherche un slot libre dans g_enemy_bullets et retourne son index,
  // ou -1 s'il n'y a plus de place.
static int bullet_alloc(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        if (!g_enemy_bullets[i].active)
            return i;
    }
    return -1;
}

// Tire un projectile depuis (ox, oy) avec la direction (dx, dy)
  // normalisée, à la vitesse ENEMY_BULLET_SPEED.
static void enemy_shoot(float ox, float oy, float dx, float dy, int dmg)
{
    int idx = bullet_alloc();
    if (idx < 0) return;

    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.001f) return;

    g_enemy_bullets[idx].x      = ox;
    g_enemy_bullets[idx].y      = oy;
    g_enemy_bullets[idx].vx     = (dx / len) * ENEMY_BULLET_SPEED;
    g_enemy_bullets[idx].vy     = (dy / len) * ENEMY_BULLET_SPEED;
    g_enemy_bullets[idx].damage = dmg;
    g_enemy_bullets[idx].active = true;
}


 // SPAWN : valeurs par défaut selon le type

static void enemy_init_defaults(Enemy *e, EnemyType type, float x, float y)
{
    /* Remise à zéro complète */
    *e = (Enemy){0};

    e->type           = type;
    e->state          = ESTATE_ALIVE;
    e->x              = x;
    e->y              = y;
    e->death_duration = 0.5f;  /* 0.5 s d'explosion */

    switch (type) {


     // DRONE : avance en ligne droite de droite vers gauche
     // - HP faible, score faible
     //  - Tir rectiligne horizontal toutes les 2 secondes

    case ENEMY_DRONE:
        e->w          = 32.0f;
        e->h          = 24.0f;
        e->hp         = e->hp_max = 2;
        e->score_value= 100;
        e->damage     = 1;
        e->speed      = 120.0f;
        e->vx         = -e->speed;
        e->vy         = 0.0f;
        e->shoot_rate = 2.0f;
        e->shoot_timer= 1.0f; /* premier tir après 1 s */
        e->color      = al_map_rgb(100, 200, 255);
        break;


     // SINUS : même déplacement horizontal que le Drone mais oscille verticalement (sinusoïde).


    case ENEMY_SINUS:
        e->w           = 28.0f;
        e->h           = 28.0f;
        e->hp          = e->hp_max = 3;
        e->score_value = 200;
        e->damage      = 1;
        e->speed       = 90.0f;
        e->vx          = -e->speed;
        e->origin_y    = y;
        e->amplitude   = 80.0f;   // pixels
        e->frequency   = 1.5f;    // Hz
        e->timer       = 0.0f;    // phase iniiale
        e->shoot_rate  = 2.5f;
        e->shoot_timer = 1.2f;
        e->color       = al_map_rgb(255, 180, 50);
        break;

     // TURRET : fixée sur le décor, ne bouge pas eaucoup de HP, score élevé


    case ENEMY_TURRET:
        e->w           = 36.0f;
        e->h           = 36.0f;
        e->hp          = e->hp_max = 8;
        e->score_value = 500;
        e->damage      = 2;
        e->speed       = 0.0f;
        e->vx          = 0.0f;
        e->vy          = 0.0f;
        e->shoot_rate  = 1.5f;
        e->shoot_timer = 0.5f;
        e->angle       = 0.0f;
        e->color       = al_map_rgb(180, 60, 60);
        break;


     // KAMIKAZE : fonce en ligne droite vers la position courante


    case ENEMY_KAMIKAZE:
        e->w           = 22.0f;
        e->h           = 22.0f;
        e->hp          = e->hp_max = 1;
        e->score_value = 300;
        e->damage      = 3;   /* l'impact fait mal ! */
        e->speed       = 200.0f;
        e->vx          = -80.0f;  /* vitesse par défaut */
        e->vy          = 0.0f;
        e->color       = al_map_rgb(255, 80, 255);
        break;


     // ZIGZAG : rebondit verticalement entre deux bornes,


    case ENEMY_ZIGZAG:
        e->w           = 30.0f;
        e->h           = 30.0f;
        e->hp          = e->hp_max = 5;
        e->score_value = 350;
        e->damage      = 1;
        e->speed       = 80.0f;
        e->vx          = -e->speed;
        e->vy          = 100.0f;           // commence à descendre
        e->bounce_min  = y - 120.0f;
        e->bounce_max  = y + 120.0f;
        e->shoot_rate  = 1.8f;
        e->shoot_timer = 0.9f;
        e->color       = al_map_rgb(80, 255, 130);
        break;

    default:
        break;
    }
}


 // INITIALISATION DU TABLEAU

void enemies_init(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        g_enemies[i].state = ESTATE_DEAD;

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        g_enemy_bullets[i].active = false;
}


 //  SPAWN

void enemy_spawn(EnemyType type, float x, float y)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (g_enemies[i].state == ESTATE_DEAD) {
            enemy_init_defaults(&g_enemies[i], type, x, y);
            return;
        }
    }
    // Plus de slot disponible : on ignore silencieusement
}


 //  GESTION DES DEGATS

void enemy_take_damage(Enemy *e, int dmg)
{
    if (e->state != ESTATE_ALIVE) return;
    e->hp -= dmg;
    if (e->hp <= 0) {
        e->hp          = 0;
        e->state       = ESTATE_DYING;
        e->death_timer = 0.0f;
    }
}


 // MISE A JOUR — COMPORTEMENTS INDIVIDUELS


// DRONE
static void update_drone(Enemy *e, float dt,
                          float player_x, float player_y)
{
    (void)player_x; (void)player_y;

    e->x += e->vx * dt;
    e->y += e->vy * dt;

    /* Tir horizontal vers la gauche */
    e->shoot_timer -= dt;
    if (e->shoot_timer <= 0.0f) {
        enemy_shoot(e->x - e->w * 0.5f, e->y, -1.0f, 0.0f, e->damage);
        e->shoot_timer = e->shoot_rate;
    }
}

// SINUS
static void update_sinus(Enemy *e, float dt,
                          float player_x, float player_y)
{
    (void)player_x; (void)player_y;

    e->timer += dt;
    e->x     += e->vx * dt;
    /* La position Y est recalculée depuis l'origine (pas d'intégration) */
    e->y      = e->origin_y
                + e->amplitude * sinf(2.0f * M_PI * e->frequency * e->timer);

    e->shoot_timer -= dt;
    if (e->shoot_timer <= 0.0f) {
        enemy_shoot(e->x - e->w * 0.5f, e->y, -1.0f, 0.0f, e->damage);
        e->shoot_timer = e->shoot_rate;
    }
}

// TURRET
static void update_turret(Enemy *e, float dt,
                           float player_x, float player_y)
{
    // Calcul de l'angle vers le joueur
    float dx = player_x - e->x;
    float dy = player_y - e->y;
    e->angle = atan2f(dy, dx);  // radians

    e->shoot_timer -= dt;
    if (e->shoot_timer <= 0.0f) {
        enemy_shoot(e->x, e->y,
                    cosf(e->angle), sinf(e->angle),
                    e->damage);
        e->shoot_timer = e->shoot_rate;
    }
    // La tourelle ne se déplace pas
}

// KAMIKAZE
static void update_kamikaze(Enemy *e, float dt,
                             float player_x, float player_y)
{
    float dx = player_x - e->x;
    float dy = player_y - e->y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist < 400.0f && dist > 1.0f) {
        // Se dirige directement vers le joueur
        e->vx = (dx / dist) * e->speed;
        e->vy = (dy / dist) * e->speed;
    } else {
        // Avance normalement vers la gauche
        e->vx = -80.0f;
        e->vy = 0.0f;
    }

    e->x += e->vx * dt;
    e->y += e->vy * dt;
}

// ZIGZAG
static void update_zigzag(Enemy *e, float dt,
                           float player_x, float player_y)
{
    e->x += e->vx * dt;
    e->y += e->vy * dt;

    // Rebond sur les bornes
    if (e->y < e->bounce_min) {
        e->y  = e->bounce_min;
        e->vy = fabsf(e->vy);   // repart vers le bas
    }
    if (e->y > e->bounce_max) {
        e->y  = e->bounce_max;
        e->vy = -fabsf(e->vy);  // repart vers le haut
    }

    /* Tir en diagonale vers le joueur (haut ou bas) */
    e->shoot_timer -= dt;
    if (e->shoot_timer <= 0.0f) {
        float dy = (player_y < e->y) ? -1.0f : 1.0f;
        enemy_shoot(e->x - e->w * 0.5f, e->y,
                    -1.0f, dy,
                    e->damage);
        e->shoot_timer = e->shoot_rate;
    }
}


 // MISE A JOUR GLOBALE DES ENNEMIS

void enemies_update(float dt, float player_x, float player_y ,int WIDTH,int HEIGHT)
{
    const float SCREEN_W = WIDTH; // à adapter à votre résolution
    const float SCREEN_H =  HEIGHT;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g_enemies[i];

        if (e->state == ESTATE_DEAD) continue;

        // Animation de mort
        if (e->state == ESTATE_DYING) {
            e->death_timer += dt;
            if (e->death_timer >= e->death_duration)
                e->state = ESTATE_DEAD;
            continue;  // pendant l'explosion on ne met plus à jour
        }

        // Comportement selon le type
        switch (e->type) {
            case ENEMY_DRONE:    update_drone   (e, dt, player_x, player_y); break;
            case ENEMY_SINUS:    update_sinus   (e, dt, player_x, player_y); break;
            case ENEMY_TURRET:   update_turret  (e, dt, player_x, player_y); break;
            case ENEMY_KAMIKAZE: update_kamikaze(e, dt, player_x, player_y); break;
            case ENEMY_ZIGZAG:   update_zigzag  (e, dt, player_x, player_y); break;
            default: break;
        }

        // Sortie d'écran → on détruit l'ennemi
        if (e->x + e->w < 0.0f ||
            e->x        > SCREEN_W + 100.0f ||
            e->y + e->h < 0.0f ||
            e->y        > SCREEN_H + 100.0f)
        {
            e->state = ESTATE_DEAD;
        }
    }
}

//  MISE A JOUR DES PROJECTILES ENNEMIS

void enemy_bullets_update(float dt,int WIDTH,int HEIGHT)
{
    const float SCREEN_W = WIDTH;
    const float SCREEN_H =  HEIGHT;

    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        EnemyBullet *b = &g_enemy_bullets[i];
        if (!b->active) continue;

        b->x += b->vx * dt * 60.0f; /* on garde la même échelle qu'avant */
        b->y += b->vy * dt * 60.0f;

        /* Hors écran → désactivation */
        if (b->x < -20.0f || b->x > SCREEN_W + 20.0f ||
            b->y < -20.0f || b->y > SCREEN_H + 20.0f)
        {
            b->active = false;
        }
    }
}


// DESSIN — ENNEMIS

void enemies_draw(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        const Enemy *e = &g_enemies[i];

        if (e->state == ESTATE_DEAD) continue;

        float half_w = e->w * 0.5f;
        float half_h = e->h * 0.5f;

        if (e->state == ESTATE_DYING) {
            // Explosion simple : rectangle qui grossit et s'éclaircit
            float ratio  = e->death_timer / e->death_duration;
            float expand = ratio * 20.0f;
            ALLEGRO_COLOR boom = al_map_rgba_f(1.0f, 0.6f, 0.1f,
                                               1.0f - ratio);
            al_draw_filled_rectangle(
                e->x - half_w - expand,
                e->y - half_h - expand,
                e->x + half_w + expand,
                e->y + half_h + expand,
                boom);
            continue;
        }

        // Corps de l'ennemi
        al_draw_filled_rectangle(
            e->x - half_w, e->y - half_h,
            e->x + half_w, e->y + half_h,
            e->color);

        // Barre de vie (rouge → vert)
        float hp_ratio = (float)e->hp / (float)e->hp_max;
        ALLEGRO_COLOR bar_color = al_map_rgb(
            (int)(255 * (1.0f - hp_ratio)),
            (int)(255 * hp_ratio), 0);
        float bar_w = e->w * hp_ratio;
        al_draw_filled_rectangle(
            e->x - half_w,
            e->y - half_h - 6.0f,
            e->x - half_w + bar_w,
            e->y - half_h - 2.0f,
            bar_color);

        // Visuel spécifique : canon de la tourelle
        if (e->type == ENEMY_TURRET) {
            float cx = e->x + cosf(e->angle) * half_w;
            float cy = e->y + sinf(e->angle) * half_h;
            al_draw_line(e->x, e->y, cx, cy,
                         al_map_rgb(255, 255, 255), 3.0f);
        }
    }
}


 //  DESSIN — PROJECTILES

void enemy_bullets_draw(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        const EnemyBullet *b = &g_enemy_bullets[i];
        if (!b->active) continue;

        al_draw_filled_circle(b->x, b->y, 5.0f,
                              al_map_rgb(255, 60, 60));
        /* Petit halo */
        al_draw_circle(b->x, b->y, 7.0f,
                       al_map_rgba(255, 100, 100, 100), 1.5f);
    }
}


 //  DETECTION DE COLLISION (AABB simple)

bool enemy_collides_point(const Enemy *e, float px, float py)
{
    if (e->state != ESTATE_ALIVE) return false;
    float half_w = e->w * 0.5f;
    float half_h = e->h * 0.5f;
    return (px >= e->x - half_w && px <= e->x + half_w &&
            py >= e->y - half_h && py <= e->y + half_h);
}

bool enemy_bullet_hits_player(const EnemyBullet *b,
                               float px, float py,
                               float pw, float ph)
{
    if (!b->active) return false;
    return (b->x >= px - pw * 0.5f && b->x <= px + pw * 0.5f &&
            b->y >= py - ph * 0.5f && b->y <= py + ph * 0.5f);
}
