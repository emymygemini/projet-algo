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

void spawn_EnemyNiv1(EnemyNiv1 *e,int WIDTH, int HEIGHT)
{

    e->x = WIDTH + 50;


    e->y = 80 + rand() % (HEIGHT - 160);

    e->active = true;
    e->nmbVie =3;
    e->laserCooldown = 0;
}
void spawn_NIV_ENEMY1(EnemyNiv1 enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (!enemies[i].active) {

            float new_y = 50 + rand() % (HEIGHT - 100);

            bool place_libre = true;

            // vérifie juste la hauteur
            for (int j = 0; j < MAX_ENEMIES; j++) {

                if (enemies[j].active &&
                    fabs(new_y - enemies[j].y) < 70)
                {
                    place_libre = false;
                }
            }

            if (place_libre) {

                enemies[i].x = WIDTH + 50;

                enemies[i].y = new_y;

                enemies[i].active = true;

                enemies[i].nmbVie = 3;

                enemies[i].laserCooldown = 0;
            }

            break;
        }
    }
}
void render_NIV_ENEMY1(EnemyNiv1 enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1){
    for (int i = 0; i < MAX_ENEMIES; i++) {

        draw_EnemyNiv1(&enemies[i],
                       EnemyNiv1);
    }

}
void update_NIV_ENEMY1(EnemyNiv1 enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]){

    for (int i = 0; i < MAX_ENEMIES; i++) {

        update_EnemyNiv1(&enemies[i],
                         vies,
                         ship_x,
                         ship_y,
                         ship_w,
                         ship_h);

        enemyNIV1_touche(&enemies[i],
                         bullets,
                         MAX_BULLETS,
                         laser,
                         MAX_LASER,
                         spray);
    }
}

void update_EnemyNiv1(EnemyNiv1 *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h){
    if (!e->active)
        return;

    e->x -= 3;

    if (e->x < -30)
        e->active = false;


    float left   = ship_x;
    float right  = ship_x + ship_w;

    float top    = ship_y;
    float bottom = ship_y + ship_h;

    if (right > e->x &&
        left < e->x + 30 &&
        bottom > e->y &&
        top < e->y + 30)
    {
        (*vies)--;

        e->active = false;

    }
}
void draw_EnemyNiv1( EnemyNiv1 *e,ALLEGRO_BITMAP *EnemyNiv1){
    if (!e->active)
        return;

    al_draw_scaled_bitmap(
        EnemyNiv1,
        0, 0,
        al_get_bitmap_width(EnemyNiv1),
        al_get_bitmap_height(EnemyNiv1),
        e->x,
        e->y,
        60,
        60,
        0
    );

    // ===== Barre de vie =====

    // vie max = 3
    float ratio = (float)e->nmbVie / 3.0f;

    if (ratio < 0)
        ratio = 0;

    int bar_w = 50;
    int bar_h = 5;

    // barre rouge qui diminue
    al_draw_filled_rectangle(
        e->x,
        e->y - 8,
        e->x + (bar_w * ratio),
        e->y - 8 + bar_h,
        al_map_rgb(255, 0, 0)
    );
}
void enemyNIV1_touche(EnemyNiv1 *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[])
{
    if (!e->active)
        return;

    if (e->laserCooldown > 0)
        e->laserCooldown--;

    // ===== TAILLE UNIQUE DE L'ENNEMI =====
    int EW = 60;
    int EH = 60;

    // ================= BULLETS =================
    for (int i = 0; i < MAX_BULLETS; i++) {

        if (!bullets[i].active)
            continue;

        if (bullets[i].x + 10 > e->x &&
            bullets[i].x < e->x + EW &&
            bullets[i].y + 3 > e->y &&
            bullets[i].y < e->y + EH)
        {
            bullets[i].active = false;

            e->nmbVie--;

            if (e->nmbVie <= 0) {
                e->active = false;
                return;
            }
        }
    }

    // ================= LASER =================
    for (int i = 0; i < MAX_LASER; i++) {

        if (!laser[i].active)
            continue;

        if (e->y + EH > laser[i].y &&
            e->y < laser[i].y + 2)
        {
            if (e->laserCooldown == 0) {

                e->nmbVie--;

                if (e->nmbVie <= 0) {
                    e->active = false;
                    return;
                }

                e->laserCooldown = 10;
            }
        }
    }

    // ================= SPRAY =================
    for (int i = 0; i < MAX_BULLETS; i++) {

        if (!spray[i].active)
            continue;

        for (int j = 0; j < 10; j++) {

            if (!spray[i].bullets[j].active)
                continue;

            float bx = spray[i].bullets[j].x;
            float by = spray[i].bullets[j].y;

            if (bx + 10 > e->x &&
                bx < e->x + EW &&
                by + 3 > e->y &&
                by < e->y + EH)
            {
                e->nmbVie--;


                if (e->nmbVie <= 0) {
                    e->active = false;
                    return;
                }
            }
        }
    }

    if (e->nmbVie <= 0)
        e->active = false;
}

