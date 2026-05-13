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

    e->x = WIDTH ;



    int marge_haut = 120;
    int marge_bas  = 120;

    int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

    // sécurité
    if (zone_spawn < 1)
        zone_spawn = 1;

    e->y = marge_haut + rand() % zone_spawn;

    e->active = true;
    e->nmbVie =3;
    e->laserCooldown = 0;
    e->dying = false;
    e->dyingTimer = 0;
}
void spawn_NIV_ENEMY1(EnemyNiv1 enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (!enemies[i].active) {

            int marge_haut = 120;
            int marge_bas  = 120;

            int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

            // sécurité
            if (zone_spawn < 1)
                zone_spawn = 1;

            int new_y = marge_haut + rand() % zone_spawn;
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

                enemies[i].x = WIDTH ;

                enemies[i].y = new_y;

                enemies[i].active = true;

                enemies[i].nmbVie = 3;

                enemies[i].laserCooldown = 0;
                enemies[i].dying = false;
                enemies[i].dyingTimer = 0;
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
    if (!e->active && !e->dying)
        return;

    if (e->dying) {

        e->dyingTimer--;

        if (e->dyingTimer <= 0) {
            e->dying = false;
            e->active = false;
        }

        return;
    }

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
    if (!e->active && !e->dying)
        return;
    if (e->dying) {

        draw_death_animation(
            e->x + 30,
            e->y + 30,
            e->dyingTimer
        );

        return;
    }

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
    if (!e->active || e->dying)
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
                e->dying = true;
                e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
                    return;
                }
            }
        }
    }

    if (e->nmbVie <= 0) {
        e->dying = true;
        e->dyingTimer = 15;
    }
}

