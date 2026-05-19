// boss.c Boss final du niveau BOSS
// Intégration : appeler boss_init() avant la boucle, boss_update() dans TIMER,
// boss_draw() dans le bloc rendu, et les check_* pour les collisions.

#include "boss.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Boss g_boss;

// Cherche un slot libre dans g_boss.bullets
static int bullet_alloc(void) {
    for (int i = 0; i < MAX_BOSS_BULLETS; i++)
        if (!g_boss.bullets[i].active) return i;
    return -1;
}

// Tir ciblé vers le joueur
static void fire_targeted(float player_x, float player_y, float spread_dy) {
    int idx = bullet_alloc();
    if (idx < 0) return;

    float ox = g_boss.x - g_boss.w * 0.5f;
    float oy = g_boss.y;
    float dx = player_x - ox;
    float dy = (player_y - oy) + spread_dy;
    float len = sqrtf(dx * dx + dy * dy);
    if (len < 0.5f) return;

    g_boss.bullets[idx].x  = ox;
    g_boss.bullets[idx].y  = oy;
    g_boss.bullets[idx].vx = (dx / len) * BOSS_BULLET_SPEED;
    g_boss.bullets[idx].vy = (dy / len) * BOSS_BULLET_SPEED;
    g_boss.bullets[idx].active = true;
}

// Rafale de 3 tirs ciblés
static void fire_burst(float player_x, float player_y) {
    float spreads[3] = { -80.0f, 0.0f, 80.0f };
    for (int s = 0; s < 3; s++)
        fire_targeted(player_x, player_y, spreads[s]);
}

//tirs éparpillés PARTIE 2
static void fire_scatter(void) {
    int num = 10;
    for (int j = 0; j < num; j++) {
        int idx = bullet_alloc();
        if (idx < 0) continue;
        float angle = (2.0f * (float)M_PI / num) * j;
        g_boss.bullets[idx].x  = g_boss.x;
        g_boss.bullets[idx].y  = g_boss.y;
        g_boss.bullets[idx].vx = cosf(angle) * BOSS_BULLET_SPEED;
        g_boss.bullets[idx].vy = sinf(angle) * BOSS_BULLET_SPEED;
        g_boss.bullets[idx].active = true;
    }
}


// Positions des 3parties du boooss
static void parts_sync_position(void) {
    float part_x = g_boss.x - g_boss.w * 0.5f + PART_W * 0.5f + 4;
    float offsets[3] = {-g_boss.h * 0.33f,0.0f, g_boss.h * 0.33f};
    for (int i = 0; i < 3; i++) {
        g_boss.parts[i].x = part_x;
        g_boss.parts[i].y = g_boss.y + offsets[i];
    }
}

//  Initialisation
void boss_init(int WIDTH, int HEIGHT) {
    g_boss = (Boss){0};
    g_boss.sprite = al_load_bitmap("bossf.png");
    g_boss.phase        = BOSS_PHASE1;
    g_boss.w = WIDTH * 0.25f;   // 1/4 de l'écran
    g_boss.h = HEIGHT;          // toute la hauteur

    // Le boss est collé à droite
    g_boss.x = WIDTH - g_boss.w * 0.5f;

    // Centre vertical
    g_boss.y = HEIGHT * 0.5f;
    g_boss.origin_y = g_boss.y;

    g_boss.float_amplitude = 55.0f;
    g_boss.float_speed     = 0.025f;  // radians/frame → période ~4s

    // 3 parties sensibles
    for (int i = 0; i < 3; i++) {
        g_boss.parts[i].hp    = PART_HP_MAX;
        g_boss.parts[i].alive = true;
    }
    parts_sync_position();

    // Première attaque après 2 secondes
    g_boss.attack_state = ATTACK_NONE;
    g_boss.attack_timer = 100;

    (void)WIDTH;
}

