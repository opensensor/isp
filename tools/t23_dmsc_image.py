#!/usr/bin/env python3
"""Generate the T23 DMSC startup register image from an OEM tuning blob.

The field order comes from tiziano_dmsc_params_refresh, while register packing
comes from tisp_dmsc_{noref,ref}_reg_cfg in the Binary Ninja MCP cache.  At the
initial gain of 0x10000, tisp_simple_intp selects word 1 from every 9-word
curve.

Usage:
  tools/t23_dmsc_image.py TUNING_BIN MCP_CACHE ORIGINAL_KO -o OUTPUT
"""

import argparse
import hashlib
import json
import re
import subprocess
from pathlib import Path


PARAMS_KEY = "fn_0000000000054d94|tiziano_dmsc_params_refresh|54d94|1710"
NOREF_KEY = "fn_000000000004ec60|tisp_dmsc_noref_reg_cfg|4ec60|378"
REF_KEY = "fn_000000000004efd8|tisp_dmsc_ref_reg_cfg|4efd8|1084"

DMSC_FILE_OFFSET = 0x91B0
DMSC_PAYLOAD_SIZE = 0x2098
# Binary Ninja's anonymous data labels in these functions are eight bytes
# beyond the corresponding ELF relocation offsets.  Named DMSC symbols are
# unaffected; this bias is only used to resolve data_cb.../data_cc... aliases.
BN_ANON_BSS_BIAS = 0xA8EC0

REGISTER_SYMBOLS = {
    "tx_isp_vin_slake_subdev": 0x4798,
    "isp_i2c_new_subdev_board": 0x48A8,
    "subdev_sensor_ops_enum_input": 0x4978,
}


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("tuning_bin", type=Path)
    parser.add_argument("mcp_cache", type=Path)
    parser.add_argument("original_ko", type=Path)
    parser.add_argument("-o", "--output", type=Path)
    return parser.parse_args()


def parse_layout(text):
    layout = []
    pattern = re.compile(
        r"memcpy\(&([A-Za-z0-9_]+), .*?, (0x[0-9a-fA-F]+|[0-9]+)\)"
    )
    for line in text.splitlines():
        match = pattern.search(line)
        if match:
            layout.append((match.group(1), int(match.group(2), 0)))

    size = sum(field_size for _, field_size in layout)
    if len(layout) != 243 or size != DMSC_PAYLOAD_SIZE:
        raise ValueError(
            f"unexpected DMSC layout: fields={len(layout)} size=0x{size:x}"
        )
    return layout


def parse_symbols(module):
    output = subprocess.check_output(
        ["readelf", "-sW", str(module)], text=True
    )
    symbols = []
    for line in output.splitlines():
        parts = line.split()
        if len(parts) < 8 or parts[3] != "OBJECT":
            continue
        symbols.append((int(parts[1], 16), int(parts[2], 0), parts[7]))
    return symbols


def load_fields(tuning, layout):
    image = tuning.read_bytes()
    end = DMSC_FILE_OFFSET + DMSC_PAYLOAD_SIZE
    if len(image) < end:
        raise ValueError(
            f"tuning blob is too short: 0x{len(image):x}, need at least 0x{end:x}"
        )

    fields = {}
    cursor = DMSC_FILE_OFFSET
    for name, size in layout:
        fields[name] = image[cursor : cursor + size]
        cursor += size
    if cursor != end:
        raise AssertionError(f"DMSC cursor ended at 0x{cursor:x}, expected 0x{end:x}")
    return image, fields


def read_field(fields, name, offset=0, width=4):
    if name not in fields:
        raise KeyError(f"DMSC expression references unknown field {name}")
    data = fields[name]
    if offset + width > len(data):
        raise ValueError(
            f"DMSC field access exceeds {name}: offset={offset} width={width} "
            f"size={len(data)}"
        )
    return int.from_bytes(data[offset : offset + width], "little")


def alias_value(fields, symbols, alias):
    raw_address = int(alias.removeprefix("data_"), 16) - BN_ANON_BSS_BIAS
    matches = [
        (size, name, raw_address - address)
        for address, size, name in symbols
        if address <= raw_address < address + size
    ]
    if not matches:
        raise ValueError(f"cannot resolve Binary Ninja alias {alias}")

    _, name, offset = min(matches)
    if name in fields:
        return read_field(fields, name, offset)

    raise ValueError(f"{alias} resolved outside DMSC payload: {name}+0x{offset:x}")