void spawn_EnemyNiv1BIS(EnemyNiv1BIS *e,int WIDTH, int HEIGHT)
{

    e->x = WIDTH ;


    e->y = 80 + rand() % (HEIGHT - 160);

    e->wave_offset = (float)(rand() % 360);
    e->active = true;
    e->nmbVie =3;
    e->laserCooldown = 0;
}
void spawn_NIV_ENEMY1BIS(EnemyNiv1BIS enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (!enemies[i].active) {

            float new_y = 50 + rand() % (HEIGHT - 100);

            bool place_libre = true;

            // vérifie juste la hauteur
            for (int j = 0; j < MAX_ENEMIES; j++) {

                if (enemies[j].active &&
                    fabs(new_y - enemies[j].y) < 70)
                {
                    place_libre = false;
                }
            }

            if (place_libre) {

                enemies[i].x = WIDTH + 50;

                enemies[i].y = new_y;

                enemies[i].active = true;

                enemies[i].nmbVie = 3;

                enemies[i].laserCooldown = 0;
                enemies[i].wave_offset = (float)(rand() % 360);
            }

            break;
        }
    }
}
void update_NIV_ENEMY1BIS(EnemyNiv1BIS enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[]){

    for (int i = 0; i < MAX_ENEMIES; i++) {

        update_EnemyNiv1BIS(&enemies[i],
                         vies,
                         ship_x,
                         ship_y,
                         ship_w,
                         ship_h);

        enemyNIV1_toucheBIS(&enemies[i],
                         bullets,
                         MAX_BULLETS,
                         laser,
                         MAX_LASER,
                         spray);
    }
}

void update_EnemyNiv1BIS(EnemyNiv1BIS *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h){
    if (!e->active)
        return;

    e->x -= 3;
    // mouvement sinusoïdal
    e->y += sinf((e->x + e->wave_offset) * 0.02f) * 7.5f;

    // ===== limites écran =====
    int margin = 30;

    // si tu as une grotte, utilise ses limites :
    int top_limit = 80;
    int bottom_limit = 1100 - 80;

    // clamp
    if (e->y < top_limit)
        e->y = top_limit;

    if (e->y > bottom_limit - 420)
        e->y = bottom_limit - 420;

    if (e->x < -30)
        e->active = false;


    float left   = ship_x;
    float right  = ship_x + ship_w;

    float top    = ship_y;
    float bottom = ship_y + ship_h;

    if (right > e->x &&
        left < e->x + 30 &&
        bottom > e->y &&
        top < e->y + 30)
    {
        (*vies)--;

        e->active = false;

    }
}
void render_NIV_ENEMY1BIS(EnemyNiv1BIS enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1){
    for (int i = 0; i < MAX_ENEMIES; i++) {

        draw_EnemyNiv1BIS(&enemies[i],
                       EnemyNiv1);
    }

}
void draw_EnemyNiv1BIS( EnemyNiv1BIS *e,ALLEGRO_BITMAP *EnemyNiv1){
    if (!e->active)
        return;

    al_draw_scaled_bitmap(
        EnemyNiv1,
        0, 0,
        al_get_bitmap_width(EnemyNiv1),
        al_get_bitmap_height(EnemyNiv1),
        e->x,
        e->y,
        60,
        60,
        0
    );

    // ===== Barre de vie =====

    // vie max = 3
    float ratio = (float)e->nmbVie / 3.0f;

    if (ratio < 0)
        ratio = 0;

    int bar_w = 50;
    int bar_h = 5;

    // barre rouge qui diminue
    al_draw_filled_rectangle(
        e->x,
        e->y - 8,
        e->x + (bar_w * ratio),
        e->y - 8 + bar_h,
        al_map_rgb(255, 0, 0)
    );
}
void enemyNIV1_toucheBIS(EnemyNiv1BIS *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[])
{
    if (!e->active)
        return;

    if (e->laserCooldown > 0)
        e->laserCooldown--;

    // ===== TAILLE UNIQUE DE L'ENNEMI =====
    int EW = 60;
    int EH = 60;

    // ================= BULLETS =================
    for (int i = 0; i < MAX_BULLETS; i++) {

        if (!bullets[i].active)
            continue;

        if (bullets[i].x + 10 > e->x &&
            bullets[i].x < e->x + EW &&
            bullets[i].y + 3 > e->y &&
            bullets[i].y < e->y + EH)
        {
            bullets[i].active = false;

            e->nmbVie--;

            if (e->nmbVie <= 0) {
                e->active = false;
                return;
            }
        }
    }

    // ================= LASER =================
    for (int i = 0; i < MAX_LASER; i++) {

        if (!laser[i].active)
            continue;

        if (e->y + EH > laser[i].y &&
            e->y < laser[i].y + 2)
        {
            if (e->laserCooldown == 0) {

                e->nmbVie--;

                if (e->nmbVie <= 0) {
                    e->active = false;
                    return;
                }

                e->laserCooldown = 10;
            }
        }
    }

    // ================= SPRAY =================
    for (int i = 0; i < MAX_BULLETS; i++) {

        if (!spray[i].active)
            continue;

        for (int j = 0; j < 10; j++) {

            if (!spray[i].bullets[j].active)
                continue;

            float bx = spray[i].bullets[j].x;
            float by = spray[i].bullets[j].y;

            if (bx + 10 > e->x &&
                bx < e->x + EW &&
                by + 3 > e->y &&
                by < e->y + EH)
            {
                e->nmbVie--;


                if (e->nmbVie <= 0) {
                    e->active = false;
                    return;
                }
            }
        }
    }

    if (e->nmbVie <= 0)
        e->active = false;
}