// Transition vers la phase 2
static void enter_phase2(int WIDTH, int HEIGHT) {
    g_boss.phase    = BOSS_PHASE2;
    g_boss.x        = WIDTH  * 0.5f;
    g_boss.y        = HEIGHT * 0.5f;
    g_boss.origin_y = HEIGHT * 0.5f;
    g_boss.vx = 9.0f;
    g_boss.vy = 7.0f;
    g_boss.w        = BOSS_MINI_W;
    g_boss.h        = BOSS_MINI_H;
    g_boss.hp       = BOSS_MINI_HP;
    g_boss.attack_state = ATTACK_NONE;
    g_boss.attack_timer = 60;
    g_boss.float_speed  = 0.06f;     // flottement plus rapide en phase 2
    // Désactiver tous les projectiles existants
    for (int i = 0; i < MAX_BOSS_BULLETS; i++)
        g_boss.bullets[i].active = false;
    printf("[BOSS] Phase 2 – boss miniature !\n");
}


// Mise à jour principale (une fois par frame)
void boss_update(float player_x, float player_y, int WIDTH, int HEIGHT) {
    if (g_boss.phase == BOSS_DEAD) return;

    //  Animation de mort
    if (g_boss.dying) {
        g_boss.death_timer++;
        if (g_boss.death_timer > 90) {
            g_boss.phase = BOSS_DEAD;
        }
        return;
    }

    // ── Flottement sinusoïdal ─────────────────────────────────────────────
    g_boss.float_timer += g_boss.float_speed;
    g_boss.y = g_boss.origin_y
               + sinf(g_boss.float_timer) * g_boss.float_amplitude;

    // Synchronisation de la position des parties
    if (g_boss.phase == BOSS_PHASE1)
        parts_sync_position();

    // Flash de dégâts
    if (g_boss.hit_flash > 0)    g_boss.hit_flash--;
    for (int i = 0; i < 3; i++)
        if (g_boss.parts[i].hit_flash > 0) g_boss.parts[i].hit_flash--;

    // Laser cooldown des parties
    for (int i = 0; i < 3; i++)
        if (g_boss.parts[i].laser_cooldown > 0) g_boss.parts[i].laser_cooldown--;


    // PHASE 1 – Logique d'attaques d
    if (g_boss.phase == BOSS_PHASE1) {

        // Laser : machine à états
        if (g_boss.attack_state == ATTACK_LASER) {
            g_boss.laser_timer--;
            if (g_boss.laser_warning && g_boss.laser_timer <= 0) {
                // Fin de l'avertissement activation du laser
                g_boss.laser_warning = false;
                g_boss.laser_timer   = LASER_ACTIVE_FRAMES;
            } else if (!g_boss.laser_warning && g_boss.laser_timer <= 0) {
                // Fin du laser et retour à l'attente
                g_boss.attack_state = ATTACK_NONE;
                g_boss.attack_timer = BOSS_ATTACK_MIN
                                    + rand() % (BOSS_ATTACK_MAX - BOSS_ATTACK_MIN);
            }
        }

        // Compte à rebours avant prochaine attaque
        if (g_boss.attack_state == ATTACK_NONE) {
            g_boss.attack_timer--;
            if (g_boss.attack_timer <= 0) {
                int choice = rand() % 2;
                if (choice == 0) {
                    // Attaque ciblée : rafale de 3 projectiles
                    fire_burst(player_x, player_y);
                    g_boss.attack_timer = BOSS_ATTACK_MIN
                                        + rand() % (BOSS_ATTACK_MAX - BOSS_ATTACK_MIN);
                    // attack_state reste ATTACK_NONE (attaque instantanée)
                } else {
                    // Attaque laser : choisir une ligne Y aléatoire
                    g_boss.attack_state  = ATTACK_LASER;
                    g_boss.laser_y       = 80 + rand() % (HEIGHT - 160);
                    g_boss.laser_warning = true;
                    g_boss.laser_timer   = LASER_WARN_FRAMES;
                }
            }
        }
        //  Vérifier si toutes les parties sont détruites
        bool all_dead = true;
        for (int i = 0; i < 3; i++)
            if (g_boss.parts[i].alive) { all_dead = false; break; }
        if (all_dead)
            enter_phase2(WIDTH, HEIGHT);
    }
    // PHASE 2 – Tirs éparpillés rapides
    //
    else if (g_boss.phase == BOSS_PHASE2) {
        g_boss.x += g_boss.vx;
        g_boss.y += g_boss.vy;
        g_boss.attack_timer--;
        if (g_boss.attack_timer <= 0) {
            fire_scatter();
            g_boss.attack_timer = 50 + rand() % 50;
        }
    }
    if (g_boss.x < g_boss.w * 0.5f)
        g_boss.vx = fabs(g_boss.vx);

    if (g_boss.x > WIDTH - g_boss.w * 0.5f)
        g_boss.vx = -fabs(g_boss.vx);

    if (g_boss.y < g_boss.h * 0.5f)
        g_boss.vy = fabs(g_boss.vy);

    if (g_boss.y > HEIGHT - g_boss.h * 0.5f)
        g_boss.vy = -fabs(g_boss.vy);

    //  Mise à jour des projectiles
    for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
        BossBullet *b = &g_boss.bullets[i];
        if (!b->active) continue;
        b->x += b->vx;
        b->y += b->vy;
        if (b->x < -20 || b->x > WIDTH  + 20 ||
            b->y < -20 || b->y > HEIGHT + 20)
            b->active = false;
    }
}

