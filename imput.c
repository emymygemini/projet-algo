#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/keyboard.h>
#include <allegro5/mouse.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "fonctions.h"
void instalationaudio(int *return1) {

    if(!al_install_audio()){
        fprintf(stderr,"Erreur installation audio\n");
        *return1 =   -1;
    }
    if(!al_init_acodec_addon()){
        fprintf(stderr,"Erreur codec audio\n");
        *return1 =-1;
    }
    if(!al_reserve_samples(16)){
        fprintf(stderr,"Erreur reserve samples\n");
        *return1= -1;
    }

}



