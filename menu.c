#include "menu.h"

#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>

// ─── Couleurs ─────────────────────────────────────────────────────────────────
#define COL_FOND        al_map_rgb(5,   5,  20)
#define COL_TITRE       al_map_rgb(255, 220, 50)
#define COL_SELECTIONE  al_map_rgb(80,  200, 255)
#define COL_NORMAL      al_map_rgb(180, 180, 180)
#define COL_DESACTIVE   al_map_rgb(80,  80,  80)
#define COL_CADRE       al_map_rgb(40,  60, 120)

// ─── Dessine un bouton ────────────────────────────────────────────────────────
static void dessine_bouton(ALLEGRO_FONT *font,
                           const char *texte,
                           float cx, float cy,
                           bool selectionne,
                           bool desactive) {
    float larg = 320;
    float haut = 52;
    float bx = cx - larg / 2;
    float by = cy - haut / 2;

    // Fond du bouton
    ALLEGRO_COLOR fond;
    if (desactive)
        fond = al_map_rgb(20, 20, 30);
    else if (selectionne)
        fond = al_map_rgb(20, 60, 100);
    else
        fond = al_map_rgb(10, 20, 50);

    al_draw_filled_rectangle(bx, by, bx + larg, by + haut, fond);

    // Contour
    ALLEGRO_COLOR contour = selectionne
        ? al_map_rgb(80, 200, 255)
        : al_map_rgb(40, 60, 120);
    al_draw_rectangle(bx, by, bx + larg, by + haut, contour, 2);

    // Triangle indicateur si sélectionné
    if (selectionne && !desactive) {
        al_draw_filled_triangle(
            bx - 20, cy,
            bx - 6,  cy - 8,
            bx - 6,  cy + 8,
            al_map_rgb(80, 200, 255));
    }

    // Texte
    ALLEGRO_COLOR col_texte = desactive ? COL_DESACTIVE
                            : selectionne ? COL_SELECTIONE
                            : COL_NORMAL;

    al_draw_text(font, col_texte,
                 cx, cy - al_get_font_line_height(font) / 2,
                 ALLEGRO_ALIGN_CENTRE, texte);
}

// ─── Dessine les étoiles de fond ──────────────────────────────────────────────
static void dessine_etoiles(int WIDTH, int HEIGHT, int frame) {
    // Étoiles fixes (pseudo-aléatoire déterministe)
    for (int i = 0; i < 80; i++) {
        int ex = (i * 137 + 41)  % WIDTH;
        int ey = (i * 251 + 97)  % HEIGHT;
        // Scintillement
        int bright = 150 + ((i * 37 + frame / 4) % 105);
        al_draw_filled_circle(ex, ey, 1,
                              al_map_rgb(bright, bright, bright));
    }
}

// ─── Dessine le titre stylisé ─────────────────────────────────────────────────
static void dessine_titre(ALLEGRO_FONT *font_titre,
                          ALLEGRO_FONT *font_sous,
                          int WIDTH, int frame) {
    // Halo derrière le titre
    al_draw_filled_rectangle(WIDTH/2 - 280, 60, WIDTH/2 + 280, 130,
                             al_map_rgba(0, 0, 60, 120));
    al_draw_rectangle(WIDTH/2 - 280, 60, WIDTH/2 + 280, 130,
                      al_map_rgb(40, 80, 180), 2);

    // Titre principal avec léger décalage couleur (effet pulsant)
    int pulse = (int)(20 * sinf(frame * 0.05f));
    al_draw_text(font_titre,
                 al_map_rgb(255, 220 + pulse, 50),
                 WIDTH / 2 + 2, 68,
                 ALLEGRO_ALIGN_CENTRE, "SPACE SHOOTER");

    // Sous-titre
    al_draw_text(font_sous,
                 al_map_rgb(120, 160, 255),
                 WIDTH / 2, 132,
                 ALLEGRO_ALIGN_CENTRE, "ECE HERO PROJECT");
}

