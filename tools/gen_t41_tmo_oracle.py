#!/usr/bin/env python3
"""Generate an OFFLINE TEST oracle, never linked into the ISP driver.

The bounded stock TMO compute routine has no MMIO or kernel calls. Relocate
it against private userspace workspaces to compare synthetic inputs with a
scalar implementation on T41. Requires the exact wrapped H20250310a object.
"""
import hashlib
import struct
import sys
from elftools.elf.elffile import ELFFile

with open(sys.argv[1], 'rb') as source:
    digest = hashlib.file_digest(source, 'sha256').hexdigest()
    assert digest == '572ff4553c1a033290ec67d2d9fc384701fbc235c2e335c7e5604100966fb2ee'
    source.seek(0)
    elf = ELFFile(source)
    code = elf.get_section_by_name('.text').data()
    rodata = elf.get_section_by_name('.rodata').data()
    symtab = elf.get_section_by_name('.symtab')
    rels = {r['r_offset']: r for r in elf.get_section_by_name('.rel.text').iter_relocations()}
    names = {'.bss': 'oracle_bss', '.data': 'oracle_data', '.rodata': 'oracle_rodata',
             'memcpy': 'oracle_copy', 'memset': 'oracle_fill',
             '__lshrdi3': 'oracle_shift', 'tisp_tmo_interplate': 'oracle_lerp'}
    print('.set noreorder\n.set noat\n.option pic0\n.text\n.balign 4')
    for name, start, end in [('oracle_lerp', 0x69300, 0x69330),
                             ('oracle_fpga', 0x69330, 0x69f50)]:
        print(f'.globl {name}\n.type {name}, @function\n{name}:')
        for pc in range(start, end, 4):
            word, = struct.unpack_from('<I', code, pc)
            if pc in rels:
                rel = rels[pc]
                entry = symtab.get_symbol(rel['r_info_sym'])
                sym = entry.name or elf.get_section(entry['st_shndx']).name
                target = names[sym]
                kind = rel['r_info_type']
                rs, rt, imm = (word >> 21) & 31, (word >> 16) & 31, word & 65535
                if kind == 5:
                    assert imm == 0
                    # Section workspaces are 64K aligned, and all section
                    # addends are below 0x8000. Function addends are zero.
                    print(f'lui ${rt}, %hi({target})')
                elif kind == 6:
                    assert imm < 0x8000
                    if word >> 26 == 9:
                        print(f'addiu ${rt}, ${rs}, %lo({target}+{imm})')
                    else:
                        op = {35: 'lw', 36: 'lbu'}[word >> 26]
                        print(f'{op} ${rt}, %lo({target}+{imm})(${rs})')
                else:
                    raise ValueError((hex(pc), kind))
            else:
                print(f'.word 0x{word:08x} # {pc:05x}')
        print(f'.size {name}, .-{name}')
    print('.section .rodata\n.balign 65536\n.globl oracle_rodata\noracle_rodata:')
    for pos in range(0, 0x3a00, 16):
        print('.byte ' + ','.join(str(x) for x in rodata[pos:pos+16]))
    print('.section .note.GNU-stack,"",@progbits')
