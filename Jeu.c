//
// Created by helen on 06/05/2026.
//
#include <stdio.h>
#include "fonctions.h"
#include "boss.h"
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>

#include "enemy.h"
#include "menu.h"

int niv1(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2) {

    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);
    ALLEGRO_SAMPLE *son_laser = al_load_sample("laser.wav");
    ALLEGRO_SAMPLE *son_explosion = al_load_sample("Mort.wav");
    ALLEGRO_SAMPLE *son_hit = al_load_sample("Vie.wav");
    ALLEGRO_SAMPLE *son_tir = al_load_sample("tir.wav");

    if (!son_tir || !son_explosion || !son_hit || !son_laser) {
        printf("Erreur chargement sons\n");
    }
    ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);
    ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 40, 0);
    ALLEGRO_FONT *fontpetit = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);

    GameState state = STATE_GAME;
    bool p_pressed = false;

    int MAX_STALAC=0;

    int ship_w = 150;
    int ship_h = 50;
    int bg_w = al_get_bitmap_width(background);

    int vies = NOMBREVIE;
    int MAX_BULLET_LASER = 100;
    int nombreTirLaser = 3;
    int tempsEntreLesTires = 0;
    int TempsChargementSpray = 0;

    CoeurVie coeurBonus;
    coeurBonus.active = false;

    laserPlus laserBonus;
    laserBonus.active = false;

    Grotte g;
    g.x = 0;
    g.gap_y = HEIGHT / 2;
    g.gap_height = 600;
    g.width = WIDTH;

    Stalactites stalactites[MAX_STALAC];
    Stalactites_init(stalactites, MAX_STALAC, WIDTH, HEIGHT);

    float x = 100;
    float y = HEIGHT / 2;

    BulletLASER bulletLASER[MAX_BULLET_LASER] = {};
    Bullet bullet[MaxBullets] = {};
    BulletSPRAY bullet_sp[MaxBullets] = {};

    GameStats stats;

    stats.score = 0;
    stats.enemiesKilled = 0;
    stats.enemiesMissed = 0;

    int MAX_ENEMIES1 = 10;
    int MAX_ENEMIES2 = 2;
    int victoire=0;

    EnemyNiv1 EnemysNIV1[MAX_ENEMIES1];
    EnemyNiv1BIS EnemysNIV1BIS[MAX_ENEMIES1];
    EnemyNiv2 EnemysNIV2[MAX_ENEMIES2];

    memset(EnemysNIV1, 0, sizeof(EnemysNIV1));
    memset(EnemysNIV2, 0, sizeof(EnemysNIV2));

    memset(EnemysNIV1BIS, 0, sizeof(EnemysNIV1BIS));


    float bgx = 0;
    bool key[ALLEGRO_KEY_MAX] = {0};

    int running = 1;
    int redraw = 1;
    int fin = 0;
    int collision = 0;
    int NombreEnnemi =30;
    int NombreEnnemi1 =20;
    int niveau = 1;
    int tempsattentefinnieveau =700;

    al_start_timer(timer);

    while (running) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        // ================= INPUT =================
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            running=0;
            return 0;
        }


         if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            key[ev.keyboard.keycode] = true;
            if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                running=0;
                fin = 2;
            }

            if (ev.keyboard.keycode == ALLEGRO_KEY_P && !p_pressed) {
                p_pressed = true;

                if (state == STATE_GAME)
                    state = STATE_PAUSE;
                else if (state == STATE_PAUSE)
                    state = STATE_GAME;
            }
             if (key[ALLEGRO_KEY_S]) {
                 sauvegarder_partie_complete(
                     vies, stats.score, niveau,
                     x, y,
                     bullet, MaxBullets,
                     EnemysNIV1, MAX_ENEMIES1
                 );
             }
        }else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            key[ev.keyboard.keycode] = false;

            if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                p_pressed = false;
        }

        // ================= UPDATE =================
        else if (ev.type == ALLEGRO_EVENT_TIMER) {

            if (state == STATE_GAME) {

                if (key[ALLEGRO_KEY_ESCAPE]) running = 0;

                collision = 0;

                int progression = stats.enemiesKilled;

                if (niveau == 1)
                {
                    if (progression < NombreEnnemi)
                    {
                        if (rand() % 10 == 0)
                        {
                            spawn_NIV_ENEMY1(EnemysNIV1, MAX_ENEMIES1, WIDTH, HEIGHT);
                        }
                    }
                    else
                    {
                        // PASSAGE NIVEAU 2
                        niveau = 2;

                        // reset progression
                        stats.enemiesKilled = 0;

                    }
                }

                else if (niveau == 2)
                {
                    if (progression < NombreEnnemi1)
                    {
                        if (rand() % 150 == 0)
                        {
                            spawn_NIV_ENEMY2(EnemysNIV2, MAX_ENEMIES2, WIDTH, HEIGHT);
                        }
                        if (rand() % 60 == 0)
                        {

                            spawn_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES1, WIDTH, HEIGHT);
                        }
                        if (rand() % 3000 == 0 || !coeurBonus.active)
                        {
                            spawn_coeur(&coeurBonus, WIDTH,stalactites, MAX_STALAC);
                        }

                    }
                    else
                    {
                        // PASSAGE NIVEAU 2
                        niveau = 3;
                    }
                }
                else if (niveau == 3 && tempsattentefinnieveau !=0 ) {
                    tempsattentefinnieveau --;

                }
                else
                {
                    running = 0;
                    victoire=1;
                }



                if (tempsEntreLesTires > 0)
                    tempsEntreLesTires--;
                else {
                    fire_bullet(bullet, x, y, MaxBullets);
                    tempsEntreLesTires = 17;
                }

                if (TempsChargementSpray > 0)
                    TempsChargementSpray--;



                Updatescrolling(&x, &y, &bgx, key, ship_w, ship_h, WIDTH, HEIGHT, SPEED, SCROLL_SPEED, bg_w);

                update_bulletsLASER(bulletLASER, x, y, MAX_BULLET_LASER, collision);
                update_bullets(bullet, MaxBullets, BULLET_SPEED, WIDTH);
                update_bulletsSpray(bullet_sp, MaxBullets, WIDTH, HEIGHT, BULLET_SPEED);
                update_coeur(&coeurBonus,&vies,x,y,ship_w,ship_h);
                update_NIV_ENEMY2(EnemysNIV2,MAX_ENEMIES2,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp,WIDTH,HEIGHT);
                update_NIV_ENEMY1(EnemysNIV1,MAX_ENEMIES1,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp);
                count_enemy1_stats(EnemysNIV1,MAX_ENEMIES1,&stats,son_explosion);
                count_enemy2_stats(EnemysNIV2,MAX_ENEMIES2,&stats,son_explosion);
                update_NIV_ENEMY1BIS(EnemysNIV1BIS,MAX_ENEMIES1,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp);
                count_enemy1BIS_stats(EnemysNIV1BIS,MAX_ENEMIES1,&stats,son_explosion);
                detecter_perte_vie(vies, son_hit);
                redraw = 1;

                if (key[ALLEGRO_KEY_SPACE] && nombreTirLaser > 0 &&
                    !bullet_active(bulletLASER, MAX_BULLET_LASER)) {
                    fire_bulletLASER(bulletLASER, x+40, y+10, MAX_BULLET_LASER);
                   jouer_son(son_laser);
                    nombreTirLaser--;
                    tempsEntreLesTires = 120;
                    }

                if (key[ALLEGRO_KEY_ENTER] && TempsChargementSpray == 0) {
                    fire_bulletSPRAY(bullet_sp, x, y, MaxBullets);
                   jouer_son(son_tir);
                    TempsChargementSpray = TempsAttenteRechargement;
                }
                if (NOMBREVIE>3){ if (vies<NOMBREVIE) { vies++; }}
            }
        }

        // ================= RENDER =================
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = 0;
            al_clear_to_color(al_map_rgb(0, 0, 0));

            if (state == STATE_GAME) {


                render(x, y, bgx, ship, background, ship_w, ship_h, WIDTH, HEIGHT, bg_w);

                draw_bullets(bullet, MaxBullets);
                draw_bulletSpray(bullet_sp, MaxBullets);
                draw_coeur(&coeurBonus,coeur);
                render_NIV_ENEMY2(EnemysNIV2, MAX_ENEMIES2, Enemy2);
                render_NIV_ENEMY1(EnemysNIV1, MAX_ENEMIES1, Enemy1);
                render_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES1, Enemy1);
                draw_bulletsLASER(bulletLASER, MAX_BULLET_LASER, WIDTH, x, y);
                DrawNombreTirsLaser(nombreTirLaser, tirLaser);
                DrawTempsEntreTir(TempsChargementSpray, tempsTir, WIDTH, HEIGHT, TempsAttenteRechargement);
                vie(&vies, coeur, &fin);
            }
            if (fin==1) {
                running=0;
                victoire=0;
            }

        }
        else if (state == STATE_PAUSE) {
            al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
            al_map_rgba(0,0,0,150));

            al_draw_text(font,
           al_map_rgb(255,255,255),
            WIDTH/2,
              HEIGHT/2,
              ALLEGRO_ALIGN_CENTER,
               "PAUSE - P pour reprendre ");
            al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                "MENU - Menu");
            al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                "S - Sauvegarde");
        }

        al_flip_display();
    }


    running = 1;
    if (fin==2) {
        running=0;
    }



    al_start_timer(timer);

    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        // ================= INPUT =================
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            running = 0;

        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            key[ev.keyboard.keycode] = true;
            if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                running=0;
            }//je

        }

        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            key[ev.keyboard.keycode] = false;

            if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                p_pressed = false;
        }

        // ================= UPDATE =================
        else if (ev.type == ALLEGRO_EVENT_TIMER) {

            if (state == STATE_GAME) {
                if (key[ALLEGRO_KEY_ESCAPE]) running = 0;
                redraw = 1;
            }



            // ================= RENDER =================
            if (redraw && al_is_event_queue_empty(queue)) {
                redraw = 0;
                al_clear_to_color(al_map_rgb(0, 0, 0));



                if (fin==1) {
                    al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                    al_draw_text(fontGROS,
                   al_map_rgb(255,0,0),
                    WIDTH/2,
                      HEIGHT/2,
                      ALLEGRO_ALIGN_CENTER,
                       "GAME OVER");
                    al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                        "MENU - M pour Menu");

                }
                else {
                    al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                    al_draw_text(fontGROS,
                   al_map_rgb(255,255,255),
                    WIDTH/2  ,
                      HEIGHT/2 -40,
                      ALLEGRO_ALIGN_CENTER,
                       "LEVEL BEAT");
                    al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                       "Vous avez tuer %d ennemis et rater %d ennemis",(NombreEnnemi+NombreEnnemi1),stats.enemiesMissed);
                    al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                      "Vous avez obtenu le score de %d",stats.score);
                    al_draw_text(fontpetit,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 200, ALLEGRO_ALIGN_CENTER,
                        "M pour Menu ou Niv suivant");
                }
            }

            al_flip_display();
        }
    }
    return victoire;
}