void spawn_EnemyNiv1BIS(EnemyNiv1BIS *e,int WIDTH, int HEIGHT)
{

    e->x = WIDTH ;


    int marge_haut = 120;
    int marge_bas  = 120;

    int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

    // sécurité
    if (zone_spawn < 1)
        zone_spawn = 1;

    e->y = marge_haut + rand() % zone_spawn;

    e->wave_offset = (float)(rand() % 360);
    e->active = true;
    e->nmbVie =3;
    e->laserCooldown = 0;
    e->dying = false;
    e->dyingTimer = 0;
}
void spawn_NIV_ENEMY1BIS(EnemyNiv1BIS enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (!enemies[i].active) {

            int marge_haut = 120;
            int marge_bas  = 120;

            int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

            // sécurité
            if (zone_spawn < 1)
                zone_spawn = 1;

            int new_y = marge_haut + rand() % zone_spawn;
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

                enemies[i].x = WIDTH ;

                enemies[i].y = new_y;

                enemies[i].active = true;

                enemies[i].nmbVie = 3;

                enemies[i].laserCooldown = 0;
                enemies[i].wave_offset = (float)(rand() % 360);

                enemies[i].dying = false;
                enemies[i].dyingTimer = 0;
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
    if (!e->active && !e->dying)
        return;
    if (e->dying) {

        e->dyingTimer--;

        if (e->dyingTimer <= 0) {
            e->dying = false;
            e->active = false;
        }

        return;
    }

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
    if (!e->active && !e->dying)
        return;
    if (e->dying) {

        draw_death_animation(
            e->x + 30,
            e->y + 30,
            e->dyingTimer
        );

        return;
    }

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
    if (!e->active || !e->dying)
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
                e->dying = true;
e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
                    return;
                }
            }
        }
    }
    if (e->nmbVie <= 0) {
        e->dying = true;
        e->dyingTimer = 15;
    }
}
void spawn_EnemyNiv2(EnemyNiv2 *e,int WIDTH, int HEIGHT)
{

    e->x = WIDTH ;


    int marge_haut = 120;
    int marge_bas  = 120;

    e->y = marge_haut +
           rand() % (HEIGHT - marge_haut - marge_bas - 60);

    e->active = true;
    e->nmbVie =3;
    e->laserCooldown = 0;
    e->tirCooldown = rand() % 120;
    e->dying = false;
    e->dyingTimer = 0;
    for (int i = 0; i < 20; i++) {
        e->bullets[i].active = false;
    }
}
void spawn_NIV_ENEMY2(EnemyNiv2 enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (!enemies[i].active) {

            int marge_haut = 120;
            int marge_bas  = 120;

            int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

            // sécurité
            if (zone_spawn < 1)
                zone_spawn = 1;

            int new_y = marge_haut + rand() % zone_spawn;
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

                enemies[i].x = WIDTH ;

                enemies[i].y = new_y;

                enemies[i].active = true;

                enemies[i].nmbVie = 3;

                enemies[i].laserCooldown = 0;

                enemies[i].tirCooldown = rand() % 120;
                enemies[i].dying = false;
                enemies[i].dyingTimer = 0;
                for (int k = 0; k < 20; k++) {
                    enemies[i].bullets[k].active = false;
                }
            }

            break;
        }
    }
}
void enemyNIV2_TIRE(EnemyNiv2 *e,
                float ship_x,
                float ship_y)
{

    if (!e->active)
        return;

    if (e->tirCooldown > 0) {
        e->tirCooldown--;
        return;
    }

    for (int i = 0; i < 20; i++) {

        if (!e->bullets[i].active) {

            e->bullets[i].active = true;

            // position du tir
            e->bullets[i].x = e->x;
            e->bullets[i].y = e->y + 30;

            // tir horizontal vers la gauche
            e->bullets[i].vx = -8;
            e->bullets[i].vy = 0;

            // délai entre tirs
            e->tirCooldown = 90;

            break;
        }
    }
}
void render_NIV_ENEMY2(EnemyNiv2 enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1){
    for (int i = 0; i < MAX_ENEMIES; i++) {

        draw_EnemyNiv2(&enemies[i],
                       EnemyNiv1);
        draw_enemyNIV2_bullets(&enemies[i]);
    }

}
void update_NIV_ENEMY2(EnemyNiv2 enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[],int WIDTH, int HEIGHT){

    for (int i = 0; i < MAX_ENEMIES; i++) {

        update_EnemyNiv2(&enemies[i],
                         vies,
                         ship_x,
                         ship_y,
                         ship_w,
                         ship_h);

        enemyNIV2_touche(&enemies[i],
                         bullets,
                         MAX_BULLETS,
                         laser,
                         MAX_LASER,
                         spray);
        enemyNIV2_TIRE(&enemies[i], ship_x, ship_y);

        update_enemyNIV2_bullets(&enemies[i],
                             vies,
                             ship_x,
                             ship_y,
                             ship_w,
                             ship_h,
                             WIDTH,
                             HEIGHT);
    }
}
void draw_enemyNIV2_bullets(EnemyNiv2 *e)
{
    for (int i = 0; i < 20; i++) {

        if (!e->bullets[i].active)
            continue;
        al_draw_filled_rectangle(e->bullets[i].x,e->bullets[i].y,
                    e->bullets[i].x+10,e->bullets[i].y+5,
                    al_map_rgb(255,0,0));
    }
}

