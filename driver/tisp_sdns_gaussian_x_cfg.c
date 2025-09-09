#include "include/main.h"


  int32_t tisp_sdns_gaussian_x_cfg()

{
    return 0;
    system_reg_write(0x88d0, 0xf0007);
    system_reg_write(0x88d4, 0x33001e);
    system_reg_write(0x88d8, 0x3b001c);
    system_reg_write(0x88dc, 0xce0079);
    system_reg_write(0x88e0, 0x84003e);
    system_reg_write(0x88e4, 0x1d00111);
    system_reg_write(0x88e8, 0xeb006e);
    system_reg_write(0x88ec, 0x33801e5);
    system_reg_write(0x88f0, 0x16f00ad);
    system_reg_write(0x88f4, 0x50802f7);
    system_reg_write(0x88f8, 0x21000f8);
    system_reg_write(0x88fc, 0x73e0445);
    system_reg_write(0x8900, 0x2ce0152);
    system_reg_write(0x8904, 0x9db05d0);
    system_reg_write(0x8908, 0x3aa01b9);
    system_reg_write(0x890c, 0xce00797);
    system_reg_write(0x8910, 0x4a3022e);
    system_reg_write(0x8914, 0x104c099b);
    system_reg_write(0x8918, 0x5ba02b1);
    system_reg_write(0x891c, 0x141e0bdb);
    system_reg_write(0x8920, 0x4960342);
    system_reg_write(0x8924, 0x18580e59);
    system_reg_write(0x8928, 0x83f03e1);
    system_reg_write(0x892c, 0x1cf91113);
    system_reg_write(0x8930, 0x9ae048d);
    system_reg_write(0x8934, 0x22001409);
    system_reg_write(0x8938, 0xb3b0547);
    system_reg_write(0x893c, 0x276f173e);
    system_reg_write(0x8940, 0xce3047f);
    system_reg_write(0x8944, 0x2d441aad);
    system_reg_write(0x8948, 0xea906e5);
    system_reg_write(0x894c, 0x33801e5b);
}

