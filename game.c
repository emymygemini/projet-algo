#include <stdio.h>
#include "fonctions.h"
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <tgmath.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "enemy.h"
#include "fonctions.h"
#include "menu.h"



void Updatescrolling(float *x, float *y,
                     float *bgx,
                     bool key[],
                     int ship_w, int ship_h,
                     int WIDTH, int HEIGHT,
                     int SPEED, int SCROLL_SPEED,
                     int bg_w ) {


    if (key[ALLEGRO_KEY_UP] || key[ALLEGRO_KEY_LEFT])    *y -= SPEED;
    if (key[ALLEGRO_KEY_DOWN]|| key[ALLEGRO_KEY_RIGHT]) *y += SPEED;

    //Contraintes écran
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (*x > WIDTH - ship_w)  *x = WIDTH - ship_w;
    if (*y > HEIGHT - ship_h) *y = HEIGHT - ship_h;

    //Scrolling
    *bgx -= SCROLL_SPEED;
    if (*bgx <= -bg_w)
        *bgx = 0;

}

void render(float x, float y,
            float bgx,
            ALLEGRO_BITMAP *ship,
            ALLEGRO_BITMAP *background,
            int ship_w, int ship_h,
            int WIDTH, int HEIGHT,
            int bg_w ) {

    al_clear_to_color(al_map_rgb(0, 0, 0));

    //Fond défilant
    al_draw_scaled_bitmap(
background,
0, 0,
al_get_bitmap_width(background),
al_get_bitmap_height(background),
bgx, 0,
WIDTH, HEIGHT,
0
);
    al_draw_scaled_bitmap(
background,
0, 0,
al_get_bitmap_width(background),
al_get_bitmap_height(background),
bgx + bg_w, 0,
WIDTH, HEIGHT,
0
);

    //Vaisseau
    al_draw_scaled_bitmap(ship,
0, 0,
al_get_bitmap_width(ship),
al_get_bitmap_height(ship),
x, y,
ship_w, ship_h,
0
);

}

void modeboss(float *x, float *y,
            ALLEGRO_BITMAP *ship,
           ALLEGRO_BITMAP *background,
           float bgx,
            bool key[],
            int ship_w, int ship_h,
            int WIDTH, int HEIGHT
            ) {
    if (key[ALLEGRO_KEY_UP])    *y -= 8;
    if (key[ALLEGRO_KEY_DOWN]) *y += 8;
    if (key[ALLEGRO_KEY_LEFT])  *x -= 8;
    if (key[ALLEGRO_KEY_RIGHT]) *x += 8;

    //Contraintes écran
    if (*x < 0) *x = 0;
    if (*y < 0) *y = 0;
    if (*x > WIDTH - ship_w)  *x = WIDTH - ship_w;
    if (*y > HEIGHT - ship_h) *y = HEIGHT - ship_h;
    al_clear_to_color(al_map_rgb(0, 0, 0));

    //Fond défilant
    al_draw_scaled_bitmap(
background,
0, 0,
al_get_bitmap_width(background),
al_get_bitmap_height(background),
bgx, 0,
WIDTH, HEIGHT,
0
);

    al_draw_scaled_bitmap(ship,
0, 0,
al_get_bitmap_width(ship),
al_get_bitmap_height(ship),
*x, *y,
ship_w, ship_h,
0
);

}

void grotte_update(Grotte *g,
                   float *x, float *y,
                   int ship_w, int ship_h,
                   int HEIGHT, int WIDTH, int* vie, int *collision) {
    g->x -= 0;
    if (g->x + g->width < 0) {
        g->x = WIDTH;

    }

    // collision
    float left = *x;
    float right = *x + ship_w;
    float top = *y;
    float bottom = *y + ship_h;

    if (right > g->x && left < g->x + g->width) {
        if (top < g->gap_y - g->gap_height / 2 ||
            bottom > g->gap_y + g->gap_height / 2) {

            *vie-=1;
            *collision = 1;
            printf("%d", *vie);




            *x = 100;
            *y = HEIGHT / 2;
            }
    }
}


