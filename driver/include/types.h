#ifndef TYPES_H
#define TYPES_H

/* Type Definitions */

/* Platform Types */
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;

/* User-Defined Types */

/* Define enums first */
typedef enum e_type {
    ET_NONE = 0,
    ET_REL = 1,
    ET_EXEC = 2,
    ET_DYN = 3,
    ET_CORE = 4,
    ET_NUM = 5,
} e_type;

typedef enum e_machine {
    EM_NONE = 0,
    EM_M32 = 1,
    EM_SPARC = 2,
    EM_386 = 3,
    EM_68K = 4,
    EM_88K = 5,
    EM_860 = 7,
    EM_MIPS = 8,
    EM_S370 = 9,
    EM_MIPS_RS3_LE = 10,
    EM_PARISC = 15,
    EM_VPP500 = 17,
    EM_SPARC32PLUS = 18,
    EM_960 = 19,
    EM_PPC = 20,
    EM_PPC64 = 21,
    EM_S390 = 22,
    EM_V800 = 36,
    EM_FR20 = 37,
    EM_RH32 = 38,
    EM_RCE = 39,
    EM_ARM = 40,
    EM_FAKE_ALPHA = 41,
    EM_SH = 42,
    EM_SPARCV9 = 43,
    EM_TRICORE = 44,
    EM_ARC = 45,
    EM_H8_300 = 46,
    EM_H8_300H = 47,
    EM_H8S = 48,
    EM_H8_500 = 49,
    EM_IA_64 = 50,
    EM_MIPS_X = 51,
    EM_COLDFIRE = 52,
    EM_68HC12 = 53,
    EM_MMA = 54,
    EM_PCP = 55,
    EM_NCPU = 56,
    EM_NDR1 = 57,
    EM_STARCORE = 58,
    EM_ME16 = 59,
    EM_ST100 = 60,
    EM_TINYJ = 61,
    EM_X86_64 = 62,
    EM_PDSP = 63,
    EM_FX66 = 66,
    EM_ST9PLUS = 67,
    EM_ST7 = 68,
    EM_68HC16 = 69,
    EM_68HC11 = 70,
    EM_68HC08 = 71,
    EM_68HC05 = 72,
    EM_SVX = 73,
    EM_ST19 = 74,
    EM_VAX = 75,
    EM_CRIS = 76,
    EM_JAVELIN = 77,
    EM_FIREPATH = 78,
    EM_ZSP = 79,
    EM_MMIX = 80,
    EM_HUANY = 81,
    EM_PRISM = 82,
    EM_AVR = 83,
    EM_FR30 = 84,
    EM_D10V = 85,
    EM_D30V = 86,
    EM_V850 = 87,
    EM_M32R = 88,
    EM_MN10300 = 89,
    EM_MN10200 = 90,
    EM_PJ = 91,
    EM_OPENRISC = 92,
    EM_ARC_A5 = 93,
    EM_XTENSA = 94,
    EM_ALTERA_NIOS2 = 113,
    EM_AARCH64 = 183,
    EM_TILEPRO = 188,
    EM_MICROBLAZE = 189,
    EM_TILEGX = 191,
    EM_NUM = 192,
} e_machine;

typedef enum p_type {
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PT_SHLIB = 5,
    PT_PHDR = 6,
    PT_TLS = 7,
    PT_NUM = 8,
    PT_LOOS = 1610612736,
    PT_GNU_EH_FRAME = 1685382480,
    PT_GNU_STACK = 1685382481,
    PT_GNU_RELRO = 1685382482,
    PT_GNU_PROPERTY = 1685382483,
    PT_LOSUNW = 1879048186,
    PT_SUNWBSS = 1879048187,
    PT_SUNWSTACK = 1879048186,
    PT_MIPS_REGINFO = 1879048192,
    PT_MIPS_RTPROC = 1879048193,
    PT_MIPS_OPTIONS = 1879048194,
    PT_MIPS_ABIFLAGS = 1879048195,
} p_type;

typedef enum p_flags {
    PF_X = 1,
    PF_W = 2,
    PF_R = 4,
} p_flags;

typedef enum sh_type {
    SHT_NULL = 0,
    SHT_PROGBITS = 1,
    SHT_SYMTAB = 2,
    SHT_STRTAB = 3,
    SHT_RELA = 4,
    SHT_HASH = 5,
    SHT_DYNAMIC = 6,
    SHT_NOTE = 7,
    SHT_NOBITS = 8,
    SHT_REL = 9,
    SHT_SHLIB = 10,
    SHT_DYNSYM = 11,
    SHT_LOUSER = 2147483648,
    SHT_HIUSER = 4294967295,
    SHT_LOPROC = 1879048192,
    SHT_HIPROC = 2147483647,
} sh_type;

typedef enum sh_flags {
    SHF_WRITE = 1,
    SHF_ALLOC = 2,
    SHF_EXECINSTR = 4,
    SHF_MERGE = 16,
    SHF_STRINGS = 32,
    SHF_INFO_LINK = 64,
    SHF_LINK_ORDER = 128,
    SHF_OS_NONCONFORMING = 256,
    SHF_GROUP = 512,
    SHF_TLS = 1024,
    SHF_COMPRESSED = 2048,
    SHF_MASKOS = 267386880,
    SHF_MIPS_GPREL = 268435456,
} sh_flags;

/* Define Elf32_Ident struct before it's used */
typedef struct Elf32_Ident {
    char signature[0x4]; // offset: 0x0
    uint8_t file_class; // offset: 0x4
    uint8_t encoding; // offset: 0x5
    uint8_t version; // offset: 0x6
    uint8_t os; // offset: 0x7
    uint8_t abi_version; // offset: 0x8
    char pad[0x7]; // offset: 0x9
} Elf32_Ident;

/* Define structs that use the above types */
typedef struct Elf32_Header {
    struct Elf32_Ident ident; // offset: 0x0
    enum e_type type; // offset: 0x10
    enum e_machine machine; // offset: 0x12
    uint32_t version; // offset: 0x14
    void (*entry)(void); // offset: 0x18
    uint32_t program_header_offset; // offset: 0x1c
    uint32_t section_header_offset; // offset: 0x20
    uint32_t flags; // offset: 0x24
    uint16_t header_size; // offset: 0x28
    uint16_t program_header_size; // offset: 0x2a
    uint16_t program_header_count; // offset: 0x2c
    uint16_t section_header_size; // offset: 0x2e
    uint16_t section_header_count; // offset: 0x30
    uint16_t string_table; // offset: 0x32
} Elf32_Header;

typedef struct Elf32_ProgramHeader {
    enum p_type type; // offset: 0x0
    uint32_t offset; // offset: 0x4
    uint32_t virtual_address; // offset: 0x8
    uint32_t physical_address; // offset: 0xc
    uint32_t file_size; // offset: 0x10
    uint32_t memory_size; // offset: 0x14
    enum p_flags flags; // offset: 0x18
    uint32_t align; // offset: 0x1c
} Elf32_ProgramHeader;

typedef struct Elf32_SectionHeader {
    uint32_t name; // offset: 0x0
    enum sh_type type; // offset: 0x4
    enum sh_flags flags; // offset: 0x8
    uint32_t address; // offset: 0xc
    uint32_t offset; // offset: 0x10
    uint32_t size; // offset: 0x14
    uint32_t link; // offset: 0x18
    uint32_t info; // offset: 0x1c
    uint32_t align; // offset: 0x20
    uint32_t entry_size; // offset: 0x24
} Elf32_SectionHeader;

// Type: va_list
typedef void* va_list;


#endif /* TYPES_H */
