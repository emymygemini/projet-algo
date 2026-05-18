//
// Created by frede on 11/05/2026.
//
#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

// Valeurs de retour du menu
#define MENU_NIV1    1
#define MENU_NIV2    2
#define MENU_NIV3    3
#define MENU_BOSS    4
#define MENU_QUITTER 0
#define DIFFICULTE_SIMPLE   0
#define DIFFICULTE_NORMAL   1
#define DIFFICULTE_DIFFICILE 2

int choisir_difficulte(ALLEGRO_EVENT_QUEUE *queue,
                       ALLEGRO_TIMER *timer,
                       int WIDTH, int HEIGHT);


// Affiche le menu principal et retourne le choix du joueur
int afficher_menu(ALLEGRO_DISPLAY *display,
                  ALLEGRO_EVENT_QUEUE *queue,
                  ALLEGRO_TIMER *timer,
                  int WIDTH, int HEIGHT);

// Affiche le menu paramètres (son à implémenter plus tard)
void afficher_parametres(ALLEGRO_DISPLAY *display,
                         ALLEGRO_EVENT_QUEUE *queue,
                         ALLEGRO_TIMER *timer,
                         int WIDTH, int HEIGHT);

#endif // MENU_H