int niv3(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);
    ALLEGRO_SAMPLE *son_laser = al_load_sample("laser.wav");
    ALLEGRO_SAMPLE *son_explosion = al_load_sample("Mort.wav");
    ALLEGRO_SAMPLE *son_hit = al_load_sample("Vie.wav");
    ALLEGRO_SAMPLE *son_tir = al_load_sample("tir.wav");

    if (!son_tir || !son_explosion || !son_hit || !son_laser) {
        printf("Erreur chargement sons\n");
    }

        ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);
        ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 40, 0);
    ALLEGRO_FONT *fontpetit = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);

        GameState state = STATE_GAME;
        bool p_pressed = false;

        int MAX_STALAC=0;

        int ship_w = 150;
        int ship_h = 50;
        int bg_w = al_get_bitmap_width(background);

        int vies = NOMBREVIE;
        int MAX_BULLET_LASER = 100;
        int nombreTirLaser = 3;
        int tempsEntreLesTires = 0;
        int TempsChargementSpray = 0;
    int victoire=0;

        CoeurVie coeurBonus;
        coeurBonus.active = false;

        laserPlus laserBonus;
        laserBonus.active = false;

        Grotte g;
        g.x = 0;
        g.gap_y = HEIGHT / 2;
        g.gap_height = 600;
        g.width = WIDTH;

        Stalactites stalactites[MAX_STALAC];
        Stalactites_init(stalactites, MAX_STALAC, WIDTH, HEIGHT);
        boss_init(WIDTH, HEIGHT);

        float x = 100;
        float y = HEIGHT / 2;

        BulletLASER bulletLASER[MAX_BULLET_LASER] = {};
        Bullet bullet[MaxBullets] = {};
        BulletSPRAY bullet_sp[MaxBullets] = {};

        GameStats stats;

        stats.score = 0;
        stats.enemiesKilled = 0;
        stats.enemiesMissed = 0;

        int MAX_ENEMIES1 = 10;
        int MAX_ENEMIES2 = 4;
        int MAX_ENEMIES3 = 2;

        EnemyNiv1 EnemysNIV1[MAX_ENEMIES1];
        EnemyNiv1BIS EnemysNIV1BIS[MAX_ENEMIES1];
        EnemyNiv2 EnemysNIV2[MAX_ENEMIES2];
        EnemyNiv2BIS EnemysNIV2BIS[MAX_ENEMIES2];
        EnemyNiv3 EnemysNIV3[MAX_ENEMIES3];
        memset(EnemysNIV1, 0, sizeof(EnemysNIV1));
        memset(EnemysNIV2, 0, sizeof(EnemysNIV2));
        memset(EnemysNIV3, 0, sizeof(EnemysNIV3));
        memset(EnemysNIV1BIS, 0, sizeof(EnemysNIV1BIS));
        memset(EnemysNIV2BIS, 0, sizeof(EnemysNIV2BIS));

        float bgx = 0;
        bool key[ALLEGRO_KEY_MAX] = {0};

        int running = 1;
        int redraw = 1;
        int fin = 0;
        int collision = 0;
        int NombreEnnemi =15;
        int NombreEnnemi1 =20;
        int NombreEnnemi3 = 30;
        int niveau = 1;
        int tempsattentefinnieveau =700;

        al_start_timer(timer);

        while (running) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(queue, &ev);

            // ================= INPUT =================
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                running = 0;
                return 0 ;

            }

            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                key[ev.keyboard.keycode] = true;
                if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                    running=0;
                    fin = 2;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_P && !p_pressed) {
                    p_pressed = true;

                    if (state == STATE_GAME)
                        state = STATE_PAUSE;
                    else if (state == STATE_PAUSE)
                        state = STATE_GAME;
                }
            }
            if (key[ALLEGRO_KEY_S]) {
                sauvegarder_partie_complete(
                    vies, stats.score, niveau,
                    x, y,
                    bullet, MaxBullets,
                    EnemysNIV1, MAX_ENEMIES1
                );
            }

            else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                key[ev.keyboard.keycode] = false;

                if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                    p_pressed = false;
            }

            // ================= UPDATE =================
            else if (ev.type == ALLEGRO_EVENT_TIMER) {

                if (state == STATE_GAME) {

                    if (key[ALLEGRO_KEY_ESCAPE]) running = 0;

                    collision = 0;

                    int progression = stats.enemiesKilled;

                    if (niveau == 1)
                    {
                        if (progression < NombreEnnemi)
                        {
                            if (rand() % 300 == 0)
                            {
                                spawn_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES2, WIDTH, HEIGHT);
                            }
                            if (rand() % 350 == 0)
                            {
                                spawn_NIV_ENEMY2(EnemysNIV2, MAX_ENEMIES2, WIDTH, HEIGHT);
                            }
                        }
                        else
                        {
                            // PASSAGE NIVEAU 2
                            niveau = 2;

                            // reset progression
                            stats.enemiesKilled = 0;

                        }
                    }
                    else if (niveau == 2){
                        if (progression < NombreEnnemi1)
                        {
                            if (rand() % 250 == 0)
                            {
                                spawn_NIV_ENEMY2BIS(EnemysNIV2BIS, MAX_ENEMIES3, WIDTH, HEIGHT);
                            }
                            if (rand() % 200 == 0)
                            {
                                spawn_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES3, WIDTH, HEIGHT);
                            }
                            if (rand() % 2000 == 0 && !coeurBonus.active)
                            {
                                spawn_coeur(&coeurBonus, WIDTH,stalactites, MAX_STALAC);
                            }
                            if (rand() % 2000 == 0 && !laserBonus.active)
                            {
                                spawn_laser(&laserBonus, WIDTH,HEIGHT);
                            }
                            if (rand() % 1000 == 0)
                            {
                                spawn_NIV_ENEMY3(EnemysNIV3, MAX_ENEMIES3, WIDTH, HEIGHT);
                            }

                        }
                        else
                        {
                            // PASSAGE NIVEAU 2
                            niveau = 3;
                        }
                    } else if (niveau == 3){
                        if (progression < NombreEnnemi3)
                        {
                            if (rand() % 400 == 0)
                            {
                                spawn_NIV_ENEMY2BIS(EnemysNIV2BIS, MAX_ENEMIES2, WIDTH, HEIGHT);

                            }
                            if (rand() % 500 == 0)
                            {

                                spawn_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES2, WIDTH, HEIGHT);

                            }
                            if (rand() % 500 == 0)
                            {
                                spawn_NIV_ENEMY3(EnemysNIV3, MAX_ENEMIES2, WIDTH, HEIGHT);
                            }
                            if (rand() % 2000 == 0 && !laserBonus.active)
                            {
                                spawn_laser(&laserBonus, WIDTH,HEIGHT);

                            }
                            if (rand() % 2000 == 0 && !coeurBonus.active)
                            {
                                spawn_coeur(&coeurBonus, WIDTH,stalactites, MAX_STALAC);

                            }
                            if (rand() % 500 == 0)
                            {
                                spawn_NIV_ENEMY1(EnemysNIV1, MAX_ENEMIES3, WIDTH, HEIGHT);
                            }
                        }
                        else
                        {
                            // PASSAGE NIVEAU 2
                            niveau = 4;

                            // reset progression
                            stats.enemiesKilled = 0;
                        }
                    }
                    else if (niveau == 4 && tempsattentefinnieveau !=0 ) {
                        tempsattentefinnieveau --;

                    }
                    else
                    {
                        running = 0;
                        victoire=1;
                    }



                    if (tempsEntreLesTires > 0)
                        tempsEntreLesTires--;
                    else {
                        fire_bullet(bullet, x, y, MaxBullets);
                        tempsEntreLesTires = 17;
                    }

                    if (TempsChargementSpray > 0)
                        TempsChargementSpray--;

                    Updatescrolling(&x, &y, &bgx, key, ship_w, ship_h, WIDTH, HEIGHT, SPEED, SCROLL_SPEED, bg_w);

                    update_bulletsLASER(bulletLASER, x, y, MAX_BULLET_LASER, collision);
                    update_bullets(bullet, MaxBullets, BULLET_SPEED, WIDTH);
                    update_bulletsSpray(bullet_sp, MaxBullets, WIDTH, HEIGHT, BULLET_SPEED);
                    update_coeur(&coeurBonus,&vies,x,y,ship_w,ship_h);

                    grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);




                    update_NIV_ENEMY3(EnemysNIV3,MAX_ENEMIES3,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp,WIDTH,HEIGHT);
                    update_NIV_ENEMY2BIS(EnemysNIV2BIS,MAX_ENEMIES2,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp,WIDTH,HEIGHT);
                    update_NIV_ENEMY2(EnemysNIV2,MAX_ENEMIES2,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp,WIDTH,HEIGHT);
                    update_NIV_ENEMY1(EnemysNIV1,MAX_ENEMIES1,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp);
                    count_enemy1_stats(EnemysNIV1,MAX_ENEMIES1,&stats,son_explosion);
                    count_enemy2_stats(EnemysNIV2,MAX_ENEMIES2,&stats,son_explosion);
                    count_enemy3_stats(EnemysNIV3,MAX_ENEMIES3,&stats,son_explosion);
                    count_enemy2BIS_stats(EnemysNIV2BIS,MAX_ENEMIES2,&stats,son_explosion);
                    update_NIV_ENEMY1BIS(EnemysNIV1BIS,MAX_ENEMIES1,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp);
                    count_enemy1BIS_stats(EnemysNIV1BIS,MAX_ENEMIES1,&stats,son_explosion);
                    update_laser_Recup(&laserBonus,&vies,x,y,ship_w,ship_h);
                    detecter_perte_vie(vies, son_hit);
                    redraw = 1;

                    if (key[ALLEGRO_KEY_SPACE] && nombreTirLaser > 0 &&
                        !bullet_active(bulletLASER, MAX_BULLET_LASER)) {
                        fire_bulletLASER(bulletLASER, x+40, y+10, MAX_BULLET_LASER);
                       jouer_son(son_laser);
                        nombreTirLaser--;
                        tempsEntreLesTires = 120;
                        }

                    if (key[ALLEGRO_KEY_ENTER] && TempsChargementSpray == 0) {
                        fire_bulletSPRAY(bullet_sp, x, y, MaxBullets);
                       jouer_son(son_tir);
                        TempsChargementSpray = TempsAttenteRechargement;
                    }
                    if (NOMBREVIE>3){ if (vies<NOMBREVIE) { vies++; }}
                }
            }

            // ================= RENDER =================
            if (redraw && al_is_event_queue_empty(queue)) {
                redraw = 0;
                al_clear_to_color(al_map_rgb(0, 0, 0));

                if (state == STATE_GAME) {


                    render(x, y, bgx, ship, background, ship_w, ship_h, WIDTH, HEIGHT, bg_w);

                    draw_bullets(bullet, MaxBullets);
                    draw_bulletSpray(bullet_sp, MaxBullets);
                    draw_coeur(&coeurBonus,coeur);
                    draw_laserplus(&laserBonus,tirLaser);

                    render_NIV_ENEMY3(EnemysNIV3, MAX_ENEMIES3, Enemy3, x, y, ship_w, ship_h, &vies);
                    render_NIV_ENEMY2BIS(EnemysNIV2BIS, MAX_ENEMIES2, Enemy2);
                    render_NIV_ENEMY2(EnemysNIV2, MAX_ENEMIES2, Enemy2);
                    render_NIV_ENEMY1(EnemysNIV1, MAX_ENEMIES1, Enemy1);
                    render_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES1, Enemy1);


                    grotte_render(g, HEIGHT, WIDTH, grotte);

                    draw_bulletsLASER(bulletLASER, MAX_BULLET_LASER, WIDTH, x, y);

                    DrawNombreTirsLaser(nombreTirLaser, tirLaser);
                    DrawTempsEntreTir(TempsChargementSpray, tempsTir, WIDTH, HEIGHT, TempsAttenteRechargement);
                    vie(&vies, coeur, &fin);
                }
                if (fin==1) {
                    running=0;
                    victoire=0;
                }
            }
            else if (state == STATE_PAUSE) {



                al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                al_map_rgba(0,0,0,150));

                al_draw_text(font,
               al_map_rgb(255,255,255),
                WIDTH/2,
                  HEIGHT/2,
                  ALLEGRO_ALIGN_CENTER,
                   "PAUSE - P pour reprendre ");
                al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                    "MENU - Menu");
                al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
              "S - Sauvegarde");

            }
            al_flip_display();
        }


        running = 1;
        if (fin==2) {
            running=0;
        }

        al_start_timer(timer);

        while (running) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(queue, &ev);

            // ================= INPUT =================
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                running = 0;

            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                key[ev.keyboard.keycode] = true;
                if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                    running = 0;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_P && !p_pressed) {
                    p_pressed = true;

                    if (state == STATE_GAME)
                        state = STATE_PAUSE;
                    else if (state == STATE_PAUSE)
                        state = STATE_GAME;
                }
            }

            else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                key[ev.keyboard.keycode] = false;

                if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                    p_pressed = false;
            }

            // ================= UPDATE =================
            else if (ev.type == ALLEGRO_EVENT_TIMER) {

                if (state == STATE_GAME) {
                    if (key[ALLEGRO_KEY_ESCAPE]) running = 0;
                    redraw = 1;
                }



                // ================= RENDER =================
                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                    if (fin==1) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                       al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(255,0,0),
                        WIDTH/2,
                          HEIGHT/2,
                          ALLEGRO_ALIGN_CENTER,
                           "GAME OVER");
                        al_draw_text(fontpetit,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                            "MENU - M pour Menu");

                    }
                    else {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                       al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(255,255,255),
                        WIDTH/2  ,
                          HEIGHT/2 -40,
                          ALLEGRO_ALIGN_CENTER,
                           "LEVEL BEAT");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                           "Vous avez tuer %d ennemis et rater %d ennemis",(NombreEnnemi+NombreEnnemi1+NombreEnnemi3),stats.enemiesMissed);
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                      "Vous avez obtenu le score de %d",stats.score);
                        al_draw_text(fontpetit,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 200, ALLEGRO_ALIGN_CENTER,
                            "M pour Menu ou Niv suivant");

                    }
                }

                al_flip_display();
            }
        }
    return victoire;

    }

    int niv2(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);
    ALLEGRO_SAMPLE *son_laser = al_load_sample("laser.wav");
    ALLEGRO_SAMPLE *son_explosion = al_load_sample("Mort.wav");
    ALLEGRO_SAMPLE *son_hit = al_load_sample("Vie.wav");
    ALLEGRO_SAMPLE *son_tir = al_load_sample("tir.wav");

    if (!son_tir || !son_explosion || !son_hit || !son_laser) {
        printf("Erreur chargement sons\n");
    }
        ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);
        ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 40, 0);
    ALLEGRO_FONT *fontpetit = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);


        GameState state = STATE_GAME;
        bool p_pressed = false;

        int MAX_STALAC= 20;

        int ship_w = 150;
        int ship_h = 50;
        int bg_w = al_get_bitmap_width(background);

        int vies = NOMBREVIE;
        int MAX_BULLET_LASER = 100;
        int nombreTirLaser = 3;
        int tempsEntreLesTires = 0;
        int TempsChargementSpray = 0;

        CoeurVie coeurBonus;
        coeurBonus.active = false;

        laserPlus laserBonus;
        laserBonus.active = false;

        Grotte g;
        g.x = 0;
        g.gap_y = HEIGHT / 2;
        g.gap_height = 600;
        g.width = WIDTH;

        int phaseStalac = 0;

        Stalactites stalactites[MAX_STALAC];
        Stalactites_init(stalactites, MAX_STALAC, WIDTH, HEIGHT);
        for (int i = 0; i < MAX_STALAC; i++) {
            stalactites[i].x += WIDTH; // tout décalé à droite
        }
        boss_init(WIDTH, HEIGHT);

        float x = 100;
        float y = HEIGHT / 2;

        BulletLASER bulletLASER[MAX_BULLET_LASER] = {};
        Bullet bullet[MaxBullets] = {};
        BulletSPRAY bullet_sp[MaxBullets] = {};

        GameStats stats;

        stats.score = 0;
        stats.enemiesKilled = 0;
        stats.enemiesMissed = 0;

        int MAX_ENEMIES1 = 10;
        int MAX_ENEMIES2 = 4;
        int MAX_ENEMIES3 = 2;

        EnemyNiv1 EnemysNIV1[MAX_ENEMIES1];
        EnemyNiv1BIS EnemysNIV1BIS[MAX_ENEMIES1];
        EnemyNiv2 EnemysNIV2[MAX_ENEMIES2];
        EnemyNiv2BIS EnemysNIV2BIS[MAX_ENEMIES2];
        EnemyNiv3 EnemysNIV3[MAX_ENEMIES3];
        memset(EnemysNIV1, 0, sizeof(EnemysNIV1));
        memset(EnemysNIV2, 0, sizeof(EnemysNIV2));
        memset(EnemysNIV3, 0, sizeof(EnemysNIV3));
        memset(EnemysNIV1BIS, 0, sizeof(EnemysNIV1BIS));
        memset(EnemysNIV2BIS, 0, sizeof(EnemysNIV2BIS));

        float bgx = 0;
        bool key[ALLEGRO_KEY_MAX] = {0};
    int victoire = 0;

        int running = 1;
        int redraw = 1;
        int fin = 0;
        int collision = 0;
        int NombreEnnemi =15;
        int NombreEnnemi1 =0;
        int NombreEnnemi3 = 30;
        int niveau = 1;
        int tempsattentefinnieveau =700;
        int tempsNiveau= 0;
        int tempsGlobal = 0;
        al_start_timer(timer);

        while (running) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(queue, &ev);

            // ================= INPUT =================
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                running = 0;
                return 0;

            }

            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                key[ev.keyboard.keycode] = true;
                if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                    running=0;
                    fin = 2;
                }
                if (key[ALLEGRO_KEY_S]) {
                    sauvegarder_partie_complete(
                        vies, stats.score, niveau,
                        x, y,
                        bullet, MaxBullets,
                        EnemysNIV1, MAX_ENEMIES1
                    );
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_P && !p_pressed) {
                    p_pressed = true;

                    if (state == STATE_GAME)
                        state = STATE_PAUSE;
                    else if (state == STATE_PAUSE)
                        state = STATE_GAME;
                }
            }

            else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                key[ev.keyboard.keycode] = false;

                if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                    p_pressed = false;
            }

            // ================= UPDATE =================
            else if (ev.type == ALLEGRO_EVENT_TIMER) {

                if (state == STATE_GAME) {

                    if (key[ALLEGRO_KEY_ESCAPE]) running = 0;

                    int progression = stats.enemiesKilled;

                    if (niveau == 1)
                    {
                        if (progression < NombreEnnemi)
                        {grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);
                            if (rand() % 300 == 0)
                            {
                                spawn_NIV_ENEMY1(EnemysNIV1, MAX_ENEMIES2, WIDTH, HEIGHT);
                            }
                            if (rand() % 350 == 0)
                            {
                                spawn_NIV_ENEMY2BIS(EnemysNIV2BIS, 1, WIDTH, HEIGHT);
                            }
                            if (rand() % 650 == 0)
                            {
                                spawn_NIV_ENEMY2BIS(EnemysNIV2BIS, 1, WIDTH, HEIGHT);
                            }
                            if (progression == NombreEnnemi-3) {
                                spawn_NIV_ENEMY3(EnemysNIV3, 1, WIDTH, HEIGHT);
                            }
                            if (rand() % 2000 == 0 && !coeurBonus.active)
                            {
                                spawn_coeur(&coeurBonus, WIDTH,stalactites, MAX_STALAC);

                            }
                        }
                        else
                        {
                            // PASSAGE NIVEAU 2
                            niveau = 2;
                            phaseStalac = 1;

                            // reset progression
                            stats.enemiesKilled = 0;

                        }
                    }
                    else if (niveau == 2){
                        if (phaseStalac == 1) {
                            Stalactites_update(stalactites, MAX_STALAC, WIDTH, HEIGHT);
                            grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);

                            // vérifier si elles ont rempli l’écran
                            if (stalactites[0].x <= 0) {
                                phaseStalac = 2;
                                tempsNiveau = 1500; // durée du niveau grotte
                            }
                        }

                        // Phase active
                        else if (phaseStalac == 2) {
                            Stalactites_update(stalactites, MAX_STALAC, WIDTH, HEIGHT);
                            stalactique_collision(stalactites, MAX_STALAC, &x, &y, ship_w, ship_h, HEIGHT, &vies);

                            tempsNiveau--;
                            if (rand() % 500 == 0 && !coeurBonus.active)
                            {
                                spawn_coeur(&coeurBonus, WIDTH,stalactites, MAX_STALAC);

                            }
                            if (tempsNiveau <= 0) {
                                phaseStalac = 3;
                            }
                        }

                        // Phase sortie
                        else if (phaseStalac == 3) {
                            grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);

                            Stalactites_update_sortie(stalactites, MAX_STALAC);
                            stalactique_collision(stalactites, MAX_STALAC, &x, &y, ship_w, ship_h, HEIGHT, &vies);

                            if (stalactites[MAX_STALAC-1].x + stalactites[MAX_STALAC-1].width < 0) {
                                phaseStalac = 0;
                            }
                        }
                        else
                        {

                            niveau = 3;

                        }
                    } else if (niveau == 3){

                        if (progression < NombreEnnemi3)
                        {grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);
                            if (rand() % 400 == 0)
                            {
                                spawn_NIV_ENEMY2(EnemysNIV2, MAX_ENEMIES2, WIDTH, HEIGHT);

                            }
                            if (rand() % 500 == 0)
                            {

                                spawn_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES2, WIDTH, HEIGHT);

                            }
                            if (rand() % 600 == 0)
                            {
                                spawn_NIV_ENEMY3(EnemysNIV3, MAX_ENEMIES2, WIDTH, HEIGHT);
                            }
                            if (rand() % 2000 == 0 && !laserBonus.active)
                            {
                                spawn_laser(&laserBonus, WIDTH,HEIGHT);

                            }
                            if (rand() % 2000 == 0 && !coeurBonus.active)
                            {
                                spawn_coeur(&coeurBonus, WIDTH,stalactites, MAX_STALAC);

                            }
                            if (rand() % 1000 == 0)
                            {
                                spawn_NIV_ENEMY2BIS(EnemysNIV2BIS, MAX_ENEMIES3, WIDTH, HEIGHT);
                            }
                        }
                        else
                        {
                            // PASSAGE NIVEAU 2
                            niveau = 4;

                            // reset progression
                            stats.enemiesKilled = 0;
                        }
                    }
                    else if (niveau == 4 && tempsattentefinnieveau !=0 ) {
                        grotte_update(&g, &x, &y, ship_w, ship_h, HEIGHT, WIDTH, &vies, &collision);
                        tempsattentefinnieveau --;

                    }
                    else
                    {
                        running = 0;
                        victoire=1;
                    }

                    collision = 0;

                    if (tempsEntreLesTires > 0)
                        tempsEntreLesTires--;
                    else {
                        fire_bullet(bullet, x, y, MaxBullets);
                        tempsEntreLesTires = 17;
                    }

                    if (TempsChargementSpray > 0)
                        TempsChargementSpray--;

                    Updatescrolling(&x, &y, &bgx, key, ship_w, ship_h, WIDTH, HEIGHT, SPEED, SCROLL_SPEED, bg_w);

                    update_bulletsLASER(bulletLASER, x, y, MAX_BULLET_LASER, collision);
                    update_bullets(bullet, MaxBullets, BULLET_SPEED, WIDTH);
                    update_bulletsSpray(bullet_sp, MaxBullets, WIDTH, HEIGHT, BULLET_SPEED);
                    update_coeur(&coeurBonus,&vies,x,y,ship_w,ship_h);


                    update_NIV_ENEMY3(EnemysNIV3,MAX_ENEMIES3,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp,WIDTH,HEIGHT);
                    update_NIV_ENEMY2BIS(EnemysNIV2BIS,MAX_ENEMIES2,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp,WIDTH,HEIGHT);
                    update_NIV_ENEMY2(EnemysNIV2,MAX_ENEMIES2,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp,WIDTH,HEIGHT);
                    update_NIV_ENEMY1(EnemysNIV1,MAX_ENEMIES1,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp);
                    count_enemy1_stats(EnemysNIV1,MAX_ENEMIES1,&stats,son_explosion);
                    count_enemy2_stats(EnemysNIV2,MAX_ENEMIES2,&stats,son_explosion);
                    count_enemy3_stats(EnemysNIV3,MAX_ENEMIES3,&stats,son_explosion);
                    count_enemy2BIS_stats(EnemysNIV2BIS,MAX_ENEMIES2,&stats,son_explosion);
                    update_NIV_ENEMY1BIS(EnemysNIV1BIS,MAX_ENEMIES1,&vies,x,y,ship_w,ship_h,bullet,MaxBullets,bulletLASER,MAX_BULLET_LASER,bullet_sp);
                    count_enemy1BIS_stats(EnemysNIV1BIS,MAX_ENEMIES1,&stats,son_explosion);
                    update_laser_Recup(&laserBonus,&vies,x,y,ship_w,ship_h);
                    detecter_perte_vie(vies, son_hit);

                    redraw = 1;

                    if (key[ALLEGRO_KEY_SPACE] && nombreTirLaser > 0 &&
                        !bullet_active(bulletLASER, MAX_BULLET_LASER)) {
                        fire_bulletLASER(bulletLASER, x+40, y+10, MAX_BULLET_LASER);
                       jouer_son(son_laser);
                        nombreTirLaser--;
                        tempsEntreLesTires = 120;
                        }

                    if (key[ALLEGRO_KEY_ENTER] && TempsChargementSpray == 0) {
                        fire_bulletSPRAY(bullet_sp, x, y, MaxBullets);
                       jouer_son(son_tir);
                        TempsChargementSpray = TempsAttenteRechargement;
                    }
                    if (NOMBREVIE>3){ if (vies<NOMBREVIE) { vies++; }}
                }
            }

            // ================= RENDER =================
            if (redraw && al_is_event_queue_empty(queue)) {
                redraw = 0;
                al_clear_to_color(al_map_rgb(0, 0, 0));

                if (state == STATE_GAME) {


                    render(x, y, bgx, ship, background, ship_w, ship_h, WIDTH, HEIGHT, bg_w);

                    draw_bullets(bullet, MaxBullets);
                    draw_bulletSpray(bullet_sp, MaxBullets);
                    draw_coeur(&coeurBonus,coeur);
                    draw_laserplus(&laserBonus,tirLaser);

                    render_NIV_ENEMY3(EnemysNIV3, MAX_ENEMIES3, Enemy3, x, y, ship_w, ship_h, &vies);
                    render_NIV_ENEMY2BIS(EnemysNIV2BIS, MAX_ENEMIES2, Enemy2);
                    render_NIV_ENEMY2(EnemysNIV2, MAX_ENEMIES2, Enemy2);
                    render_NIV_ENEMY1(EnemysNIV1, MAX_ENEMIES1, Enemy1);
                    render_NIV_ENEMY1BIS(EnemysNIV1BIS, MAX_ENEMIES1, Enemy1);
                    if (phaseStalac != 2) {
                        grotte_render(g, HEIGHT, WIDTH, grotte);
                    }

                    if (phaseStalac != 0) {
                        stalactique_render(stalactites, MAX_STALAC, HEIGHT, WIDTH, grotte);
                    }

                    draw_bulletsLASER(bulletLASER, MAX_BULLET_LASER, WIDTH, x, y);

                    DrawNombreTirsLaser(nombreTirLaser, tirLaser);
                    DrawTempsEntreTir(TempsChargementSpray, tempsTir, WIDTH, HEIGHT, TempsAttenteRechargement);
                    vie(&vies, coeur, &fin);
                }
                if (fin==1) {
                    running=0;
                    victoire=0;
                }
            }
            else if (state == STATE_PAUSE) {



                al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                al_map_rgba(0,0,0,150));

                al_draw_text(font,
               al_map_rgb(255,255,255),
                WIDTH/2,
                  HEIGHT/2,
                  ALLEGRO_ALIGN_CENTER,
                   "PAUSE - P pour reprendre ");
                al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                    "MENU - M pour Menu");
                al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
              "S - Sauvegarde");

            }
            al_flip_display();
        }


        running = 1;
        if (fin==2) {
            running=0;
        }

        al_start_timer(timer);

        while (running) {
            ALLEGRO_EVENT ev;
            al_wait_for_event(queue, &ev);

            // ================= INPUT =================
            if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
                running = 0;

            else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                key[ev.keyboard.keycode] = true;
                if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                    running = 0;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_P && !p_pressed) {
                    p_pressed = true;

                    if (state == STATE_GAME)
                        state = STATE_PAUSE;
                    else if (state == STATE_PAUSE)
                        state = STATE_GAME;
                }
            }

            else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                key[ev.keyboard.keycode] = false;

                if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                    p_pressed = false;
            }

            // ================= UPDATE =================
            else if (ev.type == ALLEGRO_EVENT_TIMER) {

                if (state == STATE_GAME) {
                    if (key[ALLEGRO_KEY_ESCAPE]) running = 0;
                    redraw = 1;
                }



                // ================= RENDER =================
                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                    if (fin==1) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                       al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(255,0,0),
                        WIDTH/2,
                          HEIGHT/2,
                          ALLEGRO_ALIGN_CENTER,
                           "GAME OVER");
                        al_draw_text(fontpetit,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                            "M pour Menu");

                    }
                    else {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                       al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(255,255,255),
                        WIDTH/2  ,
                          HEIGHT/2 -40,
                          ALLEGRO_ALIGN_CENTER,
                           "LEVEL BEAT");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                           "Vous avez tuer %d ennemis et rater %d ennemis",(NombreEnnemi+NombreEnnemi1+NombreEnnemi3),stats.enemiesMissed);
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                      "Vous avez obtenu le score de %d",stats.score);
                        al_draw_text(fontpetit,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 200, ALLEGRO_ALIGN_CENTER,
                            "M pour Menu ou Niv suivant");

                    }
                }

                al_flip_display();
            }
        }
    return victoire;
    }