void grotte_render(Grotte g, int HEIGHT,int WIDTH,
            ALLEGRO_BITMAP *grotte ) {

    int tile_w = al_get_bitmap_width(grotte);
    int tile_h = al_get_bitmap_height(grotte);

    // height of top part
    int top_h = g.gap_y - g.gap_height / 2;

    for (int x = 0; x < WIDTH; x += tile_w) {
        al_draw_scaled_bitmap(
            grotte,
            0, 0,
            tile_w, tile_h,
            x,
            0,
            tile_w,
            top_h,
            0
        );
    }
    int bottom_y = g.gap_y + g.gap_height / 2;
    int bottom_h = HEIGHT - bottom_y;

    for (int x = 0; x < WIDTH; x += tile_w) {
        al_draw_scaled_bitmap(
            grotte,
            0, 0,
            tile_w, tile_h,
            x,
            bottom_y,
            tile_w,
            bottom_h,
            ALLEGRO_FLIP_VERTICAL   // 🔥 flip vertically
        );
    }
}

void vie(int* vies, ALLEGRO_BITMAP *vie, int *fin) {

    int taille = 30;      // taille des icônes
    int espace = 10;      // espace entre elles
    int vi= (*vies);

    if (*vies>3) {
        vi = 3;
    }

    for (int i = 0; i < vi; i++) {
        al_draw_scaled_bitmap(vie,0, 0,al_get_bitmap_width(vie),al_get_bitmap_height(vie),
            10 + i * (taille + espace),  // décalage horizontal
            10,                          // en haut
            taille,
            taille,
            0
        );
    }

    if (*vies<=0 ) {
        printf("mort");
        *fin=1;

    }
}


void fire_bulletLASER(BulletLASER bullets[], float x, float y, int MAX_BULLETS) {

        for(int i = 0; i < MAX_BULLETS; i++) {
            if(!bullets[i].active) {
                bullets[i].x = x+150;
                bullets[i].y = y+16;
                bullets[i].active = true;
                bullets[i].timer = 120; // 3 secondes à 60 FPS
                break;
            }
        }


}
void update_bulletsLASER(BulletLASER bullets[], float x, float y, int MAX_BULLETS, int collision) {
    if (collision==0){
        for(int i = 0; i < MAX_BULLETS; i++) {
            if(bullets[i].active) {
                bullets[i].timer--;
                bullets[i].x = x+150;
                bullets[i].y = y+16;

                if(bullets[i].timer <= 0) {
                    bullets[i].active = false;
                }
            }
        }}
    else {
        for(int i = 0; i < MAX_BULLETS; i++) {
            if(bullets[i].active) {
                bullets[i].active = false;
                }
            }


    }
}


void draw_bulletsLASER(BulletLASER bullets[], int MAX_BULLETS,int WIDTH, float x, float y){
    for(int i = 0; i < MAX_BULLETS; i++) {
        if(bullets[i].active) {
            al_draw_filled_rectangle(
                bullets[i].x,                          // début écran
                bullets[i].y,
                WIDTH,                      // fin écran
                bullets[i].y + 2,           // épaisseur du laser
                al_map_rgb(255, 0, 0)       // rouge laser
            );
        }
    }
}
/*bool laser_hits_enemy(float laser_y, float laser_height, Enemy *e) {
    if (e->state != ESTATE_ALIVE) return false;

    float enemy_top = e->y - e->h / 2;
    float enemy_bottom = e->y + e->h / 2;

    return (enemy_bottom >= laser_y &&
            enemy_top <= laser_y + laser_height);
}*/
bool bullet_active(BulletLASER bullets[], int MAX) {
    for(int i = 0; i < MAX; i++) {
        if(bullets[i].active)
            return true;
    }
    return false;
}
void fire_bullet(Bullet bullets[], float x, float y, int MAX_BULLETS){
    for(int i=0;i<MAX_BULLETS;i++){
        if(!bullets[i].active){
            bullets[i].x=x+150;
            bullets[i].y=y+16;
            bullets[i].active=true;
            break;
        }
    }
}
void update_bullets(Bullet bullets[], int MAX_BULLETS, int BULLET_SPEED, int WIDTH){
    for(int i=0;i<MAX_BULLETS;i++){
        if(bullets[i].active){
            bullets[i].x+=BULLET_SPEED;
            if(bullets[i].x>WIDTH)
                bullets[i].active = false;
        }
    }
}
void draw_bullets(Bullet bullets[], int MAX_BULLETS){
    for(int i=0;i<MAX_BULLETS;i++){
        if(bullets[i].active)
            al_draw_filled_rectangle(bullets[i].x,bullets[i].y,
            bullets[i].x+10,bullets[i].y+3,
            al_map_rgb(255,255,0));
    }
}
void DrawNombreTirsLaser(int tirsLaser, ALLEGRO_BITMAP *tirLaser) {

    int tailleh = 25;
    int tailleL = 60;// taille des icônes
    int espace = 30  ;      // espace entre elles

    for (int i = 0; i < tirsLaser; i++) {
        al_draw_scaled_bitmap(
            tirLaser,
            0, 0,
            al_get_bitmap_width(tirLaser),
            al_get_bitmap_height(tirLaser),
            10 + i * (tailleh+ espace),  // décalage horizontal
            40,                          // en haut
            tailleL,
            tailleh,
            0
        );
    }
}


