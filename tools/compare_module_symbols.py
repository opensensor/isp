#!/usr/bin/env python3
"""Compare live object values from two Linux module core-memory dumps."""

import argparse
import re
import struct

from elftools.elf.elffile import ELFFile


def parse_section_base(value):
    name, offset = value.split("=", 1)
    return name, int(offset, 0)


def load_objects(elf_path, memory_path, section_bases):
    with open(memory_path, "rb") as memory_file:
        memory = memory_file.read()

    objects = {}
    with open(elf_path, "rb") as elf_file:
        elf = ELFFile(elf_file)
        symbols = elf.get_section_by_name(".symtab")
        if symbols is None:
            raise ValueError(f"{elf_path}: no .symtab")

        for symbol in symbols.iter_symbols():
            if symbol["st_info"]["type"] != "STT_OBJECT" or not symbol.name:
                continue
            section_index = symbol["st_shndx"]
            if not isinstance(section_index, int):
                continue
            section_name = elf.get_section(section_index).name
            if section_name not in section_bases:
                continue
            size = symbol["st_size"]
            memory_offset = section_bases[section_name] + symbol["st_value"]
            end = memory_offset + size
            if end > len(memory):
                raise ValueError(
                    f"{elf_path}: {symbol.name} extends past memory dump "
                    f"(0x{memory_offset:x}+0x{size:x} > 0x{len(memory):x})"
                )
            objects[symbol.name] = (
                section_name,
                memory_offset,
                memory[memory_offset:end],
            )
    return objects


def format_values(data, limit):
    values = []
    full_words = len(data) // 4
    for (value,) in struct.iter_unpack("<I", data[: full_words * 4]):
        values.append(f"0x{value:x}")
        if len(values) == limit:
            break
    suffix = ",..." if full_words > limit else ""
    if len(data) % 4 and len(values) < limit:
        values.append(data[full_words * 4 :].hex())
    return "{" + ",".join(values) + suffix + "}"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--left-elf", required=True)
    parser.add_argument("--left-memory", required=True)
    parser.add_argument("--left-section", action="append", required=True)
    parser.add_argument("--right-elf", required=True)
    parser.add_argument("--right-memory", required=True)
    parser.add_argument("--right-section", action="append", required=True)
    parser.add_argument("--match", default=".*")
    parser.add_argument("--values", type=int, default=12)
    args = parser.parse_args()

    left_bases = dict(parse_section_base(value) for value in args.left_section)
    right_bases = dict(parse_section_base(value) for value in args.right_section)
    left = load_objects(args.left_elf, args.left_memory, left_bases)
    right = load_objects(args.right_elf, args.right_memory, right_bases)
    pattern = re.compile(args.match)

    compared = equal = different = size_mismatch = 0
    for name in sorted(set(left) & set(right)):
        if not pattern.search(name):
            continue
        left_section, left_offset, left_data = left[name]
        right_section, right_offset, right_data = right[name]
        compared += 1
        if len(left_data) != len(right_data):
            size_mismatch += 1
            print(
                f"SIZE {name}: left={len(left_data)} right={len(right_data)} "
                f"left@{left_section}+0x{left_offset-left_bases[left_section]:x} "
                f"right@{right_section}+0x{right_offset-right_bases[right_section]:x}"
            )
            print(f"  left  {format_values(left_data, args.values)}")
            print(f"  right {format_values(right_data, args.values)}")
        elif left_data == right_data:
            equal += 1
        else:
            different += 1
            print(
                f"DIFF {name} size={len(left_data)} "
                f"left@0x{left_offset:x} right@0x{right_offset:x}"
            )
            print(f"  left  {format_values(left_data, args.values)}")
            print(f"  right {format_values(right_data, args.values)}")

    print(
        f"SUMMARY compared={compared} equal={equal} different={different} "
        f"size_mismatch={size_mismatch}"
    )


if __name__ == "__main__":
    main()