//  Rendu
void boss_draw(int WIDTH, int HEIGHT) {
    if (g_boss.phase == BOSS_DEAD && !g_boss.dying) return;

    // ── Animation de mort : explosion qui s'agrandit ──────────────────────
    if (g_boss.dying) {
        float ratio  = (float)g_boss.death_timer / 120.0f;
        float expand = ratio * 60.0f;
        // Plusieurs cercles d'explosion clignotants
        for (int k = 0; k < 5; k++) {
            float fx = g_boss.x + (rand() % 60 - 30);
            float fy = g_boss.y + (rand() % 60 - 30);
            float r  = 20.0f + ratio * 40.0f + k * 8;
            ALLEGRO_COLOR col = al_map_rgba_f(1.0f, 0.4f + k * 0.1f, 0.0f,
                                              1.0f - ratio);
            al_draw_filled_circle(fx, fy, r, col);
        }
        return;
    }

    // PHASE 1 – Gros corps + 3 parties
    if (g_boss.phase == BOSS_PHASE1) {
        float bx = g_boss.x;
        float by = g_boss.y;
        float hw = g_boss.w * 0.5f;
        float hh = g_boss.h * 0.5f;

        // Corps principal
        ALLEGRO_COLOR body_col = al_map_rgb(55, 60, 70);
        al_draw_scaled_bitmap(
    g_boss.sprite,

    0, 0,
    al_get_bitmap_width(g_boss.sprite),
    al_get_bitmap_height(g_boss.sprite),

    bx - hw,
    by - hh,
    g_boss.w,
    g_boss.h,

    0
);

        // Canon du boss
        ALLEGRO_COLOR cannon_col = al_map_rgb(40, 45, 55);
        al_draw_filled_rectangle(bx - hw - 20, by - 10, bx - hw, by + 10, cannon_col);



        // les 3 parties sensibles
        for (int i = 0; i < 3; i++) {
            if (!g_boss.parts[i].alive) {
                // Partie détruite : marques
                float px = g_boss.parts[i].x;
                float py = g_boss.parts[i].y;
                al_draw_filled_rectangle(px - PART_W*0.5f, py - PART_H*0.5f,
                                         px + PART_W*0.5f, py + PART_H*0.5f,
                                         al_map_rgb(30, 30, 30));
                al_draw_line(px - PART_W*0.5f, py - PART_H*0.5f,
                             px + PART_W*0.5f, py + PART_H*0.5f,
                             al_map_rgb(80, 80, 80), 2);
                continue;
            }

            float px = g_boss.parts[i].x;
            float py = g_boss.parts[i].y;
            float hp_ratio = (float)g_boss.parts[i].hp / PART_HP_MAX;

            // Couleur : selon HP
            ALLEGRO_COLOR part_col;
            if (hp_ratio > 0.6f)
                part_col = al_map_rgb(50, 220, 80);
            else if (hp_ratio > 0.3f)
                part_col = al_map_rgb(220, 160, 30);
            else
                part_col = al_map_rgb(220, 60,  40);

            // Flash lors d'un coup reçu
            if (g_boss.parts[i].hit_flash > 0)
                part_col = al_map_rgb(255, 255, 255);

            al_draw_filled_rectangle(px - PART_W*0.5f, py - PART_H*0.5f,
                                     px + PART_W*0.5f, py + PART_H*0.5f,
                                     part_col);

            // Contour pulsant
            al_draw_rectangle(px - PART_W*0.5f, py - PART_H*0.5f,
                               px + PART_W*0.5f, py + PART_H*0.5f,
                               al_map_rgb(255, 255, 255), 2);

            // Barre de HP sous la partie
            float bar_x = px - PART_W*0.5f;
            float bar_y = py + PART_H*0.5f + 4;
            al_draw_filled_rectangle(bar_x, bar_y, bar_x + PART_W, bar_y + 6,
                                     al_map_rgb(40, 40, 40));
            ALLEGRO_COLOR bar_col = al_map_rgb(
                (int)(255 * (1.0f - hp_ratio)),
                (int)(255 * hp_ratio), 0);
            al_draw_filled_rectangle(bar_x, bar_y,
                                     bar_x + PART_W * hp_ratio, bar_y + 6,
                                     bar_col);
        }

        // Laser boss : avertissement et rayon
        if (g_boss.attack_state == ATTACK_LASER) {
            if (g_boss.laser_warning) {
                // Clignotement rouge (toutes les 8 frames)
                if ((g_boss.laser_timer / 8) % 2 == 0) {
                    al_draw_filled_rectangle(0, g_boss.laser_y - 6,
                                             WIDTH, g_boss.laser_y + 6,
                                             al_map_rgba(255, 60, 60, 100));
                    al_draw_line(0, g_boss.laser_y, WIDTH, g_boss.laser_y,
                                 al_map_rgb(255, 60, 60), 2);
                }
                // Flèche d'indication côté boss
                al_draw_filled_triangle(
                    bx - hw - 40, g_boss.laser_y,
                    bx - hw - 10, g_boss.laser_y - 12,
                    bx - hw - 10, g_boss.laser_y + 12,
                    al_map_rgb(255, 100, 30));
            } else {
                // Laser actif : rayon rouge vif pleine largeur
                al_draw_filled_rectangle(0, g_boss.laser_y - 8,
                                         bx - hw, g_boss.laser_y + 8,
                                         al_map_rgb(255, 40, 40));
                // Cœur du rayon (blanc chaud)
                al_draw_filled_rectangle(0, g_boss.laser_y - 3,
                                         bx - hw, g_boss.laser_y + 3,
                                         al_map_rgb(255, 220, 180));
                // Halo extérieur
                al_draw_filled_rectangle(0, g_boss.laser_y - 14,
                                         bx - hw, g_boss.laser_y + 14,
                                         al_map_rgba(255, 60, 60, 60));
            }
        }
    }

// PHASE 2 – Boss miniature
//
else if (g_boss.phase == BOSS_PHASE2) {
    float bx = g_boss.x;
    float by = g_boss.y;
    float hw = g_boss.w * 0.5f;
    float hh = g_boss.h * 0.5f;

    // Corps : violet clignotant si flash
    ALLEGRO_COLOR col = (g_boss.hit_flash > 0)
        ? al_map_rgb(255, 255, 255)
        : al_map_rgb(180, 40, 220);

    al_draw_filled_rectangle(bx - hw, by - hh, bx + hw, by + hh, col);
    al_draw_rectangle(bx - hw, by - hh, bx + hw, by + hh,
                      al_map_rgb(255, 150, 255), 3);

    // Indicateur HP (2 petits cœurs en haut)
    for (int i = 0; i < g_boss.hp; i++) {
        al_draw_filled_circle(bx - 10 + i * 24, by - hh - 14, 8,
                              al_map_rgb(255, 80, 80));
    }

    // Texte indicatif (hachuré) – optionnel
    // Si vous avez allegro_font chargé, vous pouvez draw_text ici
}

// Projectiles du boss
for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
    const BossBullet *b = &g_boss.bullets[i];
    if (!b->active) continue;

    al_draw_filled_circle(b->x, b->y, 7.0f, al_map_rgb(255, 80, 30));
    al_draw_circle      (b->x, b->y, 9.0f,
                         al_map_rgba(255, 120, 60, 120), 1.5f);
}

