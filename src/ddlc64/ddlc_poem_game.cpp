#include <libdragon.h>
#include <fstream>
#include <sstream>
#include "engine.h"

namespace poemgame{ 

    #define POEM_GAME_WORDS 10
    #define POEM_GAME_NEEDEDWORDS 20

    typedef struct{ 
        sprite_t* sprite;
        sprite_t* sprite_happy;
        pos2d pos;
        pos2d vel;
        char* name;
        float nextjumptime;
        bool  jumpdir;
        bool  happy;
        int   char_entid;
        int   points = 0;
    } figure_t;

    typedef struct{
        std::string word;
        int sPoint, nPoint, yPoint;
    } poemwords_t;

    std::vector<poemwords_t> words = std::vector<poemwords_t>();
    sprite_t* background = NULL;
    rspq_block_t* backgroundblock = NULL;
    poemwords_t* randomwords[POEM_GAME_WORDS] = {0};
    figure_t figures[3];
    int selection = 0;
    int wordspicked = 0;

    void poem_minigame_load_words(){
        std::ifstream infile(filesystem_getfn(DIR_SCRIPT_LANG, "poemwords.txt").c_str());
        std::string line = std::string();

        // read and parse the file line by line
        while (std::getline(infile, line))
        {
            poemwords_t word = poemwords_t();
            std::stringstream ss(line);
            ss >> word.word;
            ss >> word.sPoint;
            ss >> word.nPoint;
            ss >> word.yPoint;
            words.push_back(word);
        }
    } 

    void poem_init(int index){
        selection = 0;
        wordspicked = 0;
        std::string bgfolder = std::string(gamestatus.data.bgfolder);
        background = sprite_load(filesystem_getfn(DIR_BG, "notebook").c_str());
        words.clear();

        // clear winners
        gamestatus.state.game.entities[characters["mc"].entityindex].variable[0] = 0;
    
        figure_t figs[3] = {
            {.sprite = sprite_load(filesystem_getfn(DIR_BG, "poem/n_sticker_1").c_str()),
            .sprite_happy = sprite_load(filesystem_getfn(DIR_BG, "poem/n_sticker_2").c_str()),
            .pos = {45,400}, .name = "n", .nextjumptime = 2, .jumpdir = true, .char_entid = characters["n"].entityindex},
            {.sprite = sprite_load(filesystem_getfn(DIR_BG, "poem/y_sticker_1").c_str()),
            .sprite_happy = sprite_load(filesystem_getfn(DIR_BG, "poem/y_sticker_2").c_str()),
            .pos = {195,400}, .name = "y", .nextjumptime = 1.5f, .jumpdir = false, .char_entid = characters["y"].entityindex},
            {.sprite = sprite_load(filesystem_getfn(DIR_BG, "poem/s_sticker_1").c_str()),
            .sprite_happy = sprite_load(filesystem_getfn(DIR_BG, "poem/s_sticker_2").c_str()),
            .pos = {120,400}, .name = "s", .nextjumptime = 1, .jumpdir = false, .char_entid = characters["s"].entityindex},
        }; for(int i = 0; i < 3; i++) figures[i] = figs[i];

        poem_minigame_load_words();

        for(int i = 0; i < POEM_GAME_WORDS; i++){
            int size = (int)words.size();
            int index = randm(size);
            randomwords[i] = &(words[index]);
        }
    }

