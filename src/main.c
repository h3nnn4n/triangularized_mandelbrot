#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <omp.h>

#include "color.h"
#include "types.h"
#include "mandel.h"
#include "image_utils.h"

#ifndef _SCHELL_
#define _SCHELL_ guided
#endif

int main(int argc, char *argv[]) {
    int     block_size;
    int     ix, iy;
    int     progress;
    int     total, i, j;
    int     *escapetime;
    int     *hist;

    srand(time(NULL));

    _config config;
    _color  *bitmap, *pal;

    config.screenx  =  1920;
    config.screeny  =  1080;

    config.screenx  =  800;
    config.screeny  =  600;

    config.bailout  =  2500;
    config.er       =  2;
    config.aa       =  1;

    config.minx     = -2.5;
    config.maxx     =  1.5;
    config.miny     = -2.0;
    config.maxy     =  2.0;

    config.minx     = -0.7436431355 - 0.000014628;
    config.maxx     = -0.7436431355 + 0.000014628;
    config.miny     =  0.131825963  - 0.000014628;
    config.maxy     =  0.131825963  + 0.000014628;

    /*config.minx     = -0.743643887037151 - 0.000000000051299 / 256.0;*/
    /*config.maxx     = -0.743643887037151 + 0.000000000051299 / 256.0;*/
    /*config.miny     =  0.131825904205330 - 0.000000000051299 / 256.0;*/
    /*config.maxy     =  0.131825904205330 + 0.000000000051299 / 256.0;*/

    /*config.minx     = -0.743643887037151 - 0.000000000051299 / 1024.0;*/
    /*config.maxx     = -0.743643887037151 + 0.000000000051299 / 1024.0;*/
    /*config.miny     =  0.131825904205330 - 0.000000000051299 / 1024.0;*/
    /*config.maxy     =  0.131825904205330 + 0.000000000051299 / 1024.0;*/

    block_size      =  10;

    progress        =  0;
    total           =  0;

    if ( argc > 1 ) {
        omp_set_num_threads(atoi(argv[1]));
    }

    hist       = ( int   * ) malloc ( sizeof ( int    ) * config.bailout                  );
    escapetime = ( int   * ) malloc ( sizeof ( int    ) * config.screenx * config.screeny );
    bitmap     = ( _color* ) malloc ( sizeof ( _color ) * config.screenx * config.screeny );
    pal        = ( _color* ) malloc ( sizeof ( _color ) * 255                             );

    populatePal ( pal ) ;

    bzero( hist, sizeof ( int ) * config.bailout );

    printf("%f \t %f\n%f\t %f\n", config.minx, config.maxx, config.miny, config.maxy);

    int bs = 10;

#pragma omp parallel for private(i,j) schedule(_SCHELL_)
    for (i = 0; i < config.screenx; i += bs) {
        for (j = 0; j < config.screeny; j += bs) {
            do_block(i, i + bs, j, j + bs, config, escapetime);
        }
        fprintf(stderr," -- %.2f%%\n",((progress += bs)/((double)config.screenx))*100.0);
    }
    progress = 0;

    block_size = 3;
#pragma omp parallel for private(i, j) shared(progress) schedule(_SCHELL_)
    for (i = 0; i < config.screenx/block_size; ++i) {
        for (j = 0; j < config.screeny/block_size; ++j) {
            int x, y, dx, dy;
            /*dx = 10 + rand()%5; //rand() % 20 - 10;*/
            /*dy = 10 + rand()%5; //rand() % 20 - 10;*/
            dx = rand() % 20 - 10;
            dy = rand() % 20 - 10;
            x  = rand() % (config.screenx - dx);
            y  = rand() % (config.screeny - dy);
            draw_line(x, x + dx, y, y + dy, config, escapetime);
        }
            fprintf(stderr," -- %.2f%%\n",((progress++)/((double)config.screenx/block_size))*100.0);
    }

    /*draw_line(0, config.screenx, 0, config.screeny, config, escapetime);*/

/*#pragma omp parallel for private(ix) schedule(_SCHELL_)*/
    /*for (i = 0; i < config.screenx/block_size; ++i) {*/
        /*for (j = 0; j < config.screeny/block_size; ++j) {*/
            /*int x, y, dx, dy;*/
            /*[>if ( rand() % 2 == 0 ) {<]*/
                /*[>dx = rand() % 20;<]*/
                /*[>dy = rand() % 10;<]*/
            /*[>} else {<]*/
                /*dx = rand() % 5 + 1;*/
                /*dy = rand() % 5 + 1;*/
            /*[>}<]*/
            /*x = rand() % (config.screenx - dx);*/
            /*y = rand() % (config.screeny - dy);*/
            /*do_block(x , x + dx, y , y + dy, config, escapetime);*/
        /*}*/
        /*fprintf(stderr," -- %.2f%%\n",((progress++)/((double)config.screenx/block_size))*100.0);*/
    /*}*/

    int max = 0;
    for ( iy = 0; iy < config.screeny; iy++ ) {
        for ( ix = 0; ix < config.screenx; ix++ ) {
            hist[escapetime[iy * config.screenx + ix]]++;
            max = max < escapetime[iy * config.screenx + ix] ? escapetime[iy * config.screenx + ix] : max;
        }
    }

    for ( i = 0; i < config.bailout; ++i) {
        total += hist[i];
    }

    for ( i = 0; i < config.bailout - 1; ++i) {
        hist[i] += hist[i-1];
    }


   for ( iy = 0; iy < config.screeny; iy++ ) {
        for ( ix = 0; ix < config.screenx; ix++ ) {
            if ( escapetime[iy * config.screenx + ix] == 0 ) {

            } else {
                /*bitmap[iy * config.screenx + ix].r = 255;*/
                /*bitmap[iy * config.screenx + ix].g = 255;*/
                /*bitmap[iy * config.screenx + ix].b = 255;*/
                bitmap[iy * config.screenx + ix] = getPalMem(hist[escapetime[iy * config.screenx + ix]]/(double)total, pal);
            }

            /*bitmap[iy * config.screenx + ix].r = (int)((escapetime[iy * config.screenx + ix] / (double)max) * 255.0);*/
            /*bitmap[iy * config.screenx + ix].g = (int)((escapetime[iy * config.screenx + ix] / (double)max) * 255.0);*/
            /*bitmap[iy * config.screenx + ix].b = (int)((escapetime[iy * config.screenx + ix] / (double)max) * 255.0);*/

            /*bitmap[iy * config.screenx + ix].r = escapetime[iy * config.screenx + ix];*/
            /*bitmap[iy * config.screenx + ix].g = escapetime[iy * config.screenx + ix];*/
            /*bitmap[iy * config.screenx + ix].b = escapetime[iy * config.screenx + ix];*/
        }
    }

    fprintf(stderr," -- %.2f%%\n",100.0);
    fprintf(stderr," <---- DONE ---->\n");
    fprintf(stderr," Writing to disk!\n");

    save_png_to_file(bitmap, config.screenx, config.screeny, "mandel.png");

    free(escapetime);

    fprintf(stderr," -- Bye\n");

    return EXIT_SUCCESS;
}