(void)HEIGHT;
}

// Collisions : attaques du joueur sur le boss

// Bullets normaux (Bullet[])
void boss_check_player_bullets(Bullet bullets[], int max) {
    for (int b = 0; b < max; b++) {
        if (!bullets[b].active) continue;
        float bx = bullets[b].x + 5;
        float by = bullets[b].y + 1;

        // ── Phase 2 : frappe le corps miniature ───────────────────────────
        if (g_boss.phase == BOSS_PHASE2) {
            if (bx >= g_boss.x - g_boss.w*0.5f &&
                bx <= g_boss.x + g_boss.w*0.5f &&
                by >= g_boss.y - g_boss.h*0.5f &&
                by <= g_boss.y + g_boss.h*0.5f)
            {
                bullets[b].active = false;
                g_boss.hp--;
                g_boss.hit_flash = 12;
                printf("[BOSS P2] HP restant : %d\n", g_boss.hp);
                if (g_boss.hp <= 0) {
                    g_boss.dying       = true;
                    g_boss.death_timer = 0;
                }
            }
            continue;
        }


        // Phase 1 : frappe uniquement les parties vivantes
        if (g_boss.phase != BOSS_PHASE1) continue;
        for (int p = 0; p < 3; p++) {
            if (!g_boss.parts[p].alive) continue;
            float px = g_boss.parts[p].x;
            float py = g_boss.parts[p].y;
            if (bx >= px - PART_W*0.5f && bx <= px + PART_W*0.5f &&
                by >= py - PART_H*0.5f && by <= py + PART_H*0.5f)
            {
                bullets[b].active = false;
                g_boss.parts[p].hp--;
                g_boss.parts[p].hit_flash = 8;
                printf("[BOSS P1] Partie %d : HP %d/%d\n",
                       p, g_boss.parts[p].hp, PART_HP_MAX);
                if (g_boss.parts[p].hp <= 0) {
                    g_boss.parts[p].alive = false;
                    printf("[BOSS P1] Partie %d détruite !\n", p);
                }
                break;
            }
        }
    }
}

