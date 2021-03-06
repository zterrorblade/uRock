#ifndef __NONE_H__
#define __NONE_H__

#include "base-effect.h"
#include "none_img.bmp.h"

struct None_t{
    struct Effect_t parent;
};

struct Effect_t* new_None();

static struct EffectType_t NoneId __attribute__((unused)) = {
    .name = "None",
    .image = none_img, 
    .Init = new_None
};


#endif //__NONE_H__