void fire_bulletSPRAY(BulletSPRAY bullets[], float x, float y, int MAX_BULLETS) {

    for(int i = 0; i < MAX_BULLETS; i++) {
        if(!bullets[i].active) {

            float angle_start = -0.5; // angle début
            float angle_step = 0.1;   // écart entre balles

            for (int j = 0; j < 10; j++) {

                float angle = angle_start + j * angle_step;

                bullets[i].bullets[j].x = x;
                bullets[i].bullets[j].y = y;

                bullets[i].bullets[j].vx = cos(angle) * 8;
                bullets[i].bullets[j].vy = sin(angle) * 8;

                bullets[i].bullets[j].active = true;
            }

            bullets[i].active = true;
            break;
        }
    }
}
void draw_bulletSpray(BulletSPRAY bullets[], int MAX_BULLETS){
    for(int i=0;i<MAX_BULLETS;i++) {
        if(bullets[i].active){
            for (int j = 0; j < 10; j++) {
                al_draw_filled_rectangle(bullets[i].bullets[j].x,bullets[i].bullets[j].y,
                bullets[i].bullets[j].x+10,bullets[i].bullets[j].y+3,
                al_map_rgb(255,255,0));
            }}

    }
}

void update_bulletsSpray(BulletSPRAY bullets[], int MAX_BULLETS, int WIDTH, int HEIGHT, int BULLET_SPEED){
    for(int i = 0; i < MAX_BULLETS; i++){
        if(bullets[i].active){

            for (int j = 0; j < 10; j++) {

                if(bullets[i].bullets[j].active){

                    bullets[i].bullets[j].x += bullets[i].bullets[j].vx+ BULLET_SPEED;;
                    bullets[i].bullets[j].y += bullets[i].bullets[j].vy;;

                    if(bullets[i].bullets[j].x > WIDTH ||
                       bullets[i].bullets[j].y < 0 ||
                       bullets[i].bullets[j].y > HEIGHT) {

                        bullets[i].bullets[j].active = false;
                       }
                }
            }
        }
    }
}

void DrawTempsEntreTir(int temps, ALLEGRO_BITMAP *tirSPRAY, int WIDTH, int HEIGHT, int TEMPSATTENTE) {


    int bar_width = 150;
    int bar_height = 15;

    int x = WIDTH - bar_width - 20; // en haut à droite
    int y = 10;
   /* al_draw_scaled_bitmap(
        tirSPRAY,
        0, 0,
        al_get_bitmap_width(tirSPRAY),
        al_get_bitmap_height(tirSPRAY),
        x - 50,   // 👈 à gauche de la barre
        y,
        20,
        20,
        0
    );*/

    // fond (barre vide)
    al_draw_filled_rectangle(
        x, y,
        x + bar_width, y + bar_height,
        al_map_rgb(50, 50, 50)
    );

    // calcul du remplissage
    float ratio = (float)( TEMPSATTENTE- temps) /TEMPSATTENTE;
    int fill = bar_width * ratio;

    // barre remplie
    al_draw_filled_rectangle(
        x, y,
        x + fill, y + bar_height,
        al_map_rgb(255, 255, 0)
    );

    // contour
    al_draw_rectangle(
        x, y,
        x + bar_width, y + bar_height,
        al_map_rgb(255, 255, 255),
        2
    );
}