void update_EnemyNiv2(EnemyNiv2 *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h){
    if (!e->active && !e->dying)
        return;
    if (e->dying) {

        e->dyingTimer--;

        if (e->dyingTimer <= 0) {
            e->dying = false;
            e->active = false;
        }

        return;
    }

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
void draw_EnemyNiv2( EnemyNiv2 *e,ALLEGRO_BITMAP *EnemyNiv1){
    if (!e->active && !e->dying)
        return;
    if (e->dying) {

        draw_death_animation(
            e->x + 30,
            e->y + 30,
            e->dyingTimer
        );

        return;
    }

    al_draw_scaled_bitmap(
        EnemyNiv1,
        0, 0,
        al_get_bitmap_width(EnemyNiv1),
        al_get_bitmap_height(EnemyNiv1),
        e->x,
        e->y,
        80,
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

    // barre  qui diminue
    al_draw_filled_rectangle(
        e->x +15,
        e->y - 8,
        e->x +15 + (bar_w * ratio),
        e->y - 8 + bar_h,
        al_map_rgb(229, 195, 152)
    );
}
void enemyNIV2_touche(EnemyNiv2 *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[])
{
    if (!e->active || !e->dying)
        return;

    if (e->laserCooldown > 0)
        e->laserCooldown--;

    // ===== TAILLE UNIQUE DE L'ENNEMI =====
    int EW = 80;
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
                e->dying = true;
e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
                    return;
                }
            }
        }
    }

    if (e->nmbVie <= 0) {
        e->dying = true;
        e->dyingTimer = 15;
    }
}
void update_enemyNIV2_bullets(EnemyNiv2 *e,
                          int *vies,
                          float ship_x,
                          float ship_y,
                          int ship_w,
                          int ship_h,
                          int WIDTH,
                          int HEIGHT)
{
    for (int i = 0; i < 20; i++) {

        if (!e->bullets[i].active)
            continue;

        e->bullets[i].x += e->bullets[i].vx;
        e->bullets[i].y += e->bullets[i].vy;

        // hors écran
        if (e->bullets[i].x < 0 ||
            e->bullets[i].x > WIDTH ||
            e->bullets[i].y < 0 ||
            e->bullets[i].y > HEIGHT)
        {
            e->bullets[i].active = false;
        }

        // collision joueur
        if (e->bullets[i].x > ship_x &&
            e->bullets[i].x < ship_x + ship_w &&
            e->bullets[i].y > ship_y &&
            e->bullets[i].y < ship_y + ship_h)
        {
            (*vies)--;

            e->bullets[i].active = false;
        }
    }
}
void spawn_EnemyNiv2BIS(EnemyNiv2BIS *e,int WIDTH, int HEIGHT)
{

    e->x = WIDTH ;


    int marge_haut = 120;
    int marge_bas  = 120;

    e->y = marge_haut +
           rand() % (HEIGHT - marge_haut - marge_bas - 60);

    e->active = true;
    e->nmbVie =3;
    e->laserCooldown = 0;
    e->tirCooldown = rand() % 120;
    e->wave_offset = (float)(rand() % 360);
    for (int i = 0; i < 20; i++) {
        e->bullets[i].active = false;
    }
    e->dying = false;
    e->dyingTimer = 0;
}
void spawn_NIV_ENEMY2BIS(EnemyNiv2BIS enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT)
{
    for (int i = 0; i < MAX_ENEMIES; i++) {

        if (!enemies[i].active) {
            int marge_haut = 120;
            int marge_bas  = 120;

            int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

            // sécurité
            if (zone_spawn < 1)
                zone_spawn = 1;

            int new_y = marge_haut + rand() % zone_spawn;
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

                enemies[i].x = WIDTH ;

                enemies[i].y = new_y;

                enemies[i].active = true;

                enemies[i].nmbVie = 3;

                enemies[i].laserCooldown = 0;

                enemies[i].wave_offset = (float)(rand() % 360);

                enemies[i].tirCooldown = rand() % 120;
                enemies[i].dying = false;
                enemies[i].dyingTimer = 0;
                for (int k = 0; k < 20; k++) {
                    enemies[i].bullets[k].active = false;
                }
            }

            break;
        }
    }
}
void enemyNIV2BIS_TIRE(EnemyNiv2BIS *e,
                float ship_x,
                float ship_y)
{

    if (!e->active)
        return;

    if (e->tirCooldown > 0) {
        e->tirCooldown--;
        return;
    }

    for (int i = 0; i < 20; i++) {

        if (!e->bullets[i].active) {

            e->bullets[i].active = true;

            // position du tir
            e->bullets[i].x = e->x;
            e->bullets[i].y = e->y + 30;

            // tir horizontal vers la gauche
            e->bullets[i].vx = -8;
            e->bullets[i].vy = 0;

            // délai entre tirs
            e->tirCooldown = 90;

            break;
        }
    }
}
void render_NIV_ENEMY2BIS(EnemyNiv2BIS enemies[],int MAX_ENEMIES,ALLEGRO_BITMAP *EnemyNiv1){
    for (int i = 0; i < MAX_ENEMIES; i++) {

        draw_EnemyNiv2BIS(&enemies[i],
                       EnemyNiv1);
        draw_enemyNIV2BIS_bullets(&enemies[i]);
    }

}
void update_NIV_ENEMY2BIS(EnemyNiv2BIS enemies[],int MAX_ENEMIES,int *vies,float ship_x,float ship_y,int ship_w,int ship_h,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[],int WIDTH, int HEIGHT){

    for (int i = 0; i < MAX_ENEMIES; i++) {

        update_EnemyNiv2BIS(&enemies[i],
                         vies,
                         ship_x,
                         ship_y,
                         ship_w,
                         ship_h);

        enemyNIV2BIS_touche(&enemies[i],
                         bullets,
                         MAX_BULLETS,
                         laser,
                         MAX_LASER,
                         spray);
        enemyNIV2BIS_TIRE(&enemies[i], ship_x, ship_y);

        update_enemyNIV2BIS_bullets(&enemies[i],
                             vies,
                             ship_x,
                             ship_y,
                             ship_w,
                             ship_h,
                             WIDTH,
                             HEIGHT);
    }
}
void draw_enemyNIV2BIS_bullets(EnemyNiv2BIS *e)
{
    for (int i = 0; i < 20; i++) {

        if (!e->bullets[i].active)
            continue;
        al_draw_filled_rectangle(e->bullets[i].x,e->bullets[i].y,
                    e->bullets[i].x+10,e->bullets[i].y+5,
                    al_map_rgb(255,0,0));
    }
}

