#include "menu.h"

#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>

bool sauvegarde_disponible = false;

// ─── Couleurs ─────────────────────────────────────────────────────────────────
#define COL_FOND        al_map_rgb(12, 5, 3)
#define COL_TITRE       al_map_rgb(255, 120, 40)
#define COL_SELECTIONE  al_map_rgb(220, 80, 40)
#define COL_NORMAL      al_map_rgb(170, 150, 130)
#define COL_DESACTIVE   al_map_rgb(70, 50, 45)
#define COL_CADRE       al_map_rgb(90, 35, 20)

bool son_active = true;
bool musique_active = true;
bool laser_actif = true;
int difficulte_globale = DIFFICULTE_NORMAL;

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
        fond = al_map_rgb(90, 30, 15);
    else
        fond = al_map_rgb(35, 15, 10);

    al_draw_filled_rectangle(bx, by, bx + larg, by + haut, fond);

    // Contour
    ALLEGRO_COLOR contour = selectionne
    ? al_map_rgb(255, 110, 50)
: al_map_rgb(100, 40, 20);
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
                              al_map_rgb(bright, bright / 3, 0));
    }
}

// ─── Dessine le titre stylisé ─────────────────────────────────────────────────
static void dessine_titre(ALLEGRO_FONT *font_titre,
                          ALLEGRO_FONT *font_sous,
                          int WIDTH, int frame) {
    // Halo derrière le titre
    al_draw_filled_rectangle(WIDTH/2 - 280, 60, WIDTH/2 + 280, 130,
                             al_map_rgba(80, 20, 0, 140));
    al_draw_rectangle(WIDTH/2 - 280, 60, WIDTH/2 + 280, 130,
                      al_map_rgb(120, 50, 20), 2);

    // Titre principal avec léger décalage couleur (effet pulsant)
    int pulse = (int)(20 * sinf(frame * 0.05f));
    al_draw_text(font_titre,
                 al_map_rgb(255, 80 + pulse, 30),
                 WIDTH / 2 + 2, 68,
                 ALLEGRO_ALIGN_CENTRE, "ART-TYPE");


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
    ALLEGRO_FONT *font_titre = al_create_builtin_font();
    ALLEGRO_FONT *font_menu  = al_create_builtin_font();
    ALLEGRO_FONT *font_info  = al_create_builtin_font();

    // ── Options du menu ──

    sauvegarde_disponible = (fopen("save.bin", "rb") != NULL);
    const char *labels[] = {
        "NIV1",
        "NIV2",
        "NIV3",
        "NIVBOSS",
        "Nouvelle Partie",
        "Sauvegarde",
        "Paramètre",
        "Quitter",
    };
    int nb_options = 8;
    int selection  = 0;   // option actuellement sélectionnée
    int choix      = -1;  // résultat final

    // Positions Y des boutons
    float debut_y = HEIGHT / 2 - 175;
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
                case 4: choix = MENU_TOUT; break;
                case 5:
                        // Reprendre : uniquement si save disponible
                        if (sauvegarde_disponible)
                            choix = MENU_REPRENDRE;
                        break;
                case 6:
                        afficher_parametres(display, queue, timer, WIDTH, HEIGHT);
                        break;
                case 7: choix = MENU_QUITTER;   break;
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
            al_clear_to_color(al_map_rgb(8, 2, 2));

            // Étoiles
            dessine_etoiles(WIDTH, HEIGHT, frame);

            // Titre
            dessine_titre(font_titre, font_info, WIDTH, frame);

            // Boutons
            for (int i = 0; i < nb_options; i++) {
                float cy = debut_y + i * espacement;
                bool desactive = (i == 5 && !sauvegarde_disponible); // grise "Reprendre"
                dessine_bouton(font_menu, labels[i],
                               WIDTH / 2.0f, cy,
                               (i == selection),
                               desactive);
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
// ─── Choisir difficulté (avec bouton Retour) ──────────────────────────────────

int choisir_difficulte(ALLEGRO_DISPLAY *display,
                       ALLEGRO_EVENT_QUEUE *queue,
                       ALLEGRO_TIMER *timer,
                       int WIDTH, int HEIGHT) {
    ALLEGRO_FONT *font = al_create_builtin_font();

    const char *options[] = {
        "SIMPLE (vies illimitees)",
        "NORMAL (3 coeurs)",
        "DIFFICILE (1 seul coup)",
        "Retour",
    };
    int nb  = 4;
    int sel = difficulte_globale; // pré-sélectionne le réglage actuel
    int choix = -1;
    bool redraw = true;

    while (choix == -1) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            choix = -2; // signal "fermeture"
        else if (ev.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
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
                if (sel == 3) {
                    // Retour : on ne change pas difficulte_globale
                    choix = difficulte_globale;
                } else {
                    difficulte_globale = sel;
                    choix = sel;
                }
                break;
            case ALLEGRO_KEY_ESCAPE:
                choix = difficulte_globale; // annule sans changer
                break;
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(8, 2, 2));

            al_draw_text(font, al_map_rgb(255, 120, 40),
                         WIDTH / 2, 80, ALLEGRO_ALIGN_CENTRE,
                         "CHOISIR LA DIFFICULTE");

            for (int i = 0; i < nb; i++) {
                float cy     = HEIGHT / 2 - 80 + i * 70;
                bool est_sel = (i == sel);
                // Mise en évidence de la difficulté actuellement active
                bool est_actif = (i == difficulte_globale && i < 3);

                al_draw_filled_rectangle(WIDTH/2 - 250, cy - 24,
                                         WIDTH/2 + 250, cy + 24,
                                         est_sel ? al_map_rgb(90, 30, 15)
                                                 : al_map_rgb(35, 15, 10));
                al_draw_rectangle(WIDTH/2 - 250, cy - 24,
                                  WIDTH/2 + 250, cy + 24,
                                  est_sel ? al_map_rgb(255, 110, 50)
                                          : al_map_rgb(100, 40, 20), 2);

                al_draw_text(font,
                             est_sel    ? al_map_rgb(220, 80, 40)
                           : est_actif  ? al_map_rgb(80, 220, 80)  // vert = actuel
                           :              al_map_rgb(170, 150, 130),
                             WIDTH / 2,
                             cy - al_get_font_line_height(font) / 2,
                             ALLEGRO_ALIGN_CENTRE, options[i]);

                // Petite puce verte pour marquer le réglage en cours
                if (est_actif)
                    al_draw_text(font, al_map_rgb(80, 220, 80),
                                 WIDTH/2 + 230,
                                 cy - al_get_font_line_height(font) / 2,
                                 ALLEGRO_ALIGN_RIGHT, "<");
            }

            al_draw_text(font, al_map_rgb(80, 80, 100),
                         WIDTH / 2, HEIGHT - 30, ALLEGRO_ALIGN_CENTRE,
                         "HAUT / BAS : naviguer    ENTREE : valider    ECHAP : retour");
            al_flip_display();
        }
    }

    al_destroy_font(font);
    return choix;
}