void Stalactites_init(Stalactites s[], int MAX_GROTTE, int WIDTH, int HEIGHT)
{
    int largeur = 150;

    float current_gap_y = HEIGHT / 2;

    for (int i = 0; i < MAX_GROTTE; i++) {

        s[i].width = largeur;

        // tunnel continu
        s[i].x = i * largeur;

        // taille passage
        s[i].gap_height = 200;

        // variation douce verticale
        current_gap_y += (rand() % 121) - 60;

        // limites
        if (current_gap_y < 120)
            current_gap_y = 120;

        if (current_gap_y > HEIGHT - 120 - s[i].gap_height)
            current_gap_y = HEIGHT - 120 - s[i].gap_height;

        s[i].gap_y = current_gap_y;
    }
}
void Stalactites_update(Stalactites s[],
                        int MAX_GROTTE,
                        int WIDTH,
                        int HEIGHT) {
    static float vitesse = 4.5f;

    for (int i = 0; i < MAX_GROTTE; i++) {

        s[i].x -= vitesse;

        // sorti écran
        if (s[i].x + s[i].width < 0) {

            float max_x = s[0].x;
            int last = 0;

            // cherche le dernier morceau
            for (int j = 1; j < MAX_GROTTE; j++) {

                if (s[j].x > max_x) {
                    max_x = s[j].x;
                    last = j;
                }
            }

            // replace à la suite
            s[i].x = max_x + s[last].width;


            // variation organique
            if (rand() % 4 == 0)
                s[i].gap_height = 170;
            else
                s[i].gap_height = 220;

            // variation plus douce
            int variation = (rand() % 141) - 70;

            s[i].gap_y = s[last].gap_y + variation;

            // garde le tunnel dans l'écran
            if (s[i].gap_y < 80)
                s[i].gap_y = 80;

            if (s[i].gap_y > HEIGHT - s[i].gap_height - 80)
                s[i].gap_y = HEIGHT - s[i].gap_height - 80;
        }
    }
}
void stalactique_collision(Stalactites s[],
                           int MAX_GROTTE,
                           float *x,
                           float *y,
                           int ship_w,
                           int ship_h,
                           int HEIGHT,
                           int *vie)
{
    static int invincible_timer = 0;

    if (invincible_timer > 0)
        invincible_timer--;

    // hitbox réduite
    float margin_x = 10;
    float margin_y = 10;

    float left   = *x + margin_x;
    float right  = *x + ship_w - margin_x;

    float top    = *y + margin_y;
    float bottom = *y + ship_h - margin_y;

    for (int i = 0; i < MAX_GROTTE; i++) {

        // 🔥 ALIGNEMENT RENDU / COLLISION
        float sx = (int)(s[i].x + 0.5f);
        float sw = (int)(s[i].width + 10);

        if (right > sx &&
            left < sx + sw) {

            // 🔥 MARGE POUR ÉVITER COLLISIONS FANTÔMES
            float top_limit =
                s[i].gap_y +1 ;

            float bottom_limit =
                s[i].gap_y + s[i].gap_height +2 ;

            if (top < top_limit ||
                bottom > bottom_limit) {

                if (invincible_timer == 0) {

                    (*vie)--;

                    // recul léger
                    *x -= 40;

                    if (*x < 50)
                        *x = 50;

                    // recentrage dans le tunnel
                    float center =
                        s[i].gap_y +
                        s[i].gap_height / 2;

                    *y = center - ship_h / 2;

                    // sécurité
                    if (*y < s[i].gap_y + 5)
                        *y = s[i].gap_y + 5;

                    if (*y + ship_h >
                        s[i].gap_y +
                        s[i].gap_height - 5)

                        *y =
                            s[i].gap_y +
                            s[i].gap_height -
                            ship_h - 5;

                    invincible_timer = 60;
                }

                return;
            }
        }
    }
}
void stalactique_render(Stalactites s[],
                        int MAX_GROTTE,
                        int HEIGHT,
                        int WIDTH,
                        ALLEGRO_BITMAP *grotte)
{
    int tile_w = al_get_bitmap_width(grotte);
    int tile_h = al_get_bitmap_height(grotte);

    for (int i = 0; i < MAX_GROTTE; i++) {

        int x = (int)s[i].x +1;
        int largeur = (int)s[i].width +1;

        // plafond
        int top_h = (int)s[i].gap_y;

        al_draw_scaled_bitmap(
            grotte,
            0, 0,
            tile_w, tile_h,
            x,
            0,
            largeur,
            top_h,
            0
        );

        // sol
        int bottom_y = (int)(s[i].gap_y + s[i].gap_height) +1;

        int bottom_h = HEIGHT - bottom_y;

        al_draw_scaled_bitmap(
            grotte,
            0, 0,
            tile_w, tile_h,
            x,
            bottom_y,
            largeur,
            bottom_h,
            ALLEGRO_FLIP_VERTICAL
        );
    }
}
void Stalactites_update_sortie(Stalactites s[], int MAX_GROTTE)
{
    float vitesse = 4.5f;

    for (int i = 0; i < MAX_GROTTE; i++) {
        s[i].x -= vitesse;
    }
}
void spawn_coeur(CoeurVie *c,
                 int WIDTH,
                 Stalactites s[],
                 int MAX_GROTTE)
{
    float max_x = s[0].x;
    int last = 0;

    // cherche le morceau le plus à droite
    for (int i = 1; i < MAX_GROTTE; i++) {

        if (s[i].x > max_x) {
            max_x = s[i].x;
            last = i;
        }
    }

    c->x = WIDTH + 50;

    // spawn dans le passage visible
    c->y = s[last].gap_y + 30 +
           rand() % ((int)s[last].gap_height - 60);

    c->active = true;
}
void update_coeur(CoeurVie *c,int *vies,
                     float ship_x,
                     float ship_y,
                     int ship_w,
                     int ship_h){
    if (!c->active)
        return;

    c->x -= 3;

    if (c->x < -30)
        c->active = false;


    float left   = ship_x;
    float right  = ship_x + ship_w;

    float top    = ship_y;
    float bottom = ship_y + ship_h;

    if (right > c->x &&
        left < c->x + 30 &&
        bottom > c->y &&
        top < c->y + 30)
    {
        if (*vies<3){
        (*vies)++;}

        c->active = false;
    }
}
void draw_coeur(CoeurVie *c,ALLEGRO_BITMAP *vie){
    if (!c->active)
        return;

    al_draw_scaled_bitmap(
        vie,
        0, 0,
        al_get_bitmap_width(vie),
        al_get_bitmap_height(vie),
        c->x,
        c->y,
        30,
        30,
        0
    );
}
void spawn_laser(laserPlus *c,
                 int WIDTH,
                 int HEIGHT)
{

    c->x = WIDTH + 50;


    c->y = rand() % (HEIGHT) -50;

    c->active = true;
}
void update_laser_Recup(laserPlus *c,int *maxBullet,
                     float ship_x,
                     float ship_y,
                     int ship_w,
                     int ship_h){
    if (!c->active)
        return;

    c->x -= 3;

    if (c->x < -30)
        c->active = false;


    float left   = ship_x;
    float right  = ship_x + ship_w;

    float top    = ship_y;
    float bottom = ship_y + ship_h;

    if (right > c->x &&
        left < c->x + 30 &&
        bottom > c->y &&
        top < c->y + 30)
    {
        (*maxBullet)++;

        c->active = false;
    }
}
void draw_laserplus(laserPlus *c,ALLEGRO_BITMAP *laser){
    if (!c->active)
        return;

    al_draw_scaled_bitmap(
        laser,
        0, 0,
        al_get_bitmap_width(laser),
        al_get_bitmap_height(laser),
        c->x,
        c->y,
        60,
        25,
        0
    );
}

