#!/usr/bin/env python3
import re, sys
ASM = sys.argv[1]
BSS = {
    18112:"ydns_wdr_flags", 18120:"sec_ydns_intp", 18124:"main_ydns_intp",
    18128:"sec_ydns_comb", 18132:"main_ydns_comb", 18136:"sec_ydns", 18140:"main_ydns",
    18212:"main_ysp_intp", 18208:"sec_ysp_intp", 18216:"sec_ysp_comb", 18220:"main_ysp_comb",
    18224:"sec_ysp", 18228:"main_ysp", 18232:"ysp_wdr_flags",
}
lines = open(ASM).read().splitlines()
insns = []
for ln in lines:
    m = re.match(r'\s*([0-9a-f]+):\s+[0-9a-f]{8}\s+(\S+)\s*(.*)', ln)
    if m: insns.append([int(m.group(1),16), m.group(2), m.group(3).strip(), None])
    else:
        r = re.match(r'\s*[0-9a-f]+:\s+(R_MIPS_\S+)\s+(\S+)', ln)
        if r and insns: insns[-1][3] = (r.group(1), r.group(2))

class V:
    def __init__(s, expr, kind="expr"): s.expr, s.kind = expr, kind

regs = {"a0": V("ch","expr")}
out = []
warn = []
lwl_pend = {}

def get(r):
    if r == "zero": return V("0","const")
    return regs.get(r)

def step(idx):
    addr, op, args, rel = insns[idx]
    a = [x.strip() for x in args.split(',')] if args else []
    if op == "lui":
        if rel and rel[1] not in (".bss",".data"): regs[a[0]] = V(rel[1],"func")
        elif rel: regs[a[0]] = V(rel[1],"anchor")
        else: regs[a[0]] = V(str((int(a[1],0)<<16)),"const")
    elif op in ("addiu","addi"):
        src = get(a[1]); imm = int(a[2],0)
        if a[1] == "zero": regs[a[0]] = V(str(imm),"const")
        elif src is None: regs[a[0]] = None
        elif src.kind in ("anchor","func"): regs[a[0]] = src
        elif src.kind == "const": regs[a[0]] = V(str((int(src.expr)+imm)&0xffffffff),"const")
        else: regs[a[0]] = V(f"({src.expr}+{imm})" if imm>=0 else f"({src.expr}-{-imm})", src.kind)
    elif op == "lw":
        m2 = re.match(r'(-?\d+)\((\w+)\)', a[1]); off, base = int(m2.group(1)), m2.group(2)
        if rel and rel[0]=="R_MIPS_LO16" and rel[1]==".bss":
            regs[a[0]] = V(BSS.get(off, f"BSS_{off}"),"ptr")
        else:
            b = get(base)
            regs[a[0]] = V(f"W({b.expr}+{off})","ptr") if b else None
    elif op in ("lbu","lhu"):
        m2 = re.match(r'(-?\d+)\((\w+)\)', a[1]); off, base = int(m2.group(1)), m2.group(2)
        b = get(base); t = "B" if op=="lbu" else "H"
        regs[a[0]] = V(f"{t}({b.expr}+{off})","expr") if b else None
        if b is None: warn.append(f"{addr:x}: {op} untracked {base}")
    elif op == "lwl":
        m2 = re.match(r'(-?\d+)\((\w+)\)', a[1]); off, base = int(m2.group(1)), m2.group(2)
        lwl_pend[a[0]] = (base, off-3)
    elif op == "lwr":
        m2 = re.match(r'(-?\d+)\((\w+)\)', a[1]); off, base = int(m2.group(1)), m2.group(2)
        if a[0] in lwl_pend:
            b = get(base)
            regs[a[0]] = V(f"W4({b.expr}+{off})","expr") if b else None
            del lwl_pend[a[0]]
    elif op == "sll":
        s_ = get(a[1]); regs[a[0]] = V(f"({s_.expr}<<{int(a[2],0)})","expr") if s_ else None
    elif op in ("srl","sra"):
        s_ = get(a[1]); regs[a[0]] = V(f"({s_.expr}>>{int(a[2],0)})","expr") if s_ else None
    elif op == "andi":
        s_ = get(a[1]); regs[a[0]] = V(f"({s_.expr}&0x{int(a[2],0):x})","expr") if s_ else None
    elif op == "ori":
        s_ = get(a[1])
        if s_ and s_.kind=="const": regs[a[0]] = V(str(int(s_.expr)|int(a[2],0)),"const")
        elif s_: regs[a[0]] = V(f"({s_.expr}|0x{int(a[2],0):x})","expr")
        else: regs[a[0]] = None
    elif op == "or":
        x,y = get(a[1]), get(a[2])
        regs[a[0]] = V(f"({x.expr}|{y.expr})","expr") if x and y else None
    elif op == "addu":
        x,y = get(a[1]), get(a[2])
        if x and y:
            k = "ptr" if "ptr" in (x.kind,y.kind) else ("const" if x.kind==y.kind=="const" else "expr")
            e = str(int(x.expr)+int(y.expr)) if k=="const" else f"({x.expr}+{y.expr})"
            regs[a[0]] = V(e,k)
        else: regs[a[0]] = None
    elif op == "move": regs[a[0]] = get(a[1])
    elif op == "li": regs[a[0]] = V(str(int(a[1],0)),"const")
    elif op in ("sw","sh","sb"): pass
    elif op in ("jalr","jr.hb"):
        return "call", a[-1]
    elif op.startswith("b") or op=="j":
        warn.append(f"{addr:x}: BRANCH {op} {args}")
    return None, None

i = 0
while i < len(insns):
    kind, tgt = step(i)
    if kind == "call":
        if i+1 < len(insns): step(i+1)  # delay slot
        f = get(tgt)
        fname = f.expr if f else tgt
        a0, a1 = get("a0"), get("a1")
        out.append(f"system_reg_write({a0.expr if a0 else '?'}, {a1.expr if a1 else '?'});"
                   if fname=="system_reg_write" else f"// CALL {fname}({a0.expr if a0 else '?'}, {a1.expr if a1 else '?'})")
        i += 2
        continue
    i += 1

print(f"// warnings: {len(warn)}")
for w in warn: print("//", w)
for o in out: print(o)
