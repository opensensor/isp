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
    names = dict(functions, **{'.bss': 'oracle_bss', 'memcpy': 'oracle_copy',
        'memset': 'oracle_fill', '__ashrdi3': 'oracle_signed_shift',
        'system_reg_write': 'oracle_write'})
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
            target = names[entry.name or elf.get_section(entry['st_shndx']).name]
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
    print('.section .note.GNU-stack,"",@progbits')