int nivBOSS(int HEIGHT, int WIDTH, int NOMBREVIE, int BULLET_SPEED, int SPEED, int MaxBullets, int TempsAttenteRechargement, int SCROLL_SPEED, ALLEGRO_BITMAP *background, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display,ALLEGRO_BITMAP *grotte  ) {
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);
    int victoire = 0;
    ALLEGRO_SAMPLE *son_laser = al_load_sample("laser.wav");
    ALLEGRO_SAMPLE *son_explosion = al_load_sample("Mort.wav");
    ALLEGRO_SAMPLE *son_hit = al_load_sample("Vie.wav");
    ALLEGRO_SAMPLE *son_tir = al_load_sample("tir.wav");

    if (!son_tir || !son_explosion || !son_hit || !son_laser) {
        printf("Erreur chargement sons\n");
    }
    int ship_w = 150;
            int ship_h = 50;
            int bg_w = al_get_bitmap_width(background);
            int vies = NOMBREVIE;// definit le nombre de vie dans la partie
            int MAX_BULLET_LASER =100;
            int nombreTirLaser=3;
            int tempsEntreLesTires = 0;
            int TempsChargementSpray=0;
            int kills = 0;
            bool boss_spawned = false;


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

            al_stop_timer(timer);
            boss_init(WIDTH, HEIGHT);
            boss_spawned = true;
            ALLEGRO_EVENT ev_vide;
            while (!al_is_event_queue_empty(queue))
                al_get_next_event(queue, &ev_vide);
            al_start_timer(timer);

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
                    if (tempsEntreLesTires>0) { // permet de laisser 10ms entre chaque tir
                        tempsEntreLesTires -= 1;
                    }
                    else {
                        fire_bullet(bullet,x,y, MaxBullets);

                        tempsEntreLesTires = 10;
                    }
                    if (TempsChargementSpray>0) { // permet de faire que les tirs se recharfe
                        TempsChargementSpray -= 1;
                    }

                    modeboss(&x, &y, ship, background, bgx, key,
                            ship_w, ship_h, WIDTH, HEIGHT); // dessine et prend en compte les déplacement du joueur
                    // on met a jour tout les tirs
                    update_bulletsLASER(bullets,x,y ,MAX_BULLET_LASER,collision);
                    update_bullets(bullet,MaxBullets,BULLET_SPEED,WIDTH);
                    update_bulletsSpray(bullet_sp,MaxBullets,WIDTH,HEIGHT,BULLET_SPEED);
                    boss_update(x, y, WIDTH, HEIGHT);
                    boss_check_player_bullets(bullet, MaxBullets);
                    boss_check_player_laser(bullets, MAX_BULLET_LASER);
                    boss_check_player_spray(bullet_sp, MaxBullets);

                    if (boss_bullet_hits_player(x, y, ship_w, ship_h) ||
                        boss_laser_hits_player(x, y, ship_w, ship_h)) {
                        vies--;
                        x = 100;
                        y = HEIGHT / 2;
                        }

                    redraw = 1;
                }
                // on vérifie quel touche ont été appuyé pour pouvoir faire les tirs qu'il faut
                if(key[ALLEGRO_KEY_SPACE]&& nombreTirLaser>0 && !bullet_active(bullets, MAX_BULLET_LASER)){
                    fire_bulletLASER(bullets, x+40, y+10, MAX_BULLET_LASER);
                    jouer_son(son_laser);
                    nombreTirLaser-=1;
                    tempsEntreLesTires = 120;
                }
                if(key[ALLEGRO_KEY_ENTER]&& TempsChargementSpray==0){
                    fire_bulletSPRAY(bullet_sp,x,y, MaxBullets);
                    jouer_son(son_tir);
                    TempsChargementSpray = TempsAttenteRechargement;
                    tempsEntreLesTires = 120;

                }


                //Rendu
                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    render(x, y, bgx,ship, background,ship_w, ship_h,WIDTH, HEIGHT,bg_w);
                    draw_bullets(bullet,MaxBullets);
                    draw_bulletSpray(bullet_sp,MaxBullets);
                    boss_draw(WIDTH, HEIGHT);
                    vie(&vies, coeur, &fin);

                    DrawNombreTirsLaser(nombreTirLaser,tirLaser);
                    draw_bulletsLASER(bullets,MAX_BULLET_LASER, WIDTH,x,y);
                    DrawTempsEntreTir(TempsChargementSpray,tempsTir,WIDTH,HEIGHT,TempsAttenteRechargement);
                    detecter_perte_vie(vies, son_hit);

                    al_flip_display();

                }
                if (fin==1) {
                    running = 0;
                    victoire=0;
                }
                if (boss_is_dead()){ running = 0; victoire=1;}
            }
     return victoire;
        }

        void debut(int HEIGHT, int WIDTH, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
            al_init_font_addon();
            al_init_ttf_addon();
            ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);
            ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 50, 0);
            ALLEGRO_FONT *fontMOYEN = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);
            ALLEGRO_FONT *fontMoG = al_load_ttf_font("PressStart2P-Regular.ttf", 35, 0);

            bool p_pressed = false;
            int redraw=0;
            int phase=0;


            bool key[ALLEGRO_KEY_MAX] = {0};

            int running = 1;
            al_start_timer(timer);

            while (running) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(queue, &ev);

                // ================= INPUT =================
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = 0;
                    return ;

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    key[ev.keyboard.keycode] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                        running=0;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_N) {
                        phase++;
                    }

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                    key[ev.keyboard.keycode] = false;

                    if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                        p_pressed = false;
                }

                // ================= UPDATE =================
                else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw=1;
                }


                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                    if (phase==0) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(255,255,255),
                        WIDTH/2,
                          HEIGHT/2 -20,
                          ALLEGRO_ALIGN_CENTER,
                           "ART-TYPE");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                            "Helene Charpentier Emy Lecanu");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                           "Jasmine Tolley Maxime Charton");
                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                            "N - next ");

                    }
                    if (phase==1) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                    al_map_rgba(0,0,0,150));
                        al_draw_text(fontMOYEN,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 - 200,
                         ALLEGRO_ALIGN_CENTER,
                          "Vous êtes un vaisseau perdu dans");
                        al_draw_text(fontMOYEN,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 - 150,
                         ALLEGRO_ALIGN_CENTER,
                          "l'esprit de Magdalena Abakanowicz");
                        al_draw_text(fontMOYEN,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 -100,
                         ALLEGRO_ALIGN_CENTER,
                          "Vous cherchez à en sortir");

                        al_draw_text(fontMoG,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 - 50,
                         ALLEGRO_ALIGN_CENTER,
                          "Comment ?");
                        al_draw_scaled_bitmap(ship,0, 0,al_get_bitmap_width(ship),al_get_bitmap_height(ship),WIDTH/2 -300, HEIGHT/2,600, 250,0);
                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                            "N - next ");

                    }
                    if (phase==2) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                    al_map_rgba(0,0,0,150));
                        al_draw_text(fontMOYEN,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 - 200,
                         ALLEGRO_ALIGN_CENTER,
                          "Vous allez devoir détruire ses creations");
                        al_draw_text(fontMOYEN,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 - 150,
                         ALLEGRO_ALIGN_CENTER,
                          "dans les trèfonds de son imagination");
                        al_draw_text(fontMOYEN,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 -100,
                         ALLEGRO_ALIGN_CENTER,
                          "et pour finir détruire son oeuvre finale");

                        al_draw_text(fontMOYEN,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 - 50,
                         ALLEGRO_ALIGN_CENTER,
                          "Concentrez vous pour ne pas périre");
                        al_draw_scaled_bitmap(Enemy1,0, 0,al_get_bitmap_width(Enemy1),al_get_bitmap_height(Enemy1),WIDTH/2-125 , HEIGHT/2,250, 250,ALLEGRO_ALIGN_CENTER);
                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                            "N - next ");

                    }
                    if (phase==3) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                    al_map_rgba(0,0,0,150));
                        al_draw_text(fontGROS,
                      al_map_rgb(255,255,255),
                       WIDTH/2,
                         HEIGHT/2 - 20,
                         ALLEGRO_ALIGN_CENTER,
                          "BONNE CHANCE");

                        al_draw_scaled_bitmap(Enemy2,0, 0,al_get_bitmap_width(Enemy2),al_get_bitmap_height(Enemy2),WIDTH/2-125 , HEIGHT/2,250, 250,ALLEGRO_ALIGN_CENTER);
                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                            "N - next");

                    }
                    if (phase==4) {
                        running =0;
                    }


                }
                al_flip_display();
            }

        }

        void debutNIV1(int HEIGHT, int WIDTH, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
            al_init_font_addon();
            al_init_ttf_addon();
            ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);
            ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 50, 0);
            ALLEGRO_FONT *fontMOYEN = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);

            bool p_pressed = false;
            int redraw=0;
            int phase=0;


            bool key[ALLEGRO_KEY_MAX] = {0};

            int running = 1;
            al_start_timer(timer);

            while (running) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(queue, &ev);

                // ================= INPUT =================
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = 0;
                    return ;

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    key[ev.keyboard.keycode] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                        running=0;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                        phase++;
                    }

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                    key[ev.keyboard.keycode] = false;

                    if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                        p_pressed = false;
                }

                // ================= UPDATE =================
                else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw=1;
                }


                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                    if (phase==0) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(173,147,120),
                        WIDTH/2,
                          HEIGHT/2 -40,
                          ALLEGRO_ALIGN_CENTER,
                           "NIVEAU 1");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                            "Espace - tir laser");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                           "Entrée - tir éparpillé");

                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "S - start ");

                    }
                    if (phase==1) {
                        running =0;
                    }


                }
                al_flip_display();
            }

        }

        void debutNIV2(int HEIGHT, int WIDTH, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
            al_init_font_addon();
            al_init_ttf_addon();
            ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);
            ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 50, 0);
            ALLEGRO_FONT *fontMOYEN = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);

            bool p_pressed = false;
            int redraw=0;
            int phase=0;


            bool key[ALLEGRO_KEY_MAX] = {0};

            int running = 1;
            al_start_timer(timer);

            while (running) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(queue, &ev);

                // ================= INPUT =================
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = 0;
                    return ;

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    key[ev.keyboard.keycode] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                        running=0;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                        phase++;
                    }

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                    key[ev.keyboard.keycode] = false;

                    if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                        p_pressed = false;
                }

                // ================= UPDATE =================
                else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw=1;
                }


                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                    if (phase==0) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(173,147,120),
                        WIDTH/2,
                          HEIGHT/2-40,
                          ALLEGRO_ALIGN_CENTER,
                           "NIVEAU 2");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                            "Espace - tir laser");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                           "Entrée - tir éparpillé");

                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "S - start ");

                    }
                    if (phase==1) {
                        running =0;
                    }


                }
                al_flip_display();
            }

        }

        void debutNIV3(int HEIGHT, int WIDTH, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
            al_init_font_addon();
            al_init_ttf_addon();
            ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);
            ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 50, 0);
            ALLEGRO_FONT *fontMOYEN = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);

            bool p_pressed = false;
            int redraw=0;
            int phase=0;


            bool key[ALLEGRO_KEY_MAX] = {0};

            int running = 1;
            al_start_timer(timer);

            while (running) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(queue, &ev);

                // ================= INPUT =================
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = 0;
                    return ;

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    key[ev.keyboard.keycode] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                        running=0;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                        phase++;
                    }

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                    key[ev.keyboard.keycode] = false;

                    if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                        p_pressed = false;
                }

                // ================= UPDATE =================
                else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw=1;
                }


                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                    if (phase==0) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(173,147,120),
                        WIDTH/2,
                          HEIGHT/2-40,
                          ALLEGRO_ALIGN_CENTER,
                           "NIVEAU 3");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                            "Espace - tir laser");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                           "Entrée - tir éparpillé");

                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "S - start ");

                    }
                    if (phase==1) {
                        running =0;
                    }


                }
                al_flip_display();
            }

        }

        void debutNIVDebutBoss(int HEIGHT, int WIDTH, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
            al_init_font_addon();
            al_init_ttf_addon();
            ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);
            ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 50, 0);
            ALLEGRO_FONT *fontMOYEN = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);
            ALLEGRO_FONT *fontMOYENG = al_load_ttf_font("PressStart2P-Regular.ttf", 30, 0);

            bool p_pressed = false;
            int redraw=0;
            int phase=0;


            bool key[ALLEGRO_KEY_MAX] = {0};

            int running = 1;
            al_start_timer(timer);

            while (running) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(queue, &ev);

                // ================= INPUT =================
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = 0;
                    return ;

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    key[ev.keyboard.keycode] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                        running=0;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                        phase++;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_N) {
                        phase++;
                    }

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                    key[ev.keyboard.keycode] = false;

                    if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                        p_pressed = false;
                }

                // ================= UPDATE =================
                else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw=1;
                }


                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    if (phase==0) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 -150, ALLEGRO_ALIGN_CENTER,
                            "Bravo vous êtes presque sorti de cette esprit tordu");
                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 - 100, ALLEGRO_ALIGN_CENTER,
                           "Il ne vous reste qu'un seul montre à détruire");
                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 - 50, ALLEGRO_ALIGN_CENTER,
                           "et vous voilà libre");
                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "N - Next ");

                    }
                    if (phase==1) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_textf(fontMOYENG,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 -10, ALLEGRO_ALIGN_CENTER,
                            "Ce n'est pas le moment de faillir !");
                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "N - Next ");

                    }

                    if (phase==2) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(173,147,120),
                        WIDTH/2,
                          HEIGHT/2-40,
                          ALLEGRO_ALIGN_CENTER,
                           "NIVEAU BOSS");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 70, ALLEGRO_ALIGN_CENTER,
                            "Espace - tir laser");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 100, ALLEGRO_ALIGN_CENTER,
                           "Entrée - tir éparpillé");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 130, ALLEGRO_ALIGN_CENTER,
                           "Fléches pour se déplacer");

                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "S - start ");

                    }
                    if (phase==3) {
                        running =0;
                    }


                }
                al_flip_display();
            }

        }

        void debutFin(int HEIGHT, int WIDTH, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
            al_init_font_addon();
            al_init_ttf_addon();
            ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);
            ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 50, 0);
            ALLEGRO_FONT *fontMOYEN = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);
            ALLEGRO_FONT *fontMOYENG = al_load_ttf_font("PressStart2P-Regular.ttf", 30, 0);

            bool p_pressed = false;
            int redraw=0;
            int phase=0;


            bool key[ALLEGRO_KEY_MAX] = {0};

            int running = 1;
            al_start_timer(timer);

            while (running) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(queue, &ev);

                // ================= INPUT =================
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = 0;
                    return ;

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    key[ev.keyboard.keycode] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                        running=0;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                        phase++;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_N) {
                        phase++;
                    }

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                    key[ev.keyboard.keycode] = false;

                    if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                        p_pressed = false;
                }

                // ================= UPDATE =================
                else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw=1;
                }


                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));
                    if (phase==0) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(173,147,120),
                        WIDTH/2,
                          HEIGHT/2-40,
                          ALLEGRO_ALIGN_CENTER,
                           "BOSS BATTU");

                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "N - Next ");

                    }
                    if (phase==1) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 -200, ALLEGRO_ALIGN_CENTER,
                            "Bravo vous avez réussi à vous echapper");
                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 - 150, ALLEGRO_ALIGN_CENTER,
                           "de l'imagination de Magdalena Abakanowicz");
                        al_draw_textf(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 , ALLEGRO_ALIGN_CENTER,
                           "Nous la remercions pour son inspiration");
                        al_draw_scaled_bitmap(Enemy3,0, 0,al_get_bitmap_width(Enemy3),al_get_bitmap_height(Enemy3),WIDTH/2-355 , HEIGHT/2,100, 250,ALLEGRO_ALIGN_CENTER);
                        al_draw_scaled_bitmap(Enemy1,0, 0,al_get_bitmap_width(Enemy1),al_get_bitmap_height(Enemy1),WIDTH/2-125, HEIGHT/2,250, 250,ALLEGRO_ALIGN_CENTER);
                        al_draw_scaled_bitmap(Enemy2,0, 0,al_get_bitmap_width(Enemy2),al_get_bitmap_height(Enemy2),WIDTH/2+125 , HEIGHT/2+10,250, 250,ALLEGRO_ALIGN_CENTER);

                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "N - Next ");

                    }
                    if (phase==2) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 -200, ALLEGRO_ALIGN_CENTER,
                            "J'espère que ce voyage vous a plu");
                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 -150, ALLEGRO_ALIGN_CENTER,
                           "Au revoir");
                        al_draw_textf(fontMOYEN,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 , ALLEGRO_ALIGN_CENTER,
                           "Et à la prochaine fois...");
                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "N - Retour au Menu ");

                    }
                    if (phase==3) {
                        running =0;
                    }


                }
                al_flip_display();
            }
        }
