#ifndef LUAPP_LMATH_H
#define LUAPP_LMATH_H
#include <stdint.h>


//运算类函数
//加法
int64_t iadd(int64_t a,int64_t b);
double fadd(double a,double b);

//减法
int64_t isub(int64_t a,int64_t b);
double fsub(double a,double b);

//乘法
int64_t imul(int64_t a,int64_t b);
double fmul(double a,double b);

//取模
int64_t imod(int64_t a,int64_t b);


//除法/整除
double fdiv(double a,double b);
int64_t iidiv(int64_t a,int64_t b);
double fidiv(double a,double b);

//位运算
int64_t band(int64_t a,int64_t b);
int64_t bor(int64_t a,int64_t b);
int64_t bxor(int64_t a,int64_t b);
int64_t bnot(int64_t a,__attribute__((unused)) int64_t b);
int64_t shl(int64_t a,int64_t b);
int64_t shr(int64_t a,int64_t b);
//取反
int64_t iunm(int64_t a,__attribute__((unused)) int64_t b);
double funm(double a,__attribute__((unused)) double b);

#endif //LUAPP_LMATH_H
