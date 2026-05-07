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
    enemies_init();


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



    if (!ship || !background || !grotte || !coeur|| !tirLaser || !tempsTir) {
        printf("Erreur chargement ressources\n");
        return -1;
    }
    niv2Grotte(HEIGHT,WIDTH,NOMBREVIE,BULLET_SPEED, SPEED, MaxBullets, TempsAttenteRechargement, SCROLL_SPEED, background,  ship, coeur, tirLaser, tempsTir, timer, queue, display, grotte );


  /*  int ship_w = 150;
    int ship_h = 50;
    int bg_w = al_get_bitmap_width(background);
    int vies = NOMBREVIE;// definit le nombre de vie dans la partie
    int MAX_BULLET_LASER =100;
    int nombreTirLaser=3;
    int tempsEntreLesTires = 0;
    int TempsChargementSpray=0;



    //la grotte
    Grotte g;
    g.x = 0;              // pas utilisé si grotte fixe
    g.gap_y = HEIGHT / 2; // centre du passage
    g.gap_height = 600;   // taille du trou
    g.width =WIDTH;

    Stalactites stalactites[MAX_STALAC];
    Stalactites_init(stalactites, MAX_STALAC, WIDTH, HEIGHT);

    //Position joueur
    float x = 100;
    float y = HEIGHT / 2;

    BulletLASER bullets[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets]={0};
    BulletSPRAY bullet_sp[MaxBullets]={0};


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

    //Boucle principale
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        // Gestion événements
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            running = 0;


        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
            key[ev.keyboard.keycode] = true;

        else if (ev.type == ALLEGRO_EVENT_KEY_UP)
            key[ev.keyboard.keycode] = false;


        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            //Sortie du jeu
            if (key[ALLEGRO_KEY_ESCAPE]) running = 0;
            collision=0;


            if (tempsEntreLesTires>0) {
                tempsEntreLesTires -= 1;
            }
            else {
                fire_bullet(bullet,x,y, MaxBullets);
                tempsEntreLesTires = 10;
            }
            if (TempsChargementSpray>0) {
                TempsChargementSpray -= 1;
            }

            Updatescrolling(&x, &y, &bgx,key,ship_w, ship_h,WIDTH, HEIGHT,SPEED, SCROLL_SPEED,bg_w);
            update_bulletsLASER(bullets,x,y ,MAX_BULLET_LASER,collision);
            update_bullets(bullet,MaxBullets,BULLET_SPEED,WIDTH);
            update_bulletsSpray(bullet_sp,MaxBullets,WIDTH,HEIGHT,BULLET_SPEED);



            redraw = 1;
        }
        if(key[ALLEGRO_KEY_SPACE]&& nombreTirLaser>0 && !bullet_active(bullets, MAX_BULLET_LASER)){
            fire_bulletLASER(bullets, x+40, y+10, MAX_BULLET_LASER);
            nombreTirLaser-=1;
            tempsEntreLesTires = 120;
        }
        if(key[ALLEGRO_KEY_ENTER]&& TempsChargementSpray==0){
                fire_bulletSPRAY(bullet_sp,x,y, MaxBullets);
                TempsChargementSpray = TempsAttenteRechargement;
                tempsEntreLesTires = 120;

        }


        //Rendu
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = 0;
            render(x, y, bgx,ship, background,ship_w, ship_h,WIDTH, HEIGHT,bg_w);

            draw_bullets(bullet,MaxBullets);
            draw_bulletSpray(bullet_sp,MaxBullets);
            enemies_draw();
            enemy_bullets_draw();
            vie(&vies, coeur, &fin);

            DrawNombreTirsLaser(nombreTirLaser,tirLaser);
            draw_bulletsLASER(bullets,MAX_BULLET_LASER, WIDTH,x,y);
           DrawTempsEntreTir(TempsChargementSpray,tempsTir,WIDTH,HEIGHT,TempsAttenteRechargement);

            al_flip_display();

        }
        if (fin==1) {
            running=0;
        }
    }




        /*else if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (key[ALLEGRO_KEY_ESCAPE]) running = 0;
            redraw = 1;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = 0;

            modeboss(&x, &y, ship, background, bgx, key,
                     ship_w, ship_h, WIDTH, HEIGHT);

        }*/


        //Libération mémoire
        al_destroy_bitmap(ship);
        al_destroy_bitmap(background);
        al_destroy_timer(timer);
        al_destroy_event_queue(queue);
        al_destroy_display(display);

        return 0;
    }

