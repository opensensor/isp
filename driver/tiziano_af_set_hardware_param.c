#include "include/main.h"


  int32_t tiziano_af_set_hardware_param()

{
    uint32_t $v0_2 = (data_d652c_1 - 0xf) / data_b1384_2;
    
    if ($v0_2 & 1)
        $v0_2 -= 1;
    
    data_b1388_1 = $v0_2;
    int32_t* $a1 = &stAFParam_Zone;
    int32_t* $a2 = &stAFParam_Zone;
    int32_t $a0_2 = 0;
    
    while (true)
    {
        $a2 = &$a2[1];
        
        if ($a0_2 >= data_b1384_3)
            break;
        
        $a0_2 += 1;
        $a2[3] = data_b1388_2;
    }
    
    uint32_t $v0_8 = (data_d6530_1 - 3) / data_b137c_2;
    
    if ($v0_8 & 1)
        $v0_8 -= 1;
    
    data_b13c4_1 = $v0_8;
    int32_t $v0_9 = 0;
    int32_t $a0_3;
    
    while (true)
    {
        $a0_3 = data_b137c_3;
        $a1 = &$a1[1];
        
        if ($v0_9 >= $a0_3)
            break;
        
        $v0_9 += 1;
        $a1[0x12] = data_b13c4_2;
    }
    
    if (!af_first)
    {
        af_first = 1;
        system_reg_write(0xb804, 
            data_b1384_4 << 0x1c | data_b1380_1 << 0x10 | stAFParam_Zone | $a0_3 << 0xc);
        system_reg_write(0xb808, 
            data_b1394_1 << 0x18 | data_b1390_1 << 0x10 | data_b1388_3 | data_b138c_1 << 8);
        system_reg_write(0xb80c, 
            data_b13a4_1 << 0x18 | data_b13a0_1 << 0x10 | data_b1398_1 | data_b139c_1 << 8);
        system_reg_write(0xb810, 
            data_b13b4_1 << 0x18 | data_b13b0_1 << 0x10 | data_b13a8_1 | data_b13ac_1 << 8);
        system_reg_write(0xb814, data_b13c0_1 << 0x10 | data_b13bc_1 << 8 | data_b13b8_1);
        system_reg_write(0xb818, 
            data_b13d0_1 << 0x18 | data_b13cc_1 << 0x10 | data_b13c4_3 | data_b13c8_1 << 8);
        system_reg_write(0xb81c, 
            data_b13e0_1 << 0x18 | data_b13dc_1 << 0x10 | data_b13d4_1 | data_b13d8_1 << 8);
        system_reg_write(0xb820, 
            data_b13f0_1 << 0x18 | data_b13ec_1 << 0x10 | data_b13e4_1 | data_b13e8_1 << 8);
        system_reg_write(0xb824, data_b13fc_1 << 0x10 | data_b13f8_1 << 8 | data_b13f4_1);
    }
    
    system_reg_write_af(1, 0xb828, 
        data_b1404_1 << 0x10 | data_b1354_1 << 8 | data_b1400_1 | data_b1350_1 << 7 | data_b134c_1 << 6
            | data_b1348_1 << 5 | stAFParam_ThresEnable << 4);
    int32_t $a1_41 = data_b1374_1 << 0x1c | data_b1370_1 << 0x18 | data_b1358_1 | data_b136c_1 << 0x14
        | data_b1368_1 << 0x10 | data_b1364_1 << 0xc | data_b1360_1 << 8 | data_b135c_1 << 4;
    system_reg_write(0xb82c, $a1_41);
    system_reg_write(0xb830, data_b1334_1 << 0x10 | stAFParam_FIR0_V);
    system_reg_write(0xb834, data_b133c_1 << 0x10 | data_b1338_1);
    system_reg_write(0xb838, data_b1340_1);
    system_reg_write(0xb83c, data_b12f0_1 << 0x10 | stAFParam_FIR1_V);
    system_reg_write(0xb840, data_b12f8_1 << 0x10 | data_b12f4_1);
    system_reg_write(0xb844, data_b12fc_1);
    system_reg_write(0xb848, data_b129c_1 << 0x10 | stAFParam_IIR0_H);
    system_reg_write(0xb84c, data_b12a4_1 << 0x10 | data_b12a0_1);
    system_reg_write(0xb850, data_b12b0_1 << 0x10 | data_b12a8_1);
    system_reg_write(0xb854, data_b12b8_1 << 0x10 | data_b12b4_1);
    system_reg_write(0xb858, data_b1244_1 << 0x10 | stAFParam_IIR1_H);
    system_reg_write(0xb85c, data_b124c_1 << 0x10 | data_b1248_1);
    system_reg_write(0xb860, data_b1258_1 << 0x10 | data_b1250_1);
    system_reg_write(0xb864, data_b1260_1 << 0x10 | data_b125c_1);
    system_reg_write(0xb868, 
        data_b131c_1 << 0x18 | data_b1318_1 << 0x10 | stAFParam_FIR0_Ldg | data_b1314_1 << 8);
    system_reg_write(0xb86c, 
        data_b132c_1 << 0x18 | data_b1328_1 << 0x10 | data_b1320_1 | data_b1324_1 << 8);
    system_reg_write(0xb870, 
        data_b12d8_1 << 0x18 | data_b12d4_1 << 0x10 | stAFParam_FIR1_Ldg | data_b12d0_1 << 8);
    system_reg_write(0xb874, 
        data_b12e8_1 << 0x18 | data_b12e4_1 << 0x10 | data_b12dc_1 | data_b12e0_1 << 8);
    system_reg_write(0xb878, 
        data_b1280_1 << 0x18 | data_b127c_1 << 0x10 | stAFParam_IIR0_Ldg | data_b1278_1 << 8);
    system_reg_write(0xb87c, 
        data_b1290_1 << 0x18 | data_b128c_1 << 0x10 | data_b1284_1 | data_b1288_1 << 8);
    system_reg_write(0xb880, 
        data_b1228_1 << 0x18 | data_b1224_1 << 0x10 | stAFParam_IIR1_Ldg | data_b1220_1 << 8);
    system_reg_write(0xb884, 
        data_b1238_1 << 0x18 | data_b1234_1 << 0x10 | data_b122c_1 | data_b1230_1 << 8);
    system_reg_write(0xb888, data_b1304_1 << 0x10 | stAFParam_FIR0_Coring);
    system_reg_write(0xb88c, data_b130c_1 << 0x10 | data_b1308_1);
    system_reg_write(0xb890, data_b12c0_1 << 0x10 | stAFParam_FIR1_Coring);
    system_reg_write(0xb894, data_b12c8_1 << 0x10 | data_b12c4_1);
    system_reg_write(0xb898, data_b1268_1 << 0x10 | stAFParam_IIR0_Coring);
    system_reg_write(0xb89c, data_b1270_1 << 0x10 | data_b126c_1);
    system_reg_write(0xb8a0, data_b1210_1 << 0x10 | stAFParam_IIR1_Coring);
    system_reg_write(0xb8a4, data_b1218_1 << 0x10 | data_b1214_1);
    return 0;
}

