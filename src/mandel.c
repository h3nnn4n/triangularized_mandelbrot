#include "types.h"
#include "mandel.h"
#include "mandel_processor.h"

void fill_block(int ix_min, int ix_max, int iy_min, int iy_max, _config c, int *img, int aa) {
    int y = iy_min;
    for (; ix_min <= ix_max; ++ix_min) {
        for (iy_min = y; iy_min <= iy_max; ++iy_min) {
            if ( iy_min >= c.screeny ) continue;
            if ( ix_min >= c.screenx ) continue;
            img[ iy_min * c.screenx + ix_min ] = aa;
        }
    }
}

void finish_block(int ix_min, int ix_max, int iy_min, int iy_max, _config c, int *img) {
    int iy, ix;
    double cx, cy;

    for(iy = iy_min; iy < iy_max; iy++ ){
        cy = c.miny + iy*(c.maxy - c.miny) / c.screeny;
        for ( ix = ix_min; ix < ix_max; ix++ ) {
            cx = c.minx + ix * (c.maxx - c.minx) / c.screenx;
            img[iy * c.screenx + ix] = process_point(cx, cy, c.er, c.bailout);
        }
    }
}

int check(int ix_min, int ix_max, int iy_min, int iy_max, _config c, int w, int color){
    int iy, ix;
    double cx, cy;

    for(iy = iy_min; iy < iy_max; iy += w ){
        cy = c.miny + iy*(c.maxy - c.miny) / c.screeny;

        if ( process_point(c.minx + (ix_max) * (c.maxx - c.minx) / c.screenx, cy, c.er, c.bailout) != color )
            return 0;

        if ( process_point(c.minx + (ix_min) * (c.maxx - c.minx) / c.screenx, cy, c.er, c.bailout) != color )
            return 0;
    }

    for ( ix = ix_min; ix < ix_max; ix += w ) {
        cx = c.minx + (ix) * (c.maxx - c.minx) / c.screenx;

        if ( process_point(cx, c.miny + (iy_max) * (c.maxy - c.miny) / c.screeny, c.er, c.bailout) != color )
            return 0;

        if ( process_point(cx, c.miny + (iy_min) * (c.maxy - c.miny) / c.screeny, c.er, c.bailout) != color )
            return 0;
    }

    return 1;
}

void do_block(int ix_min, int ix_max, int iy_min, int iy_max, _config c, int *img) {
    if ( ix_max > c.screenx ) ix_max = c.screenx - 1;
    if ( iy_max > c.screeny ) iy_max = c.screeny - 1;

    int dx = (ix_max - ix_min ) / 2;
    int dy = (iy_max - iy_min ) / 2;

    int const eps = 6;

    double cx_min, cx_max;
    double cy_min, cy_max;

    cx_min = c.minx + ix_min * (c.maxx - c.minx) / c.screenx;
    cx_max = c.minx + ix_max * (c.maxx - c.minx) / c.screenx;

    cy_min = c.miny + iy_min * (c.maxy - c.miny) / c.screeny;
    cy_max = c.miny + iy_max * (c.maxy - c.miny) / c.screeny;

    int aa = process_point(cx_min, cy_min, c.er, c.bailout);
    int bb = process_point(cx_max, cy_max, c.er, c.bailout);
    int ab = process_point(cx_min, cy_max, c.er, c.bailout);
    int ba = process_point(cx_max, cy_min, c.er, c.bailout);

    if ( aa == bb  && aa == ab && aa == ba && check(ix_min, ix_max, iy_min, iy_max, c, 5, aa) ) {
        fill_block(ix_min, ix_max, iy_min, iy_max, c, img, aa);
    } else {
        if ( dx < eps && dy < eps ) {
            finish_block(ix_min , ix_max     , iy_min     , iy_max     , c, img);
        } else if ( dy < eps ) {
            do_block(ix_min     , ix_max - dx, iy_min     , iy_max     , c, img);
            do_block(ix_min + dx, ix_max     , iy_min     , iy_max     , c, img);
        } else if ( dx < eps ) {
            do_block(ix_min     , ix_max     , iy_min     , iy_max - dy, c, img);
            do_block(ix_min     , ix_max     , iy_min + dy, iy_max     , c, img);
        } else {
            do_block(ix_min     , ix_max - dx, iy_min     , iy_max - dy, c, img);
            do_block(ix_min + dx, ix_max     , iy_min + dy, iy_max     , c, img);
            do_block(ix_min + dx, ix_max     , iy_min     , iy_max - dy, c, img);
            do_block(ix_min     , ix_max - dx, iy_min + dy, iy_max     , c, img);
        }
    }
}
