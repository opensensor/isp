#!/usr/bin/env python3
"""Literal MIPS32->C translator for Tiziano leaf functions.
Each instruction becomes a C statement over uint32_t virtual registers;
branches become gotos with pre-evaluated conditions (delay-slot safe).
Anchored .bss/.data loads are mapped to driver symbols via BSSMAP.
Supports the opcode set used by the tisp_* reg_cfg/intp/wdr_en functions.
Usage: mips2c_literal.py <objdump-d-with-relocs.asm> <funcname> [bssmap.py]
"""
import re, sys

ASM, FNAME = sys.argv[1], sys.argv[2]
BSSMAP = {}
if len(sys.argv) > 3:
    exec(open(sys.argv[3]).read())  # defines BSSMAP = {offset: "symbol"}

lines = open(ASM).read().splitlines()
insns = []
for ln in lines:
    m = re.match(r'\s*([0-9a-f]+):\s+([0-9a-f]{8})\s+(\S+)\s*(.*)', ln)
    if m:
        insns.append({"addr": int(m.group(1),16), "op": m.group(3),
                      "args": m.group(4).strip(), "reloc": None})
    else:
        r = re.match(r'\s*[0-9a-f]+:\s+(R_MIPS_\S+)\s+(\S+)', ln)
        if r and insns:
            insns[-1]["reloc"] = (r.group(1), r.group(2))

# collect branch targets for labels
targets = set()
if len(sys.argv) > 4:
    for t in sys.argv[4].split(','):
        targets.add(int(t, 16))
for ins in insns:
    m = re.search(r'\b([0-9a-f]+)\s+<', ins["args"])
    if ins["op"].startswith(("b","j")) and m:
        targets.add(int(m.group(1),16))

REGS = ["zero","at","v0","v1","a0","a1","a2","a3","t0","t1","t2","t3","t4","t5","t6","t7",
        "s0","s1","s2","s3","s4","s5","s6","s7","t8","t9","k0","k1","gp","sp","fp","ra"]

def r(name):
    name = name.replace("$","")
    if name == "zero": return "0"
    if name == "fp": name = "s8"
    return f"r_{name}"

def memref(arg, ins, size):
    m = re.match(r'(-?\d+)\((\w+)\)', arg)
    off, base = int(m.group(1)), m.group(2)
    rel = ins["reloc"]
    if rel and rel[0] == "R_MIPS_LO16" and rel[1] in (".bss", ".data"):
        sym = BSSMAP.get(off)
        if sym:
            return f"((uintptr_t)&{sym})", 0, True
        return f"/*UNMAPPED {rel[1]}+{off}*/0", 0, True
    return r(base), off, False

cast = {1:"uint8_t", 2:"uint16_t", 4:"uint32_t"}
out = []
emitted_warn = []
regsym = {}   # register -> known function symbol

def emit(s): out.append("    " + s)

