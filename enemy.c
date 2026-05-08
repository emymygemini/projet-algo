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



 //Enemy       g_enemies[MAX_ENEMIES];
//EnemyBullet g_enemy_bullets[MAX_ENEMY_BULLETS];


 //  OUTILS INTERNES


// Marge intérieure
#define MARGIN_TOP(e)  ((e)->h * 0.5f + 2.0f)
#define MARGIN_BOT(e)  (SCREEN_H - (e)->h * 0.5f - 2.0f)

/*static int bullet_alloc(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        if (!g_enemy_bullets[i].active)
            return i;
    return -1;
}

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

static bool aabb(float ax, float ay, float aw, float ah,
                 float bx, float by, float bw, float bh)
{
    return (fabsf(ax - bx) < (aw + bw) * 0.5f) &&
           (fabsf(ay - by) < (ah + bh) * 0.5f);
}


//  SPAWN  —  valeurs par défaut selon le type

static void enemy_init_defaults(Enemy *e, EnemyType type, float x, float y)
{
    *e = (Enemy){0};
    e->type           = type;
    e->state          = ESTATE_ALIVE;
    e->x              = x;
    e->y              = y;
    e->death_duration = 0.6f;

    switch (type) {


     // DRONE : ligne droite, tir horizontal

    case ENEMY_DRONE:
        e->w           = 32.0f;
        e->h           = 24.0f;
        e->hp          = e->hp_max = 2;
        e->score_value = 100;
        e->damage      = 1;
        e->speed       = 120.0f;
        e->vx          = -e->speed;
        e->vy          = 0.0f;
        e->shoot_rate  = 2.0f;
        e->shoot_timer = 1.0f;
        e->color       = al_map_rgb(100, 200, 255);
        break;


     // SINUS : oscillation verticale sinusoïdale, tir horizontal

    case ENEMY_SINUS:
        e->w           = 28.0f;
        e->h           = 28.0f;
        e->hp          = e->hp_max = 3;
        e->score_value = 200;
        e->damage      = 1;
        e->speed       = 90.0f;
        e->vx          = -e->speed;
        e->amplitude   = 80.0f;
        e->frequency   = 1.5f;
        e->timer       = 0.0f;
        e->shoot_rate  = 2.5f;
        e->shoot_timer = 1.2f;
        e->color       = al_map_rgb(255, 180, 50);


        {
            float min_orig = e->amplitude + e->h * 0.5f + 2.0f;
            float max_orig = SCREEN_H - e->amplitude - e->h * 0.5f - 2.0f;
            if (min_orig > max_orig) min_orig = max_orig = SCREEN_H * 0.5f;
            if (y < min_orig) y = min_orig;
            if (y > max_orig) y = max_orig;
            e->y       = y
            e->origin_y = y;
        }
        break;


     //TURRET : fixée au décor → scroll avec lui (vx = -scroll_speed)

    case ENEMY_TURRET:
        e->w           = 40.0f;
        e->h           = 40.0f;
        e->hp          = e->hp_max = 8;
        e->score_value = 500;
        e->damage      = 2;
        e->speed       = 0.0f;   // pas de déplacement propre
        e->vx          = 0.0f;   // sera mis à -scroll_speed dans update
        e->vy          = 0.0f;
        e->angle       = (float)M_PI; // canon pointe à gauche par défaut
        e->shoot_rate  = 1.5f;
        e->shoot_timer = 0.8f;
        e->color       = al_map_rgb(200, 60, 60);
        break;


     // KAMIKAZE : fonce sur le joueur à courte portée,

    case ENEMY_KAMIKAZE:
        e->w           = 24.0f;
        e->h           = 24.0f;
        e->hp          = e->hp_max = 1;
        e->score_value = 300;
        e->damage      = 3;
        e->speed       = 220.0f;
        e->vx          = -80.0f;
        e->vy          = 0.0f;
        e->color       = al_map_rgb(255, 80, 255);
        break;


     //ZIGZAG : rebondit verticalement, tir diagonale

    case ENEMY_ZIGZAG:
        e->w           = 30.0f;
        e->h           = 30.0f;
        e->hp          = e->hp_max = 5;
        e->score_value = 350;
        e->damage      = 1;
        e->speed       = 80.0f;
        e->vx          = -e->speed;
        e->vy          = 100.0f;
        e->shoot_rate  = 1.8f;
        e->shoot_timer = 0.9f;
        e->color       = al_map_rgb(80, 255, 130);


        e->bounce_min  = y - 120.0f;
        e->bounce_max  = y + 120.0f;
        if (e->bounce_min < MARGIN_TOP(e)) e->bounce_min = MARGIN_TOP(e);
        if (e->bounce_max > MARGIN_BOT(e)) e->bounce_max = MARGIN_BOT(e);
        // Si l'amplitude restante est trop faible, on centre
        if (e->bounce_max - e->bounce_min < 20.0f) {
            e->bounce_min = SCREEN_H * 0.5f - 60.0f;
            e->bounce_max = SCREEN_H * 0.5f + 60.0f;
        }
        break;

    default:
        break;
    }
}


 //  INITIALISATION

void enemies_init(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
        g_enemies[i].state = ESTATE_DEAD;
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        g_enemy_bullets[i].active = false;
}

void enemy_spawn(EnemyType type, float x, float y)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (g_enemies[i].state == ESTATE_DEAD) {
            enemy_init_defaults(&g_enemies[i], type, x, y);
            return;
        }
    }
}


 //  GESTION DES DÉGÂTS

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


 // COMPORTEMENTS INDIVIDUELS


//DRONE
static void update_drone(Enemy *e, float dt,
                          float player_x, float player_y)
{
    (void)player_x; (void)player_y;
    e->x += e->vx * dt;


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
    e->y      = e->origin_y
                + e->amplitude * sinf(2.0f * (float)M_PI * e->frequency * e->timer);


    if (e->y < MARGIN_TOP(e)) e->y = MARGIN_TOP(e);
    if (e->y > MARGIN_BOT(e)) e->y = MARGIN_BOT(e);

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
    /* Déplacement avec le décor (vx = -scroll_speed, fixé en amont)
    e->x += e->vx * dt;

    /* Visée : calcul de l'angle vers le joueur
    float dx = player_x - e->x;
    float dy = player_y - e->y;
    e->angle = atan2f(dy, dx);

    e->shoot_timer -= dt;
    if (e->shoot_timer <= 0.0f) {
        enemy_shoot(e->x, e->y,
                    cosf(e->angle), sinf(e->angle),
                    e->damage);
        e->shoot_timer = e->shoot_rate;
    }
}

// KAMIKAZE
static void update_kamikaze(Enemy *e, float dt,
                             float player_x, float player_y,
                             float player_w, float player_h)
{
    float dx   = player_x - e->x;
    float dy   = player_y - e->y;
    float dist = sqrtf(dx * dx + dy * dy);

    // Comportement : fonce sur le joueur si à portée
    if (dist < 420.0f && dist > 1.0f) {
        e->vx = (dx / dist) * e->speed;
        e->vy = (dy / dist) * e->speed;
    } else {
        e->vx = -100.0f;  // avance doucement vers la gauche
        e->vy = 0.0f;
    }

    e->x += e->vx * dt;
    e->y += e->vy * dt;

    /* FIX [3] : clampage vertical strict
    if (e->y < MARGIN_TOP(e)) { e->y = MARGIN_TOP(e); e->vy = 0.0f; }
    if (e->y > MARGIN_BOT(e)) { e->y = MARGIN_BOT(e); e->vy = 0.0f; }

    /* FIX [2] : collision AABB kamikaze ↔ joueur → explosion
    if (aabb(e->x, e->y, e->w, e->h,
             player_x, player_y, player_w, player_h)) {
        enemy_take_damage(e, 9999); /* déclenche ESTATE_DYING
    }
}

/* --- ZIGZAG -----------------------------------------------
static void update_zigzag(Enemy *e, float dt,
                           float player_x, float player_y)
{
    e->x += e->vx * dt;
    e->y += e->vy * dt;

    /* Rebond sur les bornes (déjà clampées à l'écran au spawn)
    if (e->y < e->bounce_min) {
        e->y  = e->bounce_min;
        e->vy =  fabsf(e->vy);
    }
    if (e->y > e->bounce_max) {
        e->y  = e->bounce_max;
        e->vy = -fabsf(e->vy);
    }

    e->shoot_timer -= dt;
    if (e->shoot_timer <= 0.0f) {
        float dir_y = (player_y < e->y) ? -1.0f : 1.0f;
        enemy_shoot(e->x - e->w * 0.5f, e->y,
                    -1.0f, dir_y, e->damage);
        e->shoot_timer = e->shoot_rate;
    }
}


 //  MISE À JOUR GLOBALE

void enemies_update(float dt, float scroll_speed,
                    float player_x, float player_y,
                    float player_w, float player_h)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        Enemy *e = &g_enemies[i];
        if (e->state == ESTATE_DEAD) continue;

        /* Animation d'explosion
        if (e->state == ESTATE_DYING) {
            e->death_timer += dt;
            if (e->death_timer >= e->death_duration)
                e->state = ESTATE_DEAD;
            /* FIX [1] : la tourelle morte scroll aussi pendant l'explosion
            if (e->type == ENEMY_TURRET)
                e->x -= scroll_speed * dt;
            continue;
        }

        /* FIX [1] : la tourelle suit le décor.
           On règle vx avant d'appeler update_turret().
        if (e->type == ENEMY_TURRET)
            e->vx = -scroll_speed;

        /* Dispatch comportement
        switch (e->type) {
            case ENEMY_DRONE:
                update_drone(e, dt, player_x, player_y);
                break;
            case ENEMY_SINUS:
                update_sinus(e, dt, player_x, player_y);
                break;
            case ENEMY_TURRET:
                update_turret(e, dt, player_x, player_y);
                break;
            case ENEMY_KAMIKAZE:
                /* FIX [2] : on passe player_w/h pour la collision AABB
                update_kamikaze(e, dt,
                                player_x, player_y,
                                player_w, player_h);
                break;
            case ENEMY_ZIGZAG:
                update_zigzag(e, dt, player_x, player_y);
                break;
            default: break;
        }

        // Sortie par la gauche ou trop loin à droite → mort silencieuse
        if (e->x + e->w * 0.5f < -10.0f ||
            e->x - e->w * 0.5f > SCREEN_W + 200.0f)
        {
            e->state = ESTATE_DEAD;
        }
        // Sortie par le haut/bas : uniquement si l'ennemi n'est pas
           un type qui devrait être clampé (sécurité générale)
        if (e->y + e->h * 0.5f < -50.0f ||
            e->y - e->h * 0.5f > SCREEN_H + 50.0f)
        {
            e->state = ESTATE_DEAD;
        }
    }
}


 //  MISE À JOUR DES PROJECTILES

void enemy_bullets_update(float dt)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        EnemyBullet *b = &g_enemy_bullets[i];
        if (!b->active) continue;

        b->x += b->vx * dt;
        b->y += b->vy * dt;

        if (b->x < -20.0f || b->x > SCREEN_W + 20.0f ||
            b->y < -20.0f || b->y > SCREEN_H + 20.0f)
            b->active = false;
    }
}


 //  DESSIN — ENNEMIS

void enemies_draw(void)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {
        const Enemy *e = &g_enemies[i];
        if (e->state == ESTATE_DEAD) continue;

        float hw = e->w * 0.5f;
        float hh = e->h * 0.5f;

        /* ── Explosion ──
        if (e->state == ESTATE_DYING) {
            float ratio  = e->death_timer / e->death_duration;
            float expand = ratio * 24.0f;
            /* Cercle orange qui grossit et disparaît
            al_draw_filled_circle(e->x, e->y,
                                  hw + expand,
                                  al_map_rgba_f(1.0f, 0.55f, 0.05f,
                                                1.0f - ratio));
            /* Halo blanc au centre
            al_draw_filled_circle(e->x, e->y,
                                  (hw + expand) * 0.4f,
                                  al_map_rgba_f(1.0f, 1.0f, 0.8f,
                                                1.0f - ratio));
            continue;
        }

        /* ── Corps ──
        al_draw_filled_rectangle(e->x - hw, e->y - hh,
                                 e->x + hw, e->y + hh,
                                 e->color);

        /* ── Barre de vie ──
        float hp_ratio = (float)e->hp / (float)e->hp_max;
        ALLEGRO_COLOR bar_col = al_map_rgb(
            (int)(255 * (1.0f - hp_ratio)),
            (int)(255 * hp_ratio), 0);
        al_draw_filled_rectangle(
            e->x - hw,            e->y - hh - 7.0f,
            e->x - hw + e->w * hp_ratio, e->y - hh - 3.0f,
            bar_col);

        /* ── Détails visuels par type ──
        switch (e->type) {

        case ENEMY_DRONE:
            /* Petit triangle pointé à gauche
            al_draw_filled_triangle(
                e->x - hw - 8.0f, e->y,
                e->x - hw + 4.0f, e->y - 6.0f,
                e->x - hw + 4.0f, e->y + 6.0f,
                al_map_rgb(200, 240, 255));
            break;

        case ENEMY_SINUS:
            /* Cercle central
            al_draw_filled_circle(e->x, e->y, hw * 0.45f,
                                  al_map_rgb(255, 220, 100));
            break;

        case ENEMY_TURRET:
            /* Socle + canon orienté vers le joueur */
            /* Socle (triangle)
            al_draw_filled_triangle(
                e->x - hw, e->y + hh,
                e->x + hw, e->y + hh,
                e->x,      e->y,
                al_map_rgb(140, 40, 40));
            /* Canon : ligne épaisse dans la direction e->angle
            {
                float len = hw * 1.4f;
                al_draw_line(
                    e->x, e->y,
                    e->x + cosf(e->angle) * len,
                    e->y + sinf(e->angle) * len,
                    al_map_rgb(255, 230, 50), 4.0f);
                /* Cercle de pivot
                al_draw_filled_circle(e->x, e->y, 6.0f,
                                      al_map_rgb(255, 200, 50));
            }
            break;

        case ENEMY_KAMIKAZE:
            /* Croix pour indiquer le danger
            al_draw_line(e->x - hw, e->y - hh,
                         e->x + hw, e->y + hh,
                         al_map_rgb(255, 255, 255), 2.0f);
            al_draw_line(e->x + hw, e->y - hh,
                         e->x - hw, e->y + hh,
                         al_map_rgb(255, 255, 255), 2.0f);
            break;

        case ENEMY_ZIGZAG:
            /* Losange intérieur
            al_draw_filled_triangle(
                e->x,      e->y - hh,
                e->x - hw, e->y,
                e->x + hw, e->y,
                al_map_rgb(50, 200, 100));
            al_draw_filled_triangle(
                e->x,      e->y + hh,
                e->x - hw, e->y,
                e->x + hw, e->y,
                al_map_rgb(50, 200, 100));
            break;

        default: break;
        }
    }
}


 // DESSIN — PROJECTILES

void enemy_bullets_draw(void)
{
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        const EnemyBullet *b = &g_enemy_bullets[i];
        if (!b->active) continue;
        al_draw_filled_circle(b->x, b->y, 5.0f, al_map_rgb(255, 60, 60));
        al_draw_circle(b->x, b->y, 7.5f,
                       al_map_rgba(255, 120, 120, 120), 1.5f);
    }
}


 //  COLLISIONS

bool enemy_collides_rect(const Enemy *e,
                         float px, float py,
                         float pw, float ph)
{
    if (e->state != ESTATE_ALIVE) return false;
    return aabb(e->x, e->y, e->w, e->h, px, py, pw, ph);
}

bool enemy_bullet_hits_player(const EnemyBullet *b,
                              float px, float py,
                              float pw, float ph)
{
    if (!b->active) return false;
    return (b->x >= px - pw * 0.5f && b->x <= px + pw * 0.5f &&
            b->y >= py - ph * 0.5f && b->y <= py + ph * 0.5f);
}
*/
