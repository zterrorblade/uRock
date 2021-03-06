#ifndef __HELPRE_H__
#define __HELPER_H__

#include "base-effect.h"

void LinkPot(struct parameter_t *p, float value);
float unLinkPot(struct parameter_t *p);
float map(float value, float iupper, float ilower, float oupper, float olower);

void reverse(char s[]);
void itoa(int n, char s[]);

int intToStr(int x, char str[], int d);
void ftoa(float n, char *res, int afterpoint);

float atof(const char* s);

q31_t arm_tan_q31(q31_t x);
#endif //__HELPER_H__