void sauvegarder_partie_complete(
    int vies, int score, int niveau,
    float x, float y,

    Bullet bullet[], int nb_bullets,
    EnemyNiv1 ennemis[], int nb_ennemis
) {
    FILE *f = fopen("save.bin", "wb");
    if (!f) return;

    fwrite(&vies, sizeof(int), 1, f);
    fwrite(&score, sizeof(int), 1, f);
    fwrite(&niveau, sizeof(int), 1, f);
    fwrite(&x, sizeof(float), 1, f);
    fwrite(&y, sizeof(float), 1, f);

    fwrite(&nb_bullets, sizeof(int), 1, f);
    fwrite(bullet, sizeof(Bullet), nb_bullets, f);

    fwrite(&nb_ennemis, sizeof(int), 1, f);
    fwrite(ennemis, sizeof(EnemyNiv1), nb_ennemis, f);

    fclose(f);
}
int charger_partie_complete(
    int *vies, int *score, int *niveau,
    float *x, float *y,

    Bullet bullet[], int *nb_bullets,
    EnemyNiv1 ennemis[], int *nb_ennemis
) {
    FILE *f = fopen("save.bin", "rb");
    if (!f) return 0;

    fread(vies, sizeof(int), 1, f);
    fread(score, sizeof(int), 1, f);
    fread(niveau, sizeof(int), 1, f);
    fread(x, sizeof(float), 1, f);
    fread(y, sizeof(float), 1, f);

    fread(nb_bullets, sizeof(int), 1, f);
    fread(bullet, sizeof(Bullet), *nb_bullets, f);

    fread(nb_ennemis, sizeof(int), 1, f);
    fread(ennemis, sizeof(EnemyNiv1), *nb_ennemis, f);

    fclose(f);
    return 1;
}