void update_EnemyNiv2BIS(EnemyNiv2BIS *e,int *vies,float ship_x,float ship_y,int ship_w,int ship_h){
    if (!e->active && !e->dying)
        return;

    if (e->dying) {

        e->dyingTimer--;

        if (e->dyingTimer <= 0) {
            e->dying = false;
            e->active = false;
        }

        return;
    }

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
void draw_EnemyNiv2BIS( EnemyNiv2BIS *e,ALLEGRO_BITMAP *EnemyNiv1){

    if (!e->active && !e->dying)
        return;
    if (e->dying) {

        draw_death_animation(
            e->x + 30,
            e->y + 30,
            e->dyingTimer
        );

        return;
    }

    al_draw_scaled_bitmap(
        EnemyNiv1,
        0, 0,
        al_get_bitmap_width(EnemyNiv1),
        al_get_bitmap_height(EnemyNiv1),
        e->x,
        e->y,
        80,
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

    // barre  qui diminue
    al_draw_filled_rectangle(
        e->x +15,
        e->y - 8,
        e->x +15 + (bar_w * ratio),
        e->y - 8 + bar_h,
        al_map_rgb(229, 195, 152)
    );
}
void enemyNIV2BIS_touche(EnemyNiv2BIS *e,Bullet bullets[],int MAX_BULLETS,BulletLASER laser[],int MAX_LASER,BulletSPRAY spray[])
{
    if (!e->active || e->dying)
        return;

    if (e->laserCooldown > 0)
        e->laserCooldown--;

    // ===== TAILLE UNIQUE DE L'ENNEMI =====
    int EW = 80;
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
                e->dying = true;
                e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
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
                    e->dying = true;
e->dyingTimer = 15;
                    return;
                }
            }
        }
    }

    if (e->nmbVie <= 0) {
        e->dying = true;
        e->dyingTimer = 15;
    }
}
void update_enemyNIV2BIS_bullets(EnemyNiv2BIS *e,
                          int *vies,
                          float ship_x,
                          float ship_y,
                          int ship_w,
                          int ship_h,
                          int WIDTH,
                          int HEIGHT)
{
    for (int i = 0; i < 20; i++) {

        if (!e->bullets[i].active)
            continue;

        e->bullets[i].x += e->bullets[i].vx;
        e->bullets[i].y += e->bullets[i].vy;

        // hors écran
        if (e->bullets[i].x < 0 ||
            e->bullets[i].x > WIDTH ||
            e->bullets[i].y < 0 ||
            e->bullets[i].y > HEIGHT)
        {
            e->bullets[i].active = false;
        }

        // collision joueur
        if (e->bullets[i].x > ship_x &&
            e->bullets[i].x < ship_x + ship_w &&
            e->bullets[i].y > ship_y &&
            e->bullets[i].y < ship_y + ship_h)
        {
            (*vies)--;

            e->bullets[i].active = false;
        }
    }
}

