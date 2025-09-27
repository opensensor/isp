# ISP Driver Architecture

## Overview

This document describes the ISP (Image Signal Processor) driver architecture for the T31 platform, based on Binary Ninja MCP decompilation analysis of the reference driver.

## Key Discovery: Stateless Core Device Architecture

Through detailed Binary Ninja MCP decompilation, we discovered that the reference driver uses a **completely different state management architecture** than initially implemented:

### Architecture Transformation

#### ❌ OLD (Incorrect) Architecture:
```
┌─────────────────┐    ┌─────────────────┐
│   Core Device   │    │   VIC Device    │
│   (STATEFUL)    │    │   (STATEFUL)    │
├─────────────────┤    ├─────────────────┤
│ • state: 1-4    │    │ • state: 1-4    │
│ • State machine │    │ • State machine │
│ • Conflicts!    │    │ • Conflicts!    │
└─────────────────┘    └─────────────────┘
```

#### ✅ NEW (Correct) Architecture:
```
┌─────────────────────────────────────────────────────────────┐
│                    ISP DEVICE (MAIN)                       │
├─────────────────────────────────────────────────────────────┤
│ • pipeline_state: ISP_PIPELINE_IDLE/READY/ACTIVE/STREAMING │
│ • Coordinates all subdevices                               │
│ • Global state management                                  │
└─────────────────────────────────────────────────────────────┘
                                │
                ┌───────────────┼───────────────┐
                │               │               │
┌───────────────▼───┐  ┌────────▼────────┐  ┌──▼──────────┐
│  CORE DEVICE      │  │  VIC DEVICE     │  │  OTHER      │
│  (STATELESS)      │  │  (STATEFUL)     │  │  SUBDEVS    │
├───────────────────┤  ├─────────────────┤  ├─────────────┤
│ • Registers       │  │ • state: 1-4    │  │ • Various   │
│ • IRQ handling    │  │ • State machine │  │ • States    │
│ • Clocks          │  │ • Frame counters│  │ • Functions │
│ • Operations      │  │ • Hardware ctrl │  │             │
│ • streaming flag  │  │ • THE AUTHORITY │  │             │
│ • NO STATE!       │  │   FOR STREAMING │  │             │
└───────────────────┘  └─────────────────┘  └─────────────┘
```

## State Management Hierarchy

### 1. ISP Device (Global Coordinator)
- **pipeline_state**: Overall system state
- **Values**: `ISP_PIPELINE_IDLE`, `ISP_PIPELINE_READY`, `ISP_PIPELINE_ACTIVE`, `ISP_PIPELINE_STREAMING`
- **Purpose**: Coordinates all subdevices and manages global pipeline state

### 2. VIC Device (Streaming Authority)
- **state**: Streaming state machine
- **Values**: 
  - `1` = VIC_INIT (Initialized)
  - `2` = VIC_READY (Ready for configuration)
  - `3` = VIC_ACTIVE (Configured, ready to stream)
  - `4` = VIC_STREAMING (Actively streaming)
- **Purpose**: **THE SINGLE SOURCE OF TRUTH** for streaming state

### 3. Core Device (Stateless Operations)
- **NO STATE ATTRIBUTE** - Completely stateless
- **Purpose**: Hardware operations, register access, IRQ handling, clock management
- **streaming flag**: Simple boolean for streaming status (not state machine)

## VIC State Machine Transitions

```
    ┌─────────┐    ispcore_activate_module    ┌─────────┐
    │ State 1 │ ──────────────────────────────▶ State 2 │
    │  INIT   │                               │  READY  │
    └─────────┘                               └─────────┘
                                                   │
                                                   │ ispcore_core_ops_init
                                                   ▼
    ┌─────────┐    ispcore_video_s_stream     ┌─────────┐
    │ State 4 │ ◀──────────────────────────── │ State 3 │
    │STREAMING│                               │ ACTIVE  │
    └─────────┘                               └─────────┘
```

## Function Responsibilities

### ispcore_activate_module
- **Transition**: VIC State 1 → 2
- **Purpose**: Clock setup, basic initialization
- **Binary Ninja**: Matches exact reference implementation

### ispcore_core_ops_init  
- **Transition**: VIC State 2 → 3
- **Purpose**: Hardware configuration, register setup
- **Binary Ninja**: Uses VIC state at offset `0xe8`

### ispcore_video_s_stream
- **Transition**: VIC State 3 ↔ 4
- **Purpose**: Enable/disable streaming
- **Binary Ninja**: All state checks use VIC device

## Binary Ninja MCP Compatibility

All ISP functions access VIC state at memory offset `0xe8`:
- `ispcore_core_ops_init` → `*($s0 + 0xe8)` = VIC state
- `ispcore_slake_module` → `*($s0 + 0xe8)` = VIC state  
- `ispcore_video_s_stream` → `*($s0 + 0xe8)` = VIC state
- `ispcore_activate_module` → `*($s0 + 0xe8)` = VIC state

## Key Benefits

1. **No State Conflicts**: Single source of truth eliminates timing issues
2. **Hardware Compatibility**: Matches T31 reference driver exactly
3. **Proper Separation**: Core handles hardware, VIC handles state
4. **Binary Ninja Compliance**: Exact offset and access patterns
5. **Maintainability**: Clear responsibilities and state flow

## Implementation Notes

- Core device has **NO state attribute** in structure
- All state management functions removed from core device
- VIC device is the **authoritative source** for streaming state
- ISP device coordinates global pipeline state
- Binary Ninja decompilation patterns followed exactly