// ─── Menu paramètres ──────────────────────────────────────────────────────────
void afficher_parametres(ALLEGRO_DISPLAY *display,
                         ALLEGRO_EVENT_QUEUE *queue,
                         ALLEGRO_TIMER *timer,
                         int WIDTH, int HEIGHT) {

    ALLEGRO_FONT *font = al_create_builtin_font();

    // 3 options seulement : Son | Difficulté | Retour
    const char *options[] = {
        "Son general",
        "Difficulte",
        "Retour",
    };
    int nb  = 3;
    int sel = 0;
    bool quitter = false;
    bool redraw  = true;

    while (!quitter) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            quitter = true;
        else if (ev.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
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
                    switch (sel) {
                    case 0:
                            son_active = !son_active;
                            break;
                    case 1:
                            choisir_difficulte(display, queue, timer, WIDTH, HEIGHT);
                            break;
                    case 2:
                            quitter = true;
                            break;
                    }
                    break;

                case ALLEGRO_KEY_ESCAPE:   // ← un seul, ici
                    quitter = true;
                    break;
            }
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(8, 2, 2));

            al_draw_text(font, al_map_rgb(255, 220, 50),
                         WIDTH / 2, 80,
                         ALLEGRO_ALIGN_CENTRE, "PARAMETRES");

            float debut_y = HEIGHT / 2 - 60;
            float esp     = 65;

            for (int i = 0; i < nb; i++) {
                float cy     = debut_y + i * esp;
                bool est_sel = (i == sel);

                al_draw_filled_rectangle(WIDTH/2 - 220, cy - 24,
                                         WIDTH/2 + 220, cy + 24,
                                         est_sel ? al_map_rgb(90, 30, 15)
                                                : al_map_rgb(35, 15, 10));
                al_draw_rectangle(WIDTH/2 - 220, cy - 24,
                                  WIDTH/2 + 220, cy + 24,
                                  est_sel ? al_map_rgb(255, 110, 50)
                                          : al_map_rgb(100, 40, 20), 2);

                al_draw_text(font,
                             est_sel ? al_map_rgb(220, 80, 40)
                                     : al_map_rgb(170, 150, 130),
                             WIDTH/2 - 160,
                             cy - al_get_font_line_height(font) / 2,
                             ALLEGRO_ALIGN_LEFT, options[i]);


                // Valeur à droite selon l'option
                if (i == 0) {
                    // Son : ON / OFF
                    al_draw_text(font,
                                 son_active ? al_map_rgb(50, 220, 80)
                                            : al_map_rgb(200, 60, 40),
                                 WIDTH/2 + 160,
                                 cy - al_get_font_line_height(font) / 2,
                                 ALLEGRO_ALIGN_RIGHT,
                                 son_active ? "ON" : "OFF");
                } else if (i == 1) {
                    // Difficulté : affiche le nom du réglage actuel
                    const char *noms_diff[] = { "SIMPLE", "NORMAL", "DIFFICILE" };
                    al_draw_text(font,
                                 al_map_rgb(255, 200, 50),
                                 WIDTH/2 + 160,
                                 cy - al_get_font_line_height(font) / 2,
                                 ALLEGRO_ALIGN_RIGHT,
                                 noms_diff[difficulte_globale]);
                }
            }

            al_draw_text(font, al_map_rgb(80, 80, 100),
                         WIDTH / 2, HEIGHT - 30,
                         ALLEGRO_ALIGN_CENTRE,
                         "HAUT / BAS : naviguer    ENTREE : valider    ECHAP : retour");
            al_flip_display();
        }
    }

    al_destroy_font(font);
}