def evaluate_value(expression, fields, symbols, temporaries):
    expression = expression.strip()
    expression = expression.replace("zx.d(", "(")

    for name, value in temporaries.items():
        expression = expression.replace(name, str(value))

    def replace_offset(match):
        name = match.group(1)
        offset = int(match.group(2), 0)
        width = {None: 4, "d": 4, "w": 2, "b": 1}[match.group(3)]
        return str(read_field(fields, name, offset, width))

    expression = re.sub(
        r"\b(dmsc_[A-Za-z0-9_]+):(0x[0-9a-fA-F]+|[0-9]+)(?:\.([dwb]))?",
        replace_offset,
        expression,
    )
    expression = re.sub(
        r"\b(dmsc_[A-Za-z0-9_]+)\.d",
        lambda match: str(read_field(fields, match.group(1))),
        expression,
    )

    def replace_intp(match):
        name = match.group(1)
        array_name = name.removesuffix("_intp") + "_array"
        return str(read_field(fields, array_name, 4))

    expression = re.sub(
        r"\b(dmsc_[A-Za-z0-9_]+_intp)\b", replace_intp, expression
    )
    expression = re.sub(
        r"\b(data_[0-9a-f]+)\b",
        lambda match: str(alias_value(fields, symbols, match.group(1))),
        expression,
    )

    field_names = sorted(fields, key=len, reverse=True)
    field_pattern = r"\b(" + "|".join(map(re.escape, field_names)) + r")\b"
    expression = re.sub(
        field_pattern,
        lambda match: str(read_field(fields, match.group(1))),
        expression,
    )

    try:
        value = eval(expression, {"__builtins__": {}}, {})
    except (NameError, SyntaxError) as error:
        raise ValueError(
            f"unresolved DMSC value expression: {expression}"
        ) from error
    return int(value) & 0xFFFFFFFF


def evaluate_register(expression):
    expression = expression.strip()
    for name, value in REGISTER_SYMBOLS.items():
        expression = expression.replace(name, str(value))
    try:
        return int(eval(expression, {"__builtins__": {}}, {}))
    except (NameError, SyntaxError) as error:
        raise ValueError(
            f"unresolved DMSC register expression: {expression}"
        ) from error


def parse_writes(text, fields, symbols):
    writes = []
    temporaries = {}
    for raw_line in text.splitlines():
        line = raw_line.strip()
        temporary = re.match(r"uint32_t (\$[A-Za-z0-9_]+) = (.+)", line)
        if temporary:
            temporaries[temporary.group(1)] = evaluate_value(
                temporary.group(2), fields, symbols, temporaries
            )
            continue
        marker = "system_reg_write("
        if marker not in line:
            continue
        body = line.split(marker, 1)[1]
        if ") __tailcall" in body:
            body = body.split(") __tailcall", 1)[0]
        else:
            body = body.rsplit(")", 1)[0]
        register_expression, value_expression = body.split(",", 1)
        writes.append(
            (
                evaluate_register(register_expression),
                evaluate_value(value_expression, fields, symbols, temporaries),
                value_expression.strip(),
            )
        )
    return writes


def render(image, noref_writes, ref_writes):
    if len(noref_writes) != 28 or len(ref_writes) != 100:
        raise ValueError(
            f"unexpected DMSC write counts: noref={len(noref_writes)} "
            f"ref={len(ref_writes)}"
        )

    digest = hashlib.sha256(image).hexdigest()
    lines = [
        "/* Generated by tools/t23_dmsc_image.py. */",
        f"/* Source tuning SHA256: {digest} */",
        "static const uint32_t regtrace_t23_dmsc_sc2336_startup[][2] = {",
    ]
    for group_name, writes in (("noref", noref_writes), ("ref", ref_writes)):
        lines.append(f"    /* tisp_dmsc_{group_name}_reg_cfg */")
        for register, value, expression in writes:
            lines.append(
                f"    {{ 0x{register:04x}U, 0x{value:08x}U }},"
                f" /* {expression} */"
            )
    lines.extend(
        [
            "    /* tisp_dmsc_all_reg_refresh commit */",
            "    { 0x499cU, 0x00000001U },",
            "};",
            "",
        ]
    )
    return "\n".join(lines)


def main():
    args = parse_args()
    cache = json.loads(args.mcp_cache.read_text())
    functions = cache["functions"]
    layout = parse_layout(functions[PARAMS_KEY]["decompiled_text"])
    image, fields = load_fields(args.tuning_bin, layout)
    symbols = parse_symbols(args.original_ko)
    noref_writes = parse_writes(functions[NOREF_KEY]["decompiled_text"], fields, symbols)
    ref_writes = parse_writes(functions[REF_KEY]["decompiled_text"], fields, symbols)
    output = render(image, noref_writes, ref_writes)
    if args.output:
        args.output.write_text(output)
    else:
        print(output, end="")


if __name__ == "__main__":
    main()
