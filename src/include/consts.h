
#ifndef LUAPP_CONSTS_H

#define LUAPP_CONSTS_H

#define LUAPP_TNONE -1
#define LUAPP_TNIL 0
#define LUAPP_TBOOLEAN 1
#define LUAPP_TLIGHTUSERDATA 2
#define LUAPP_TINT 3
#define LUAPP_TFLOAT 4
#define LUAPP_TSTRING 5
#define LUAPP_TTABLE 6
#define LUAPP_TFUNCTION 7
#define LUAPP_TUSERDATA 8
#define LUAPP_TTHREAD 9
enum {  // 算术和按位运算符
    LUAPP_OPADD,        // +
    LUAPP_OPSUB,        // -
    LUAPP_OPMUL,       // *
    LUAPP_OPMOD,      // %
    LUAPP_OPPOW,      // ^
    LUAPP_OPDIV,        // /
    LUAPP_OPIDIV,       // //
    LUAPP_OPBAND,    // &
    LUAPP_OPBOR,      // |
    LUAPP_OPBXOR,   // ~
    LUAPP_OPSHL,      // <<
    LUAPP_OPSHR,      // >>
    LUAPP_OPUNM,    // - (unary minus)
    LUAPP_OPBNOT    // ~
}Op;
enum {  //比较运算符
    LUAPP_OPEQ, // ==
    LUAPP_OPLT, // <
    LUAPP_OPLE     // <=
} Comp;

#define HASH_SEED (3)
#endif //LUAPP_CONSTS_H