void MORTBoss(int HEIGHT, int WIDTH, ALLEGRO_BITMAP *ship, ALLEGRO_BITMAP *coeur,ALLEGRO_BITMAP *tirLaser, ALLEGRO_BITMAP *tempsTir,ALLEGRO_TIMER *timer,ALLEGRO_EVENT_QUEUE *queue,ALLEGRO_DISPLAY *display, ALLEGRO_BITMAP *Enemy1,ALLEGRO_BITMAP *Enemy2,ALLEGRO_BITMAP *Enemy3,ALLEGRO_BITMAP *grotte) {
            al_init_font_addon();
            al_init_ttf_addon();
            ALLEGRO_FONT *font = al_load_ttf_font("PressStart2P-Regular.ttf", 10, 0);
            ALLEGRO_FONT *fontGROS = al_load_ttf_font("PressStart2P-Regular.ttf", 50, 0);
            ALLEGRO_FONT *fontMOYEN = al_load_ttf_font("PressStart2P-Regular.ttf", 20, 0);

            bool p_pressed = false;
            int redraw=0;
            int phase=0;


            bool key[ALLEGRO_KEY_MAX] = {0};

            int running = 1;
            al_start_timer(timer);

            while (running) {
                ALLEGRO_EVENT ev;
                al_wait_for_event(queue, &ev);

                // ================= INPUT =================
                if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                    running = 0;
                    return ;

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                    key[ev.keyboard.keycode] = true;
                    if (ev.keyboard.keycode == ALLEGRO_KEY_M) {
                        running=0;
                    }
                    if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                        phase++;
                    }

                }

                else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
                    key[ev.keyboard.keycode] = false;

                    if (ev.keyboard.keycode == ALLEGRO_KEY_P)
                        p_pressed = false;
                }

                // ================= UPDATE =================
                else if (ev.type == ALLEGRO_EVENT_TIMER) {
                    redraw=1;
                }


                if (redraw && al_is_event_queue_empty(queue)) {
                    redraw = 0;
                    al_clear_to_color(al_map_rgb(0, 0, 0));

                    if (phase==0) {
                        al_draw_filled_rectangle(0,0,WIDTH,HEIGHT,
                   al_map_rgba(0,0,0,150));

                        al_draw_text(fontGROS,
                       al_map_rgb(173,147,120),
                        WIDTH/2,
                          HEIGHT/2 -40,
                          ALLEGRO_ALIGN_CENTER,
                           "GAME OVER");

                        al_draw_text(font,al_map_rgb(255,255,255),WIDTH/2,HEIGHT/2 + 300, ALLEGRO_ALIGN_CENTER,
                           "S - Return ");

                    }
                    if (phase==1) {
                        running =0;
                    }


                }
                al_flip_display();
            }

}

