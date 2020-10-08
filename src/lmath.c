#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lerror.h"
#include "lvalue.h"
#include "lstack.h"
#include "consts.h"
#include "convert.h"
#include "lstate.h"
#include "lvm.h"
static inline int64_t ShiftRight(int64_t a,int64_t n);
static inline int64_t ShiftLeft(int64_t a,int64_t n);

static inline int64_t ShiftRight(int64_t a,int64_t n) {
    if (n > 0)
        return a >> n;
    else
        return ShiftLeft(a,-n);
}

static inline int64_t ShiftLeft(int64_t a,int64_t n) {
    if (n >= 0)
        return a << n;
    else
        return ShiftRight(a,-n);
}

static inline int64_t IFloorDiv(int64_t a,int64_t b) {
    if ((a > 0 && b < 0) || (a < 0 && b < 0) || a%b == 0)
        return a / b;
    else
        return a / b - 1;
}
static inline double FFloorDiv(double a,double b) {
    if(b == 0)
        return INFINITY;
    return floor(a / b);
}


int64_t iadd(int64_t a,int64_t b) {
    return a + b;
}
double fadd(double a,double b) {
    return a + b;
}
int64_t isub(int64_t a,int64_t b) {
    return a - b;
}
double fsub(double a,double b) {
    return a - b;
}

int64_t imul(int64_t a,int64_t b) {
    return a * b;
}
double fmul(double a,double b) {
    return a * b;
}

int64_t imod(int64_t a,int64_t b) {
    if(b == 0)
        builtinRaiseError(vm->state, "Div by zero");
    return a % b;
}

double fdiv(double a,double b) {
    if(b == 0)
        return INFINITY;
    return a / b;
}
int64_t iidiv(int64_t a,int64_t b) {
    return IFloorDiv(a,b);
}
double fidiv(double a,double b) {
    return FFloorDiv(a,b);
}

int64_t band(int64_t a,int64_t b) {
    return a & b;
}
int64_t bor(int64_t a,int64_t b) {
    return a | b;
}
int64_t bxor(int64_t a,int64_t b) {
    return a ^ b;
}

int64_t shl(int64_t a,int64_t b) {
    return ShiftLeft(a,b);
}
int64_t shr(int64_t a,int64_t b) {
    return ShiftRight(a,b);
}
int64_t bnot(int64_t a,__attribute__((unused)) int64_t b) {
    return ~a;
}
int64_t iunm(int64_t a,__attribute__((unused)) int64_t b) {
    return -a;
}
double funm(double a,__attribute__((unused)) double b) {
    return -a;
}