// ─── Menu principal ───────────────────────────────────────────────────────────
int afficher_menu(ALLEGRO_DISPLAY *display,
                  ALLEGRO_EVENT_QUEUE *queue,
                  ALLEGRO_TIMER *timer,
                  int WIDTH, int HEIGHT) {

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();

    // Chargement des polices
    // Si tu n'as pas de .ttf, on utilise la police intégrée d'Allegro
    ALLEGRO_FONT *font_titre = al_create_builtin_font();
    ALLEGRO_FONT *font_menu  = al_create_builtin_font();
    ALLEGRO_FONT *font_info  = al_create_builtin_font();

    // ── Options du menu ──
    // index 0..4 = boutons principaux, index 5 = paramètres
    const char *labels[] = {
        "NIVEAU 1",
        "NIVEAU 2",
        "NIVEAU 3",
        "NIVEAU BOSS (direct)",
        "PARAMETRES",
        "QUITTER"
    };
    int nb_options = 6;
    int selection  = 0;   // option actuellement sélectionnée
    int choix      = -1;  // résultat final

    // Positions Y des boutons
    float debut_y = HEIGHT / 2 - 80;
    float espacement = 65;

    int frame = 0;
    bool redraw = true;

    al_start_timer(timer);

    while (choix == -1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            choix = MENU_QUITTER;
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            frame++;
            redraw = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_UP:
                selection = (selection - 1 + nb_options) % nb_options;
                break;

            case ALLEGRO_KEY_DOWN:
                selection = (selection + 1) % nb_options;
                break;

            case ALLEGRO_KEY_ENTER:
            case ALLEGRO_KEY_SPACE:
                switch (selection) {
                case 0: choix = MENU_NIV1;    break;
                case 1: choix = MENU_NIV2;    break;
                case 2: choix = MENU_NIV3;    break;
                case 3: choix = MENU_BOSS;    break;
                case 4:
                    // Ouvre les paramètres puis revient au menu
                    afficher_parametres(display, queue, timer, WIDTH, HEIGHT);
                    break;
                case 5: choix = MENU_QUITTER; break;
                }
                break;

            case ALLEGRO_KEY_ESCAPE:
                choix = MENU_QUITTER;
                break;
            }
        }

        // ── Rendu ──
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;

            // Fond
            al_clear_to_color(COL_FOND);

            // Étoiles
            dessine_etoiles(WIDTH, HEIGHT, frame);

            // Ligne décorative horizontale
            al_draw_line(WIDTH/2 - 300, HEIGHT/2 - 115,
                         WIDTH/2 + 300, HEIGHT/2 - 115,
                         al_map_rgb(40, 60, 120), 1);

            // Titre
            dessine_titre(font_titre, font_info, WIDTH, frame);

            // Boutons
            for (int i = 0; i < nb_options; i++) {
                float cy = debut_y + i * espacement;
                dessine_bouton(font_menu, labels[i],
                               WIDTH / 2.0f, cy,
                               (i == selection),
                               false);
            }

            // Aide navigation
            al_draw_text(font_info,
                         al_map_rgb(80, 80, 100),
                         WIDTH / 2, HEIGHT - 30,
                         ALLEGRO_ALIGN_CENTRE,
                         "HAUT / BAS : naviguer    ENTREE : valider    ECHAP : quitter");

            al_flip_display();
        }
    }

    al_destroy_font(font_titre);
    al_destroy_font(font_menu);
    al_destroy_font(font_info);

    return choix;
}

// ─── Menu paramètres ──────────────────────────────────────────────────────────
void afficher_parametres(ALLEGRO_DISPLAY *display,
                         ALLEGRO_EVENT_QUEUE *queue,
                         ALLEGRO_TIMER *timer,
                         int WIDTH, int HEIGHT) {

    ALLEGRO_FONT *font = al_create_builtin_font();

    // Options son (à relier à des variables globales plus tard)
    bool son_active      = true;
    bool musique_active  = true;
    bool laser_actif     = true;

    const char *options[] = {
        "Son general",
        "Musique",
        "Bruitages laser",
        "Retour"
    };
    bool *etats[] = { &son_active, &musique_active, &laser_actif, NULL };
    int nb = 4;
    int sel = 0;
    bool quitter = false;
    bool redraw  = true;

    while (!quitter) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) quitter = true;

        else if (ev.type == ALLEGRO_EVENT_TIMER) redraw = true;

        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_UP:
                sel = (sel - 1 + nb) % nb;
                break;

            case ALLEGRO_KEY_DOWN:
                sel = (sel + 1) % nb;
                break;

            case ALLEGRO_KEY_ENTER:
            case ALLEGRO_KEY_SPACE:
                if (sel == nb - 1) {
                    // Retour
                    quitter = true;
                } else {
                    // Toggle on/off
                    *etats[sel] = !(*etats[sel]);
                }
                break;

            case ALLEGRO_KEY_ESCAPE:
                quitter = true;
                break;
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(5, 5, 20));

            // Titre
            al_draw_text(font,
                         al_map_rgb(255, 220, 50),
                         WIDTH / 2, 80,
                         ALLEGRO_ALIGN_CENTRE, "PARAMETRES");

            al_draw_text(font,
                         al_map_rgb(80, 80, 120),
                         WIDTH / 2, 108,
                         ALLEGRO_ALIGN_CENTRE,
                         "(Les options son seront actives dans une prochaine version)");

            // Options
            float debut_y = HEIGHT / 2 - 60;
            float esp     = 65;

            for (int i = 0; i < nb; i++) {
                float cy = debut_y + i * esp;
                bool est_sel = (i == sel);

                // Cadre
                al_draw_filled_rectangle(WIDTH/2 - 200, cy - 24,
                                         WIDTH/2 + 200, cy + 24,
                                         est_sel
                                             ? al_map_rgb(20, 60, 100)
                                             : al_map_rgb(10, 20, 50));
                al_draw_rectangle(WIDTH/2 - 200, cy - 24,
                                  WIDTH/2 + 200, cy + 24,
                                  est_sel
                                      ? al_map_rgb(80, 200, 255)
                                      : al_map_rgb(40, 60, 120), 2);

                // Texte option
                al_draw_text(font,
                             est_sel
                                 ? al_map_rgb(80, 200, 255)
                                 : al_map_rgb(180, 180, 180),
                             WIDTH/2 - 140,
                             cy - al_get_font_line_height(font) / 2,
                             ALLEGRO_ALIGN_LEFT, options[i]);

                // État ON/OFF (sauf "Retour")
                if (etats[i] != NULL) {
                    bool val = *etats[i];
                    al_draw_text(font,
                                 val ? al_map_rgb(50, 220, 80)
                                     : al_map_rgb(200, 60, 40),
                                 WIDTH/2 + 140,
                                 cy - al_get_font_line_height(font) / 2,
                                 ALLEGRO_ALIGN_RIGHT,
                                 val ? "ON" : "OFF");
                }
            }

            al_draw_text(font,
                         al_map_rgb(80, 80, 100),
                         WIDTH / 2, HEIGHT - 30,
                         ALLEGRO_ALIGN_CENTRE,
                         "HAUT / BAS : naviguer    ENTREE : toggle / valider");

            al_flip_display();
        }
    }

    al_destroy_font(font);
}