// Laser joueur (BulletLASER[])
void boss_check_player_laser(BulletLASER bullets[], int max) {
    for (int b = 0; b < max; b++) {
        if (!bullets[b].active) continue;
        float beam_y  = bullets[b].y;          // haut du faisceau
        float beam_y2 = bullets[b].y + 2;      // bas du faisceau

        // ── Phase 2 ───────────────────────────────────────────────────────
        if (g_boss.phase == BOSS_PHASE2) {
            if (beam_y2 >= g_boss.y - g_boss.h*0.5f &&
                beam_y  <= g_boss.y + g_boss.h*0.5f)
            {
                // 1 dégât par frame, limité par cooldown
                if (g_boss.hit_flash == 0) {
                    g_boss.hp--;
                    g_boss.hit_flash = 12;
                    printf("[BOSS P2 LASER] HP restant : %d\n", g_boss.hp);
                    if (g_boss.hp <= 0) {
                        g_boss.dying       = true;
                        g_boss.death_timer = 0;
                        bullets[b].active  = false;
                    }
                }
            }
            continue;
        }

        //  Phase 1
        if (g_boss.phase != BOSS_PHASE1) continue;
        for (int p = 0; p < 3; p++) {
            if (!g_boss.parts[p].alive) continue;
            if (g_boss.parts[p].laser_cooldown > 0) continue;

            float py1 = g_boss.parts[p].y - PART_H*0.5f;
            float py2 = g_boss.parts[p].y + PART_H*0.5f;
            if (beam_y2 >= py1 && beam_y <= py2) {
                g_boss.parts[p].hp--;
                g_boss.parts[p].hit_flash      = 8;
                g_boss.parts[p].laser_cooldown = LASER_HIT_COOLDOWN;
                printf("[BOSS P1 LASER] Partie %d : HP %d/%d\n",
                       p, g_boss.parts[p].hp, PART_HP_MAX);
                if (g_boss.parts[p].hp <= 0) {
                    g_boss.parts[p].alive = false;
                    printf("[BOSS P1] Partie %d détruite !\n", p);
                }
            }
        }
    }
}