void enemyNIV3_TIRE(EnemyNiv3 *e,
                    float ship_x,
                    float ship_y)
{
    // ne tire pas si mort ou explosion
    if (!e->active || e->exploding)
        return;

    if (e->tirCooldown > 0) {
        e->tirCooldown--;
        return;
    }

    for (int i = 0; i < 20; i++) {

        if (!e->bullets[i].active) {

            e->bullets[i].active = true;

            // position de départ
            e->bullets[i].x = e->x + 20;
            e->bullets[i].y = e->y + 35;

            // direction FIXE vers le joueur
            float dx = ship_x - e->bullets[i].x;
            float dy = ship_y - e->bullets[i].y;

            float len = sqrt(dx * dx + dy * dy);

            if (len != 0) {
                dx /= len;
                dy /= len;
            }

            // vitesse fixe
            e->bullets[i].vx = dx * 6;
            e->bullets[i].vy = dy * 6;

            e->tirCooldown = 90;

            break;
        }
    }
}

void spawn_EnemyNiv3(EnemyNiv3 *e, int WIDTH, int HEIGHT) {
    e->x = WIDTH;

    e->exploding = false;
    e->explosionTimer = 0;

    // marges grotte
    int marge_haut = 120;
    int marge_bas  = 180;

    // hauteur disponible
    int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

    // sécurité
    if (zone_spawn < 1)
        zone_spawn = 1;

    e->y = marge_haut + rand() % zone_spawn;

    e->active = true;
    e->nmbVie = 3;
    e->laserCooldown = 0;
    e->tirCooldown = rand() % 120;

    for (int i = 0; i < 20; i++) {
        e->bullets[i].active = false;
    }
}

void spawn_NIV_ENEMY3(EnemyNiv3 enemies[],
                      int MAX_ENEMIES,
                      int WIDTH,
                      int HEIGHT) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            int marge_haut = 120;
            int marge_bas  = 180;

            // hauteur disponible
            int zone_spawn = HEIGHT - marge_haut - marge_bas - 70;

            // sécurité
            if (zone_spawn < 1)
                zone_spawn = 1;

            int new_y = marge_haut + rand() % zone_spawn;
            bool place_libre = true;

            // vérifie juste la hauteur
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j].active &&
                    fabs(new_y - enemies[j].y) < 70) {
                    place_libre = false;
                }
            }

            if (place_libre) {
                enemies[i].x = WIDTH ;

                enemies[i].y = new_y;

                enemies[i].active = true;

                enemies[i].nmbVie = 3;

                enemies[i].laserCooldown = 0;

                enemies[i].tirCooldown = rand() % 120;
            }

            break;
        }
    }
}

void render_NIV_ENEMY3(EnemyNiv3 enemies[], int MAX_ENEMIES, ALLEGRO_BITMAP *EnemyNiv1, float ship_x,
                    float ship_y,
                    int ship_w,
                    int ship_h,
                    int *vies) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        draw_EnemyNiv3(&enemies[i],
                       EnemyNiv1,ship_x, ship_y, ship_w, ship_h,vies);
        draw_enemyNIV3_bullets(&enemies[i]);
    }
}