    void poem_minigame_update(){
        for(int i = 0; i < 3; i++){
            figure_t& fig = figures[i];
            fig.pos.x += fig.vel.x; fig.pos.y += fig.vel.y;
            fig.vel.y += 0.5;
            if(fig.pos.y > 400) {fig.pos.y = 400; fig.vel.x = 0; fig.vel.y = 0; fig.happy = false;}

            fig.nextjumptime -= display_get_delta_time();
            if(fig.nextjumptime < 0) {fig.vel.y = -5; fig.vel.x = fig.jumpdir? 1 : -1; fig.jumpdir = !fig.jumpdir; fig.nextjumptime = frandr(2,9);}
            rdpq_sprite_blit_anchor(fig.happy? fig.sprite_happy : fig.sprite, ALIGN_CENTER, VALIGN_BOTTOM, fig.pos.x, fig.pos.y, NULL );
        }

        {
            joypad_poll();
            joypad_buttons_t pressed = joypad_get_buttons_pressed(JOYPAD_PORT_1);
            if(pressed.d_right)  selection ++;
            if(pressed.d_left) selection --;
            if(pressed.d_down)    selection +=2;
            if(pressed.d_up)  selection -=2; 
            if(selection >= POEM_GAME_WORDS) selection -= POEM_GAME_WORDS;
            if(selection < 0) selection += POEM_GAME_WORDS;

            if(pressed.a) {
                sound_play(gamestatus.data.defaults.selectsound, false);

                figures[0].points += randomwords[selection]->nPoint;
                figures[1].points += randomwords[selection]->yPoint;
                figures[2].points += randomwords[selection]->sPoint;

                if(randomwords[selection]->nPoint == 3) {figures[0].happy = true; figures[0].vel.y = -9; figures[0].nextjumptime = frandr(2,9);}
                if(randomwords[selection]->yPoint == 3) {figures[1].happy = true; figures[1].vel.y = -9; figures[0].nextjumptime = frandr(2,9);}
                if(randomwords[selection]->sPoint == 3) {figures[2].happy = true; figures[2].vel.y = -9; figures[0].nextjumptime = frandr(2,9);}

                for(int i = 0; i < POEM_GAME_WORDS; i++){
                    int size = (int)words.size();
                    int index = randm(size);
                    randomwords[i] = &(words[index]);
                }

                wordspicked++;

                // determine winner
                if(wordspicked == POEM_GAME_NEEDEDWORDS) {
                    if(figures[0].points > figures[1].points){
                        if(figures[0].points > figures[2].points) gamestatus.state.game.entities[characters["mc"].entityindex].variable[0] = 0;
                        else gamestatus.state.game.entities[characters["mc"].entityindex].variable[0] = 2;
                    } else{
                        if(figures[1].points > figures[2].points) gamestatus.state.game.entities[characters["mc"].entityindex].variable[0] = 1;
                        else gamestatus.state.game.entities[characters["mc"].entityindex].variable[0] = 2;
                    }
                }
                // they haven't seen the poem yet
                gamestatus.state.game.entities[figures[0].char_entid].variable[0] = 0;
                gamestatus.state.game.entities[figures[1].char_entid].variable[0] = 0;
                gamestatus.state.game.entities[figures[2].char_entid].variable[0] = 0;
                // set the poem points
                gamestatus.state.game.entities[figures[0].char_entid].variable[1] = figures[0].points;
                gamestatus.state.game.entities[figures[1].char_entid].variable[1] = figures[1].points;
                gamestatus.state.game.entities[figures[2].char_entid].variable[1] = figures[2].points;
            }
        }

            audioutils_mixer_update();
    }

    void poem_minigame_render(){
        if(!backgroundblock){
            rspq_block_begin();
            rdpq_set_mode_copy(true);
            rdpq_sprite_blit(background, 0,0,0);
            backgroundblock = rspq_block_end();
        } rspq_block_run(backgroundblock);

        rdpq_set_mode_standard();
        rdpq_mode_alphacompare(30);

        for(int i = 0; i < 3; i++){
            figure_t& fig = figures[i];
            rdpq_blitparms_t parms; parms.flip_x = fig.jumpdir? false : true;
            rdpq_sprite_blit_anchor(fig.happy? fig.sprite_happy : fig.sprite, ALIGN_CENTER, VALIGN_BOTTOM, fig.pos.x, fig.pos.y, &parms );
        }

        rdpq_text_printf(NULL, 2, 510, 62, "%i/%i", wordspicked, POEM_GAME_NEEDEDWORDS);

        for(int i = 0; i < POEM_GAME_WORDS; i++){
            rdpq_text_printf(NULL, 2, i % 2 == 0? 300 : 430, ((i / 2) * 62) + 90, i == selection? "^02%s" : "%s", randomwords[i]->word.c_str());
        }

    }

    void poem_transition(bool way){

        float t = 0;
        while(t < 2){
            t += display_get_delta_time();
                audioutils_mixer_update();
            rdpq_attach(display_try_get(), NULL);
            poem_minigame_render();
            rdpq_set_mode_standard();
            rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
            rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
            rdpq_mode_dithering(DITHER_SQUARE_INVSQUARE);
            rdpq_set_prim_color(RGBA32(0,0,0,(way? (2 - t) : t)*120));
            rdpq_fill_rectangle(0,0, display_get_width(), display_get_height());
            rdpq_detach_show();
        }

    }

    void poem_free(){
        rspq_wait();

        selection = 0;
        wordspicked = 0;
        if(background) {sprite_free(background); background = NULL;}
        if(backgroundblock) {rspq_block_free(backgroundblock); backgroundblock = NULL;}

        for(int i = 0; i < 3; i++){
            figure_t& fig = figures[i];
            sprite_free(fig.sprite); fig.sprite = NULL;
            sprite_free(fig.sprite_happy); fig.sprite_happy = NULL; 
        }

        words.clear();
    }

    void poem_minigame(int index){
        poem_init(index);
        poem_minigame_update();
        poem_transition(true);
        if(index == 0)
            overlays_message(dictstr("POEM_intro"));
        while(wordspicked < POEM_GAME_NEEDEDWORDS){
            poem_minigame_update();
            rdpq_attach(display_try_get(), NULL);
            poem_minigame_render();
            rdpq_detach_show();
        }
        poem_transition(false);
        poem_free();
    }

}