// Bullets spray joueur (BulletSPRAY[])
// Chaque BulletSPRAY contient 10 sous-projectiles (Bullets[])
void boss_check_player_spray(BulletSPRAY bullets[], int max) {
    for (int b = 0; b < max; b++) {
        if (!bullets[b].active) continue;
        for (int j = 0; j < 10; j++) {
            if (!bullets[b].bullets[j].active) continue;
            float bx = bullets[b].bullets[j].x + 5;
            float by = bullets[b].bullets[j].y + 1;

            // Phase 2
            if (g_boss.phase == BOSS_PHASE2) {
                if (bx >= g_boss.x - g_boss.w*0.5f &&
                    bx <= g_boss.x + g_boss.w*0.5f &&
                    by >= g_boss.y - g_boss.h*0.5f &&
                    by <= g_boss.y + g_boss.h*0.5f)
                {
                    bullets[b].bullets[j].active = false;
                    g_boss.hp--;
                    g_boss.hit_flash = 12;
                    printf("[BOSS P2 SPRAY] HP restant : %d\n", g_boss.hp);
                    if (g_boss.hp <= 0) {
                        g_boss.dying       = true;
                        g_boss.death_timer = 0;
                    }
                }
                continue;
            }


            // Phase 1
            if (g_boss.phase != BOSS_PHASE1) continue;
            for (int p = 0; p < 3; p++) {
                if (!g_boss.parts[p].alive) continue;
                float px = g_boss.parts[p].x;
                float py = g_boss.parts[p].y;
                if (bx >= px - PART_W*0.5f && bx <= px + PART_W*0.5f &&
                    by >= py - PART_H*0.5f && by <= py + PART_H*0.5f)
                {
                    bullets[b].bullets[j].active = false;
                    g_boss.parts[p].hp--;
                    g_boss.parts[p].hit_flash = 8;
                    if (g_boss.parts[p].hp <= 0) {
                        g_boss.parts[p].alive = false;
                        printf("[BOSS P1] Partie %d détruite par spray!\n", p);
                    }
                    break;
                }
            }
        }
    }
}

//  Collisions : boss vers le joueur

// Vérifie si un projectile boss touche le joueur
bool boss_bullet_hits_player(float px, float py, float pw, float ph) {
    for (int i = 0; i < MAX_BOSS_BULLETS; i++) {
        BossBullet *b = &g_boss.bullets[i];
        if (!b->active) continue;
        if (b->x >= px && b->x <= px + pw &&
            b->y >= py && b->y <= py + ph)
        {
            b->active = false;  // le projectile est consommé
            return true;
        }
    }
    return false;
}

// Vérifie si le laser actif du boss touche le joueur
bool boss_laser_hits_player(float px, float py, float pw, float ph) {
    if (g_boss.phase != BOSS_PHASE1)        return false;
    if (g_boss.attack_state != ATTACK_LASER) return false;
    if (g_boss.laser_warning)               return false;  // avertissement ≠ actif

    float laser_top = g_boss.laser_y - 8;
    float laser_bot = g_boss.laser_y + 8;
    float player_top    = py;
    float player_bot    = py + ph;
    float player_right  = px + pw;

    // Le laser va de x=0 jusqu'au boss ; le joueur est à gauche
    if (player_right < 0) return false;
    return (player_bot >= laser_top && player_top <= laser_bot);
}

// Statut
bool boss_is_dead(void)   { return g_boss.phase == BOSS_DEAD; }
bool boss_is_active(void) { return g_boss.phase != BOSS_DEAD; }