i = 0
hi_pending = {}
while i < len(insns):
    ins = insns[i]
    addr, op, args = ins["addr"], ins["op"], ins["args"]
    a = [x.strip() for x in args.split(',')] if args else []
    if addr in targets:
        out.append(f"L_{addr:x}:")
    def binop(fmt):
        emit(fmt.format(d=r(a[0]), s=r(a[1]), t=r(a[2]) if len(a)>2 else None))
    handled = True
    if op == "lui":
        rel = ins["reloc"]
        if rel and rel[1] not in (".bss",".data"):
            emit(f"{r(a[0])} = (uint32_t)(uintptr_t)&{rel[1]}; /* HI16 {rel[1]} */")
            hi_pending[a[0].replace('$','')] = rel[1]
            regsym[a[0].replace('$','')] = rel[1]
        elif rel:
            emit(f"{r(a[0])} = 0; /* HI16 {rel[1]} anchor */")
            hi_pending[a[0].replace('$','')] = rel[1]
        else:
            emit(f"{r(a[0])} = 0x{int(a[1],0)<<16:x}U;")
    elif op in ("addiu","addi"):
        rel = ins["reloc"]
        imm = int(a[2],0)
        if rel and rel[0]=="R_MIPS_LO16" and rel[1] not in (".bss",".data"):
            emit(f"{r(a[0])} = (uint32_t)(uintptr_t)&{rel[1]};")
        elif rel and rel[0]=="R_MIPS_LO16":
            sym = BSSMAP.get(imm if imm >= 0 else imm + 0x10000)
            key = imm if imm >= 0 else imm + 0x10000
            sym = BSSMAP.get(key)
            if sym: emit(f"{r(a[0])} = (uint32_t)(uintptr_t)&{sym};")
            else:
                emit(f"{r(a[0])} = 0; /* UNMAPPED {rel[1]}+{key} */")
                emitted_warn.append(f"{addr:x}: unmapped anchor {rel[1]}+{key}")
        else:
            emit(f"{r(a[0])} = {r(a[1])} + {'0x%x' % imm if imm>=0 else '-0x%x' % -imm}U;")
    elif op == "addu": binop("{d} = {s} + {t};")
    elif op == "subu": binop("{d} = {s} - {t};")
    elif op == "negu": emit(f"{r(a[0])} = (uint32_t)(-(int32_t){r(a[1])});")
    elif op == "and": binop("{d} = {s} & {t};")
    elif op == "or": binop("{d} = {s} | {t};")
    elif op == "xor": binop("{d} = {s} ^ {t};")
    elif op == "nor": binop("{d} = ~({s} | {t});")
    elif op == "andi": emit(f"{r(a[0])} = {r(a[1])} & 0x{int(a[2],0):x}U;")
    elif op == "ori":  emit(f"{r(a[0])} = {r(a[1])} | 0x{int(a[2],0):x}U;")
    elif op == "xori": emit(f"{r(a[0])} = {r(a[1])} ^ 0x{int(a[2],0):x}U;")
    elif op == "sll":  emit(f"{r(a[0])} = {r(a[1])} << {int(a[2],0)};")
    elif op == "srl":  emit(f"{r(a[0])} = {r(a[1])} >> {int(a[2],0)};")
    elif op == "sra":  emit(f"{r(a[0])} = (uint32_t)((int32_t){r(a[1])} >> {int(a[2],0)});")
    elif op == "sllv": emit(f"{r(a[0])} = {r(a[1])} << ({r(a[2])} & 31);")
    elif op == "srlv": emit(f"{r(a[0])} = {r(a[1])} >> ({r(a[2])} & 31);")
    elif op == "srav": emit(f"{r(a[0])} = (uint32_t)((int32_t){r(a[1])} >> ({r(a[2])} & 31));")
    elif op == "slt":  emit(f"{r(a[0])} = ((int32_t){r(a[1])} < (int32_t){r(a[2])}) ? 1U : 0U;")
    elif op == "sltu": emit(f"{r(a[0])} = ({r(a[1])} < {r(a[2])}) ? 1U : 0U;")
    elif op == "slti": emit(f"{r(a[0])} = ((int32_t){r(a[1])} < {int(a[2],0)}) ? 1U : 0U;")
    elif op == "sltiu":emit(f"{r(a[0])} = ({r(a[1])} < (uint32_t){int(a[2],0)}) ? 1U : 0U;")
    elif op == "movn": emit(f"if ({r(a[2])} != 0) {r(a[0])} = {r(a[1])};")
    elif op == "movz": emit(f"if ({r(a[2])} == 0) {r(a[0])} = {r(a[1])};")
    elif op == "mul":  emit(f"{r(a[0])} = (uint32_t)((int32_t){r(a[1])} * (int32_t){r(a[2])});")
    elif op in ("div","divu"):
        s_, t_ = (a[1], a[2]) if len(a)==3 else (a[0], a[1])
        c = "(int32_t)" if op=="div" else ""
        emit(f"if ({r(t_)}) {{ mips_lo = (uint32_t)({c}{r(s_)} / {c}{r(t_)}); mips_hi = (uint32_t)({c}{r(s_)} % {c}{r(t_)}); }}")
    elif op == "mflo": emit(f"{r(a[0])} = mips_lo;")
    elif op == "mfhi": emit(f"{r(a[0])} = mips_hi;")
    elif op == "ext":
        # ext rt, rs, pos, size
        pos, size = int(a[2],0), int(a[3],0)
        emit(f"{r(a[0])} = ({r(a[1])} >> {pos}) & 0x{(1<<size)-1:x}U;")
    elif op == "ins":
        pos, size = int(a[2],0), int(a[3],0)
        mask = ((1<<size)-1) << pos
        emit(f"{r(a[0])} = ({r(a[0])} & ~0x{mask:x}U) | (({r(a[1])} << {pos}) & 0x{mask:x}U);")
    elif op in ("lw","lbu","lhu","lb","lh"):
        size = 4 if op=="lw" else (1 if op in ("lbu","lb") else 2)
        base, off, isanchor = memref(a[1], ins, size)
        signed = op in ("lb","lh")
        c = cast[size]
        expr = f"*({c} *)(uintptr_t)({base} + {off})" if not isanchor else f"*({c} *)(uintptr_t){base}"
        if isanchor and off: expr = f"*({c} *)((uintptr_t){base} + {off})"
        if signed: expr = f"(uint32_t)(int{size*8}_t){expr}"
        emit(f"{r(a[0])} = {expr};")
    elif op in ("sw","sh","sb"):
        size = 4 if op=="sw" else (2 if op=="sh" else 1)
        base, off, isanchor = memref(a[1], ins, size)
        c = cast[size]
        lhs = f"*({c} *)(uintptr_t)({base} + {off})" if not isanchor else f"*({c} *)((uintptr_t){base} + {off})"
        emit(f"{lhs} = ({c}){r(a[0])};")
    elif op == "lwl":
        m2 = re.match(r'(-?\d+)\((\w+)\)', a[1]); off, base = int(m2.group(1)), m2.group(2)
        emit(f"{r(a[0])} = REGTRACE_LWLR({r(base)}, {off - 3}); /* lwl/lwr pair */")
    elif op == "lwr":
        emit(f"/* lwr handled with lwl above */")
    elif op in ("beq","bne","beqz","bnez","bltz","bgez","blez","bgtz","beql","bnel","beqzl","bnezl"):
        m2 = re.search(r'\b([0-9a-f]+)\s+<', args)
        tgt = int(m2.group(1),16)
        if op in ("beq","bne","beql","bnel"):
            cmp_ = f"{r(a[0])} {'==' if 'eq' in op else '!='} {r(a[1])}"
        elif op in ("beqz","beqzl"): cmp_ = f"{r(a[0])} == 0"
        elif op in ("bnez","bnezl"): cmp_ = f"{r(a[0])} != 0"
        elif op == "bltz": cmp_ = f"(int32_t){r(a[0])} < 0"
        elif op == "bgez": cmp_ = f"(int32_t){r(a[0])} >= 0"
        elif op == "blez": cmp_ = f"(int32_t){r(a[0])} <= 0"
        elif op == "bgtz": cmp_ = f"(int32_t){r(a[0])} > 0"
        emit(f"mips_cond = ({cmp_}) ? 1U : 0U;")
        if op.endswith("l"): emitted_warn.append(f"{addr:x}: branch-likely used (delay slot annulled when not taken!)")
        if i+1 < len(insns):
            insns[i+1]["__after"] = f"if (mips_cond) goto L_{tgt:x};"
        i += 1
        continue
    elif op in ("b","j"):
        m2 = re.search(r'\b([0-9a-f]+)\s+<', args)
        tgt = int(m2.group(1),16)
        if i+1 < len(insns):
            insns[i+1]["__after"] = f"goto L_{tgt:x};"
        i += 1
        continue
    elif op in ("jr","jr.hb"):
        if a[-1].replace('$','') == "ra":
            if i+1 < len(insns):
                insns[i+1]["__after"] = "goto fn_exit;"
            i += 1
            continue
        else:
            tgt = a[-1].replace('$','')
            sym = regsym.get(tgt)
            emitted_warn.append(f"{addr:x}: jr via {a[-1]} (tail call sym={sym})")
            callstr = (f"r_v0 = (uint32_t)REGCALL_{sym}(r_a0, r_a1, r_a2, r_a3); goto fn_exit;"
                       if sym else f"TAILCALL({r(a[-1])}); goto fn_exit;")
            if i+1 < len(insns):
                insns[i+1]["__after"] = callstr
            i += 1
            continue
    elif op == "jalr":
        tgt = a[-1].replace('$','')
        sym = regsym.get(tgt)
        callstr = (f"r_v0 = (uint32_t)REGCALL_{sym}(r_a0, r_a1, r_a2, r_a3);"
                   if sym else f"CALL_VIA({r(tgt)});")
        if i+1 < len(insns):
            insns[i+1]["__after"] = callstr
        i += 1
        continue
    elif op == "move":
        emit(f"{r(a[0])} = {r(a[1])};")
        s_ = a[1].replace('$',''); d_ = a[0].replace('$','')
        if s_ in regsym: regsym[d_] = regsym[s_]
    elif op == "li":
        emit(f"{r(a[0])} = {int(a[1],0) & 0xffffffff}U;")
    elif op == "nop":
        emit("/* nop */")
    else:
        emitted_warn.append(f"{addr:x}: UNHANDLED {op} {args}")
        emit(f"/* UNHANDLED {op} {args} */")
        handled = True
    after = ins.get("__after")
    if after: emit(after)
    i += 1

print(f"/* literal translation of {FNAME}; warnings: {len(emitted_warn)} */")
for w in emitted_warn: print(f"/* WARN {w} */")
print("\n".join(out))