void update_NIV_ENEMY3(EnemyNiv3 enemies[], int MAX_ENEMIES, int *vies, float ship_x, float ship_y, int ship_w,
                       int ship_h, Bullet bullets[], int MAX_BULLETS, BulletLASER laser[], int MAX_LASER,
                       BulletSPRAY spray[], int WIDTH, int HEIGHT) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        update_EnemyNiv3(&enemies[i],
                         vies,
                         ship_x,
                         ship_y,
                         ship_w,
                         ship_h);

        enemyNIV3_touche(&enemies[i],
                         bullets,
                         MAX_BULLETS,
                         laser,
                         MAX_LASER,
                         spray);
        enemyNIV3_TIRE(&enemies[i], ship_x, ship_y);

        update_enemyNIV3_bullets(&enemies[i],
                                 vies,
                                 ship_x,
                                 ship_y,
                                 ship_w,
                                 ship_h,
                                 WIDTH,
                                 HEIGHT);
    }
}

void draw_enemyNIV3_bullets(EnemyNiv3 *e) {
    for (int i = 0; i < 20; i++) {
        if (!e->bullets[i].active)
            continue;
        al_draw_filled_rectangle(e->bullets[i].x, e->bullets[i].y,
                                 e->bullets[i].x + 10, e->bullets[i].y + 5,
                                 al_map_rgb(255, 0, 0));
    }
}

void update_EnemyNiv3(EnemyNiv3 *e,
                      int *vies,
                      float ship_x,
                      float ship_y,
                      int ship_w,
                      int ship_h)
{
    if (!e->active && !e->exploding)
        return;

    // ===== EXPLOSION =====
    if (e->exploding) {

        e->explosionTimer--;

        if (e->explosionTimer <= 0) {

            e->exploding = false;
            e->active = false;
        }

        return;
    }

    // ===== MOUVEMENT =====
    e->x -= 3;

    // hors écran
    if (e->x < -100) {
        e->active = false;
        return;
    }

    // collision joueur
    if (ship_x + ship_w > e->x &&
        ship_x < e->x + 40 &&
        ship_y + ship_h > e->y &&
        ship_y < e->y + 70)
    {
        (*vies)--;

        // déclenche explosion
        e->exploding = true;
        e->explosionTimer = 30;
    }
}

void draw_EnemyNiv3(EnemyNiv3 *e,
                    ALLEGRO_BITMAP *EnemyNiv1,
                    float ship_x,
                    float ship_y,
                    int ship_w,
                    int ship_h,
                    int *vies)
{
    if (!e->active && !e->exploding)
        return;

    // ================= EXPLOSION =================
    if (e->exploding) {

        float center_x = e->x + 20;
        float center_y = e->y + 35;

        // rayon qui grandit
        float r = (30 - e->explosionTimer) * 5;

        // limite rayon
        if (r > 90)
            r = 90;

        // ===== collision explosion joueur =====

        float closestX = ship_x;
        float closestY = ship_y;

        if (center_x < ship_x)
            closestX = ship_x;
        else if (center_x > ship_x + ship_w)
            closestX = ship_x + ship_w;
        else
            closestX = center_x;

        if (center_y < ship_y)
            closestY = ship_y;
        else if (center_y > ship_y + ship_h)
            closestY = ship_y + ship_h;
        else
            closestY = center_y;

        float dx = center_x - closestX;
        float dy = center_y - closestY;

        float distance = sqrt(dx * dx + dy * dy);

        // dégâts UNE SEULE FOIS
        if (distance < r && e->explosionTimer == 25) {
            (*vies)--;
        }

        // ===== DESSIN EXPLOSION =====

        // extérieur rouge
        al_draw_filled_circle(
            center_x,
            center_y,
            r,
            al_map_rgb(255, 40, 0)
        );

        // milieu orange
        al_draw_filled_circle(
            center_x,
            center_y,
            r * 0.7,
            al_map_rgb(255, 140, 0)
        );

        // coeur jaune
        al_draw_filled_circle(
            center_x,
            center_y,
            r * 0.35,
            al_map_rgb(255, 255, 120)
        );

        return;
    }


    // ================= ENNEMI =================

    al_draw_scaled_bitmap(
        EnemyNiv1,
        0, 0,
        al_get_bitmap_width(EnemyNiv1),
        al_get_bitmap_height(EnemyNiv1),
        e->x,
        e->y,
        40,
        70,
        0
    );

    // ================= BARRE DE VIE =================

    float ratio = (float)e->nmbVie / 3.0f;

    if (ratio < 0)
        ratio = 0;

    int bar_w = 50;
    int bar_h = 5;

    al_draw_filled_rectangle(
        e->x + 15,
        e->y - 8,
        e->x + 15 + (bar_w * ratio),
        e->y - 8 + bar_h,
        al_map_rgb(68, 76, 78)
    );
}

