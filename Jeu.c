//
// Created by helen on 06/05/2026.
//
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

void niv1(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display ) {
    int ship_w = 150;
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

    //Position joueur
    float x = 100;
    float y = HEIGHT / 2;

    BulletLASER bullets[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets]={};
    BulletSPRAY bullet_sp[MaxBullets]={};


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
    al_destroy_bitmap(ship);
    al_destroy_bitmap(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}
void niv3(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  ) {
    int ship_w = 150;
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
    g.gap_height = 500;   // taille du trou
    g.width =WIDTH;

    //Position joueur
    float x = 100;
    float y = HEIGHT / 2;

    BulletLASER bullets[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets]={};
    BulletSPRAY bullet_sp[MaxBullets]={};


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
            grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);
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
            grotte_render(g, HEIGHT, WIDTH, grotte);
            draw_bulletSpray(bullet_sp,MaxBullets);
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
    al_destroy_bitmap(ship);
    al_destroy_bitmap(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}
void niv2(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  ) {
    int ship_w = 150;
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

    //Position joueur
    float x = 100;
    float y = HEIGHT / 2;

    BulletLASER bullets[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets]={};
    BulletSPRAY bullet_sp[MaxBullets]={};


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
            grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);
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
            grotte_render(g, HEIGHT, WIDTH, grotte);
            draw_bulletSpray(bullet_sp,MaxBullets);
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
    al_destroy_bitmap(ship);
    al_destroy_bitmap(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}
void niv2Grotte(int HEIGHT, int WIDTH ,int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  ) {
    int ship_w = 150;
    int ship_h = 50;
    int bg_w = al_get_bitmap_width(background);
    int vies = NOMBREVIE;// definit le nombre de vie dans la partie
    int MAX_BULLET_LASER =100;
    int nombreTirLaser=3;
    int tempsEntreLesTires = 0;
    int TempsChargementSpray=0;
    int MAX_STALAC = 20;





    //la grotte
    Grotte g;
    g.x = 0;              // pas utilisé si grotte fixe
    g.gap_y = HEIGHT / 2; // centre du passage
    g.gap_height = 600;   // taille du trou
    g.width =WIDTH;

    //Position joueur
    float x = 100;
    float y = HEIGHT / 2;

    BulletLASER bullets[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets]={};
    BulletSPRAY bullet_sp[MaxBullets]={};
    Stalactites stalactites[MAX_STALAC];
    Stalactites_init(stalactites, MAX_STALAC, WIDTH, HEIGHT);


    //Scrolling
    float bgx = 0;

    //Clavier
    bool key[ALLEGRO_KEY_MAX] = {0};

    int running = 1;
    int redraw = 1;
    int fin=0;
    int collision = 0;
    int tempsNiveau= 1200;

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
            if (tempsNiveau>0) {
                tempsNiveau -= 1;
            }else {
                running = 0;
            }
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
            Stalactites_update(stalactites, MAX_STALAC, WIDTH, HEIGHT);
            stalactique_collision(stalactites, MAX_STALAC, &x, &y, ship_w, ship_h, HEIGHT, &vies);
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
            stalactique_render(stalactites, MAX_STALAC, HEIGHT, WIDTH, grotte);
            draw_bulletSpray(bullet_sp,MaxBullets);
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
    al_destroy_bitmap(ship);
    al_destroy_bitmap(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}
void nivBOSS(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  ) {
    int ship_w = 150;
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

    //Position joueur
    float x = 100;
    float y = HEIGHT / 2;

    BulletLASER bullets[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets]={};
    BulletSPRAY bullet_sp[MaxBullets]={};


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

            modeboss(&x, &y, ship, background, bgx, key,
                    ship_w, ship_h, WIDTH, HEIGHT);
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
    al_destroy_bitmap(ship);
    al_destroy_bitmap(background);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    al_destroy_display(display);
}