if  (niv1(HEIGHT, WIDTH, nombrevie_reel, BULLET_SPEED, SPEED, MaxBullets,
         TempsAttenteRechargement, SCROLL_SPEED, background, ship,
         coeur, tirLaser, tempsTir, timer, queue, display, Enemy1, Enemy2)) {
    if  (niv2(HEIGHT, WIDTH, nombrevie_reel, BULLET_SPEED, SPEED, MaxBullets,
             TempsAttenteRechargement, SCROLL_SPEED, background, ship,
             coeur, tirLaser, tempsTir, timer, queue, display,
             Enemy1, Enemy2, Enemy3, grotte)) {
        if  (niv3(HEIGHT, WIDTH, nombrevie_reel, BULLET_SPEED, SPEED, MaxBullets,
             TempsAttenteRechargement, SCROLL_SPEED, background, ship,
             coeur, tirLaser, tempsTir, timer, queue, display,
             Enemy1, Enemy2, Enemy3, grotte)) {
            if  (nivBOSS(HEIGHT, WIDTH, nombrevie_reel, BULLET_SPEED, SPEED, MaxBullets,
              TempsAttenteRechargement, SCROLL_SPEED, background, ship,
              coeur, tirLaser, tempsTir, timer, queue, display, grotte)){
                debutFin(HEIGHT, WIDTH, ship, coeur, tirLaser, tempsTir,
                         timer, queue, display, Enemy1, Enemy2, Enemy3, grotte);}
             }
             }
         }




