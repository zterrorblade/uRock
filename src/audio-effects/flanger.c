#include "flanger.h"
#include "helper.h"
#include "FreeRTOS.h"

void Flanger(q31_t* pData, void *opaque){
    struct Flanger_t *tmp = (struct Flanger_t*)opaque;
    q31_t bData[256];
    q31_t fData[256];
    int32_t relativeBlock = 0;

    arm_copy_q31(pData, fData, SAMPLE_NUM);
    arm_scale_q31(pData, (q31_t)(0.5 * Q_1), Q_MULT_SHIFT, pData, SAMPLE_NUM);

    relativeBlock = (tmp->blockPtr - (uint32_t)(tmp->lfo.next(&(tmp->lfo))));

    if(relativeBlock < 0)
        relativeBlock += 400;

    BSP_SDRAM_ReadData(tmp->baseAddress + relativeBlock * 4 * SAMPLE_NUM, (uint32_t*)bData, SAMPLE_NUM);

    arm_scale_q31(bData, (q31_t)(0.5 * Q_1), Q_MULT_SHIFT, bData, SAMPLE_NUM);
    arm_add_q31(pData, bData, pData, SAMPLE_NUM);

    arm_scale_q31(bData, tmp->cache, Q_MULT_SHIFT, bData, SAMPLE_NUM);
    arm_add_q31(fData, bData, fData, SAMPLE_NUM);

    BSP_SDRAM_WriteData(tmp->baseAddress + tmp->blockPtr * SAMPLE_NUM * 4, (uint32_t*)fData, SAMPLE_NUM);

    tmp->blockPtr++;
    if(tmp->blockPtr >= 400)
        tmp->blockPtr = 0;

    return;
}

void delete_Flanger(void *opaque){
    struct Flanger_t *tmp = (struct Flanger_t*)opaque;
    releaseDelayLine(tmp->baseAddress);
    vPortFree(tmp); 
    return;
}

void adjust_Flanger(void *opaque, uint8_t* values){
    struct Flanger_t *tmp = (struct Flanger_t*)opaque;
    
    LinkPot(&(tmp->attenuation), values[0]);  
    LinkPot(&(tmp->speed), values[1]);  
    LinkPot(&(tmp->depth), values[2]);  

    tmp->cache = (q31_t)(powf(10, (tmp->attenuation.value * 0.1f)) * 2 * Q_1); //saving memory

    adjust_LFO_speed(&(tmp->lfo), tmp->speed.value);
    tmp->lfo.upperBound = tmp->depth.value;

    return;
}

void getParam_Flanger(void *opaque, struct parameter_t *param[], uint8_t* paramNum){
    struct Flanger_t *tmp = (struct Flanger_t*)opaque;
    *paramNum = 3;
    param[0] = &tmp->attenuation;
    param[1] = &tmp->speed;
    param[2] = &tmp->depth;
    return;
}


struct Effect_t* new_Flanger(){
    struct Flanger_t* tmp = pvPortMalloc(sizeof(struct Flanger_t));

    strcpy(tmp->parent.name, FlangerId.name);
    tmp->parent.FXid = &FlangerId;

    tmp->parent.func = Flanger;
    tmp->parent.del = delete_Flanger;
    tmp->parent.adj = adjust_Flanger;
    tmp->parent.getParam = getParam_Flanger;

    tmp->attenuation.name = "Attenuation";
    tmp->attenuation.upperBound = -2.0f;
    tmp->attenuation.lowerBound = -30.0f;
    tmp->attenuation.value = -30.0f;
    tmp->cache = (q31_t)(powf(10, (tmp->attenuation.value * 0.1f)) * 2 * Q_1); //saving memory

    tmp->speed.name = "Speed";
    tmp->speed.upperBound = 14.0f;
    tmp->speed.lowerBound = 0.25;
    tmp->speed.value = 0.25f;

    tmp->depth.name = "Depth";
    tmp->depth.upperBound = 200.0f;
    tmp->depth.lowerBound = 40.0;
    tmp->depth.value = 40.0f;

    new_LFO(&(tmp->lfo), 200, 40, tmp->speed.value);

    tmp->blockPtr = 0;
    tmp->baseAddress = allocateDelayLine();

    if(tmp->baseAddress < 0){
        vPortFree(tmp); 
        return NULL;
    }

    return (struct Effect_t*)tmp;
}


