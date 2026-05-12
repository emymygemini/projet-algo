#include <stdio.h>
#include "fonctions.h"
#include "menu.h"
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "boss.h"
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "enemy.h"
#define WIDTH 1100
#define HEIGHT 700
#define FPS 60
#define SPEED 6
#define SCROLL_SPEED 3
#define NOMBREVIE 3
#define BULLET_SPEED 8
#define MaxBullets 200
#define TempsAttenteRechargement 180
#define MAX_STALAC 20
int main() {
    //Initialisation Allegro
    if (!al_init()) return -1;

    al_install_keyboard();
    al_init_image_addon();
    //enemies_init();


    ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);
    ALLEGRO_TIMER *timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    //Chargement ressources
    ALLEGRO_BITMAP *ship = al_load_bitmap("vaisseau.png");
    ALLEGRO_BITMAP *background = al_load_bitmap("fondetoile.jpg");
    ALLEGRO_BITMAP *grotte = al_load_bitmap("grotte.png");
    ALLEGRO_BITMAP *coeur = al_load_bitmap("coeur.png");
    ALLEGRO_BITMAP *tirLaser = al_load_bitmap("tirLaser.png");
    ALLEGRO_BITMAP *tempsTir = al_load_bitmap("Spray.png");
    ALLEGRO_BITMAP *Enemy1 = al_load_bitmap("Rouge.png");



    if (!ship || !background || !grotte || !coeur|| !tirLaser || !tempsTir || !Enemy1) {
        printf("Erreur chargement ressources\n");
        return -1;
    }
    //garder tout ce qui au dessus pour que ça tourne
    // niv2Grotte(HEIGHT,WIDTH,NOMBREVIE,BULLET_SPEED, SPEED, MaxBullets, TempsAttenteRechargement, SCROLL_SPEED, background,  ship, coeur, tirLaser, tempsTir, timer, queue, display, grotte );


    int ship_w = 150;
    int ship_h = 50;
    int bg_w = al_get_bitmap_width(background);
    int vies = NOMBREVIE;// definit le nombre de vie dans la partie
    int MAX_BULLET_LASER =100;
    int nombreTirLaser=3;
    int tempsEntreLesTires = 0;
    int TempsChargementSpray=0;

    CoeurVie coeurBonus;
    coeurBonus.active = false;

    laserPlus laserBonus;
    laserBonus.active = false;




    //la grotte
    Grotte g;
    g.x = 0;              // pas utilisé si grotte fixe
    g.gap_y = HEIGHT / 2; // centre du passage
    g.gap_height = 600;   // taille du trou
    g.width =WIDTH;

    Stalactites stalactites[MAX_STALAC];
    Stalactites_init(stalactites, MAX_STALAC, WIDTH, HEIGHT);
    boss_init(WIDTH, HEIGHT);

    //Position joueur
    float x = 100;
    float y = HEIGHT / 2;

    //les tirs du joueur

    BulletLASER bulletLASER[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets]={};
    BulletSPRAY bullet_sp[MaxBullets]={};

    //les ennemis
    int MAX_ENEMIES1 = 40;
    EnemyNiv1BIS EnemysNIV1[MAX_ENEMIES1];


    //Scrolling
    float bgx = 0;

    //Clavier
    bool key[ALLEGRO_KEY_MAX] = {0};

    int running = 1;
    int redraw = 1;
    int fin=0;
    int collision = 0;

    al_start_timer(timer);
    printf("ship_w = %d\n", ship_w);

    // Affiche le menu et récupère le choix
    int choix = afficher_menu(display, queue, timer, WIDTH, HEIGHT);

    // Lance le niveau correspondant
    switch (choix) {
        case MENU_NIV1:
            niv1(HEIGHT, WIDTH, NOMBREVIE, BULLET_SPEED, SPEED, MaxBullets,
                 TempsAttenteRechargement, SCROLL_SPEED, background, ship,
                 coeur, tirLaser, tempsTir, timer, queue, display);
            break;
        case MENU_NIV2:
            niv2(HEIGHT, WIDTH, NOMBREVIE, BULLET_SPEED, SPEED, MaxBullets,
                 TempsAttenteRechargement, SCROLL_SPEED, background, ship,
                 coeur, tirLaser, tempsTir, timer, queue, display, grotte);
            break;
        case MENU_NIV3:
            niv3(HEIGHT, WIDTH, NOMBREVIE, BULLET_SPEED, SPEED, MaxBullets,
                 TempsAttenteRechargement, SCROLL_SPEED, background, ship,
                 coeur, tirLaser, tempsTir, timer, queue, display, grotte);
            break;
        case MENU_BOSS:
            nivBOSS(HEIGHT, WIDTH, NOMBREVIE, BULLET_SPEED, SPEED, MaxBullets,
                    TempsAttenteRechargement, SCROLL_SPEED, background, ship,
                    coeur, tirLaser, tempsTir, timer, queue, display, grotte);
            break;
        case MENU_QUITTER:
        default:
            break;
    }

    //Libération mémoire;
    al_destroy_bitmap(ship);
    al_destroy_bitmap(background);
    al_destroy_bitmap(grotte);
    al_destroy_bitmap(coeur);
    al_destroy_bitmap(tirLaser);
    al_destroy_bitmap(tempsTir);
    al_destroy_bitmap(Enemy1);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);

    return 0;
}

