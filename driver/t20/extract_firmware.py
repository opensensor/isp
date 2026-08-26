#!/usr/bin/env python3
"""Bootstrap the proprietary firmware candidates from a regtrace monolith.

The checked-in output contains reviewed integration repairs after extraction.
This tool therefore refuses to replace an existing output unless --force is
explicitly supplied.
"""

import argparse
import pathlib
import re
import subprocess


CANDIDATE = re.compile(
    r"(?m)^/\* WHOLE_DRIVER_CANDIDATE .*? original=(.*?) \*/$"
)


def archive_functions(nm: str, archive: pathlib.Path) -> set[str]:
    output = subprocess.check_output(
        [nm, "--defined-only", str(archive)], text=True
    )
    functions = set()
    for line in output.splitlines():
        fields = line.split()
        if len(fields) >= 3 and fields[-2] in {"T", "t"}:
            functions.add(fields[-1])
    return functions


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("source", type=pathlib.Path)
    parser.add_argument("output", type=pathlib.Path)
    parser.add_argument("--archive", required=True, type=pathlib.Path)
    parser.add_argument("--nm", default="nm")
    parser.add_argument("--exclude", action="append", default=[])
    parser.add_argument("--force", action="store_true")
    args = parser.parse_args()

    if args.output.exists() and not args.force:
        raise SystemExit(
            f"refusing to overwrite reviewed output {args.output}; "
            "pass --force only when intentionally regenerating the baseline"
        )

    keep = archive_functions(args.nm, args.archive) - set(args.exclude)
    source = args.source.read_text()

    # The one-off repair inserted source-backed VIC helpers ahead of the
    # generated candidate stream. The canonical build supplies those helpers
    # from the GPL SDK translation units instead.
    sdk_start = source.index("static int isp_vic_init_clk(")
    sdk_end = source.index("static const char LC10[]", sdk_start)
    source = (
        source[:sdk_start]
        + "/* T20 SDK-owned support functions compile in sdk/*.o. */\n"
        + source[sdk_end:]
    )

    # SDK objects own their static V4L2 operation tables. The generated
    # relocation patcher is both unused and invalid once those tables live in
    # their original translation units.
    patch_start = source.index("/* WHOLE_DRIVER_RELOCATED_DATA_PATCHES */")
    patch_end_match = CANDIDATE.search(source, patch_start)
    if patch_end_match is None:
        raise SystemExit("relocation patch block has no following candidate")
    source = (
        source[:patch_start]
        + "/* SDK and firmware objects retain their native static initializers. */\n"
        + source[patch_end_match.start() :]
    )

    # regtrace conflated the address of the archive's 60-byte `stab` object
    # with a scalar pointer. Preserve the OEM storage and make its typed field
    # accesses explicit.
    source = source.replace(
        "static uintptr_t stab;", "static unsigned char stab[60];"
    )
    source = re.sub(
        r"\bstab\.(b\d+|h56|h58)\b",
        r"((struct stab_t *)stab)->\1",
        source,
    )
    source = source.replace(
        "stab.field_56", "((struct stab_t *)stab)->h56"
    )
    source = source.replace(
        "stab.field_58", "((struct stab_t *)stab)->h58"
    )
    source = source.replace(
        "stab.f4", "(*(int32_t *)(stab + 16))"
    )
    source = source.replace("v0 = stab;", "v0 = stab[0];")
    source = source.replace(
        "stab = (uint32_t *)arg2;", "stab[0] = (uint8_t)arg2;"
    )

    # A cast on the induction variable belongs on its uses, not on the lvalue
    # side of an increment. This is a mechanical decompiler repair.
    source = re.sub(
        r"\(uintptr_t\)([A-Za-z_][A-Za-z0-9_]*)\s*\+=",
        r"\1 +=",
        source,
    )
    source = source.replace(
        "(uintptr_t)dev->flags |=", "dev->flags |="
    )

    matches = list(CANDIDATE.finditer(source))
    if not matches:
        raise SystemExit("no WHOLE_DRIVER_CANDIDATE sections found")

    pieces = [source[: matches[0].start()]]
    selected = []
    for index, match in enumerate(matches):
        end = matches[index + 1].start() if index + 1 < len(matches) else len(source)
        name = match.group(1)
        if name in keep:
            pieces.append(source[match.start() : end])
            selected.append(name)

    banner = (
        "/* Firmware-only extraction: GPL SDK and shared bodies are separate objects. */\n"
        f"/* Selected {len(selected)} archive-backed function candidates. */\n"
    )
    args.output.write_text(banner + "".join(pieces))
    print(f"selected {len(selected)} archive-backed functions")


if __name__ == "__main__":
    main()
