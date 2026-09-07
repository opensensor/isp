#!/usr/bin/env python3
"""Offline CCM test oracle: private userspace objects, no ISP access."""
import hashlib
import struct
import sys
from elftools.elf.elffile import ELFFile

with open(sys.argv[1], 'rb') as source:
    assert hashlib.file_digest(source, 'sha256').hexdigest() == \
        '572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee'
    source.seek(0)
    elf = ELFFile(source)
    code = elf.get_section_by_name('.text').data()
    symbols = elf.get_section_by_name('.symtab')
    functions = {
        'tisp_round_int64': 'oracle_round',
        'tisp_ccm_interp_by_ct': 'oracle_ct',
        'tisp_ccm_interp_by_ev': 'oracle_ev',
        'tisp_ccm_matrix_trans_by_sat': 'oracle_saturate',
        'tisp_ccm_write_reg': 'oracle_pack',
    }
    bcsh = len(sys.argv) > 2 and sys.argv[2] == 'bcsh'
    ae = len(sys.argv) > 2 and sys.argv[2] == 'ae'
    gib = len(sys.argv) > 2 and sys.argv[2] == 'gib'
    awb_gain = len(sys.argv) > 2 and sys.argv[2] == 'awb_gain'
    gamma = len(sys.argv) > 2 and sys.argv[2] == 'gamma'
    dpc = len(sys.argv) > 2 and sys.argv[2] == 'dpc'
    dmsc = len(sys.argv) > 2 and sys.argv[2] == 'dmsc'
    if bcsh:
        functions = dict(zip([
            'tisp_round_int64', 'tisp_min', 'tisp_max', 'tisp_bcsh_itp',
            'tisp_bcsh_matrix_multi', 'tisp_bcsh_interp_by_ct',
            'tisp_bcsh_interp_by_ev', 'tisp_bcsh_BCS_adjust',
            'tisp_bcsh_H_adjust', 'tisp_bcsh_aitp_to_hard', 'tisp_bcsh_write_reg',
        ], ['oracle_round', 'oracle_min', 'oracle_max', 'oracle_itp',
            'oracle_multiply', 'oracle_ct', 'oracle_ev', 'oracle_bcs',
            'oracle_hue', 'oracle_hard', 'oracle_pack']))
    if ae:
        functions = dict(zip([
            'tisp_ae_long_target', 'tisp_ae_lib_bilinear_intp',
            'tisp_ae_lib_div_64', 'tisp_ae_get_statistics',
            'tisp_ae_weight_mean', 'fix_point_div_32', 'fix_point_mult2_32',
        ], ['oracle_target', 'oracle_interpolate', 'oracle_divide',
            'oracle_statistics', 'oracle_mean', 'oracle_fixed_div',
            'oracle_fixed_mul']))
    if gib:
        functions = dict(zip(['tisp_gib_calc_self_gain', 'tisp_gib_ae_write_dgain',
            'tisp_round_int64', 'tisp_max'],
            ['oracle_self_gain', 'oracle_dgain', 'oracle_round', 'oracle_max']))
    if awb_gain:
        functions = dict(zip(['tisp_awb_gain_reg', 'tisp_awb_set_gain', 'fix_point_mult2_32'],
            ['oracle_pack', 'oracle_gain', 'oracle_fixed_mul']))
    if gamma:
        functions = dict(zip(['tisp_gamma_interp_by_ev', 'tisp_gamma_strength_transform',
            'tisp_gamma_write_lut_rgb', 'tisp_round_int64'],
            ['oracle_select', 'oracle_curve', 'oracle_pack', 'oracle_round']))
    if dpc:
        functions = dict(zip(['tisp_dpc_gain_interp', 'tisp_dpc_write_reg_long',
            'tisp_dpc_write_reg_short', 'tisp_simple_intp_int16', 'tisp_simple_intp_int8',
            'tisp_dpc_write_reg_other'],
            ['oracle_interpolate', 'oracle_long', 'oracle_short', 'oracle_lerp16', 'oracle_lerp8', 'oracle_other']))
    if dmsc:
        functions = dict(zip(['tisp_dmsc_intp', 'tisp_dmsc_noref_reg_cfg',
            'tisp_dmsc_ref_reg_cfg', 'tisp_simple_intp_int16', 'tisp_simple_intp_int8',
            'tisp_ratio'],
            ['oracle_interpolate', 'oracle_static', 'oracle_dynamic', 'oracle_lerp16', 'oracle_lerp8', 'oracle_ratio']))
    names = dict(functions, **{'.bss': 'oracle_bss', 'memcpy': 'oracle_copy',
        'memset': 'oracle_fill', '__ashrdi3': 'oracle_signed_shift',
        'system_reg_write': 'oracle_write', '.rodata': 'oracle_rodata',
        'isp_printf': 'oracle_unexpected'})
    if awb_gain:
        names['system_reg_set_awb_trig'] = 'oracle_trigger'
    if ae:
        names.update({'__ashldi3': 'oracle_left_shift',
            '__lshrdi3': 'oracle_shift', '__div64_32': 'oracle_div64',
            'y_arr': 'oracle_y',
            'tisp_ae_fliker_detect': 'oracle_noop',
            'tisp_ae_get_bv': 'oracle_noop'})
    rels = {r['r_offset']: r for r in elf.get_section_by_name('.rel.text').iter_relocations()}
    print('.set noreorder\n.set noat\n.option pic0\n.text\n.balign 4')
    for original, name in functions.items():
        symbol, = symbols.get_symbol_by_name(original)
        start, size = symbol['st_value'], symbol['st_size']
        print(f'.globl {name}\n.type {name}, @function\n{name}:')
        for pc in range(start, start + size, 4):
            word, = struct.unpack_from('<I', code, pc)
            if pc not in rels:
                print(f'.word 0x{word:08x} # {pc:05x}')
                continue
            rel = rels[pc]
            entry = symbols.get_symbol(rel['r_info_sym'])
            target = 'oracle_message' if entry.name.startswith('$LC') else \
                names[entry.name or elf.get_section(entry['st_shndx']).name]
            kind = rel['r_info_type']
            rs, rt, imm = word >> 21 & 31, word >> 16 & 31, word & 65535
            if kind == 5:
                assert imm == 0
                print(f'lui ${rt}, %hi({target})')
            elif kind == 6:
                assert imm < 0x8000
                if word >> 26 == 9:
                    print(f'addiu ${rt}, ${rs}, %lo({target}+{imm})')
                else:
                    op = {35: 'lw', 36: 'lbu', 43: 'sw'}[word >> 26]
                    print(f'{op} ${rt}, %lo({target}+{imm})(${rs})')
            else:
                raise ValueError((hex(pc), kind))
        print(f'.size {name}, .-{name}')
    if ae:
        # The target-table adjustment is inlined in tisp_ae_tune. This exact
        # bounded, call-free loop gets private caller-owned arrays, a private
        # calibration copy and a normal o32 stack; no surrounding AE routine.
        print('''\n.globl oracle_adjust
.type oracle_adjust, @function
oracle_adjust:
addiu $sp, $sp, -256
sw $s6, 240($sp)
move $s6, $a0
move $v1, $a1
move $a1, $a2
addiu $a0, $s6, 0x78
addiu $t1, $s6, 0x208''')
        for pc in range(0x272f4, 0x273d8, 4):
            assert pc not in rels
            word, = struct.unpack_from('<I', code, pc)
            print(f'.word 0x{word:08x} # {pc:05x}')
        print('''lw $s6, 240($sp)
jr $ra
addiu $sp, $sp, 256
.size oracle_adjust, .-oracle_adjust''')
    if bcsh:
        print('.section .rodata\n.balign 65536\n.globl oracle_rodata\noracle_rodata:')
        data = elf.get_section_by_name('.rodata').data()
        for pos in range(0, 0x2a70, 16):
            print('.byte ' + ','.join(str(v) for v in data[pos:pos+16]))
        print('oracle_message:\n.asciz "unexpected reference diagnostic"')
    print('.section .note.GNU-stack,"",@progbits')