void enemyNIV3_touche(EnemyNiv3 *e, Bullet bullets[], int MAX_BULLETS, BulletLASER laser[], int MAX_LASER,
                      BulletSPRAY spray[]) {
    if (!e->active && !e->exploding)
        return;

    if (e->laserCooldown > 0)
        e->laserCooldown--;

    // ===== TAILLE UNIQUE DE L'ENNEMI =====
    int EW = 40;
    int EH = 70;

    // ================= BULLETS =================
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active)
            continue;

        if (bullets[i].x + 10 > e->x &&
            bullets[i].x < e->x + EW &&
            bullets[i].y + 3 > e->y &&
            bullets[i].y < e->y + EH) {
            bullets[i].active = false;

            e->nmbVie--;

            if (e->nmbVie <= 0 && !e->exploding) {
                e->exploding = true;
                e->explosionTimer = 30;

                return;
            }
        }
    }

    // ================= LASER =================
    for (int i = 0; i < MAX_LASER; i++) {
        if (!laser[i].active)
            continue;

        if (e->y + EH > laser[i].y &&
            e->y < laser[i].y + 2) {
            if (e->laserCooldown == 0) {
                e->nmbVie--;

                if (e->nmbVie <= 0 && !e->exploding) {
                    e->exploding = true;
                    e->explosionTimer = 30;

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
                by < e->y + EH) {
                e->nmbVie--;


                if (e->nmbVie <= 0 && !e->exploding) {
                    e->exploding = true;
                    e->explosionTimer = 30;

                    return;
                }
            }
        }
    }

}

void update_enemyNIV3_bullets(EnemyNiv3 *e,
                              int *vies,
                              float ship_x,
                              float ship_y,
                              int ship_w,
                              int ship_h,
                              int WIDTH,
                              int HEIGHT) {

    for (int i = 0; i < 20; i++) {
        if (!e->bullets[i].active)
            continue;

        e->bullets[i].x += e->bullets[i].vx;
        e->bullets[i].y += e->bullets[i].vy;

        // hors écran
        if (e->bullets[i].x < -20 ||
     e->bullets[i].x > WIDTH + 20 ||
     e->bullets[i].y < -20 ||
     e->bullets[i].y > HEIGHT + 20)
        {
            e->bullets[i].active = false;
            continue;
        }
        // collision joueur
        if (e->bullets[i].x > ship_x &&
            e->bullets[i].x < ship_x + ship_w &&
            e->bullets[i].y > ship_y &&
            e->bullets[i].y < ship_y + ship_h) {
            (*vies)--;

            e->bullets[i].active = false;
        }
    }
}
void draw_death_animation(float x, float y, int timer)
{
    // commence gros puis rétrécit
    float r = timer * 1.5;

    // minimum
    if (r < 0)
        r = 0;

    // extérieur rouge/orange
    al_draw_filled_circle(
        x,
        y,
        r,
        al_map_rgb(255, 80, 0)
    );

    // milieu orange
    al_draw_filled_circle(
        x,
        y,
        r * 0.6,
        al_map_rgb(255, 180, 0)
    );

    // coeur jaune
    al_draw_filled_circle(
        x,
        y,
        r * 0.3,
        al_map_rgb(255, 255, 150)
    );
}


