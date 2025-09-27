# Reference Driver Rules

## Core Principle
**ONLY implement functionality that exists in the reference driver as analyzed through Binary Ninja decompilation.**

## Strict Rules

### ❌ NEVER ADD:
1. **Recovery/Error Handling Code** - If it's not in the reference driver, don't add it
   - No VIC control limit error recovery
   - No CSI PHY register protection/restoration
   - No automatic error correction mechanisms
   - No custom interrupt handling beyond what reference driver does

2. **Custom Register Sequences** - Only use exact Binary Ninja decompiled sequences
   - No custom register initialization
   - No additional register writes "for safety"
   - No register validation or correction code

3. **Enhanced Functionality** - Don't improve upon the reference driver
   - No dynamic sensor dimension detection beyond reference
   - No additional error logging beyond reference
   - No performance optimizations not in reference
   - No additional safety checks

4. **Protective Mechanisms** - Don't add safeguards not in reference
   - No register backup/restore
   - No state validation
   - No timeout handling beyond reference
   - No fallback mechanisms

### ✅ ONLY ADD:
1. **Exact Binary Ninja Implementations** - Copy decompiled code exactly
   - Use exact register addresses from Binary Ninja
   - Use exact register values from Binary Ninja
   - Use exact sequence timing from Binary Ninja

2. **Missing Hardware Connections** - Only if proven missing in our driver
   - ISP pipeline enable registers (0x800, 0x804, 0x1c) - PROVEN missing
   - Interrupt enable registers - PROVEN missing
   - Hardware connection registers - PROVEN missing

3. **Debug Logging** - Only for understanding, not for fixing
   - Log register values to match reference behavior
   - Log interrupt activity to verify correct operation
   - Log state transitions to match reference

### 🔍 Validation Process:
Before adding ANY code:
1. **Prove it exists in reference driver** - Find it in Binary Ninja decompilation
2. **Prove it's missing in our driver** - Verify we don't already have equivalent
3. **Implement exactly as decompiled** - No modifications or improvements
4. **Test that it matches reference behavior** - Verify identical operation

### 🚨 Red Flags - STOP if you're about to:
- Add error recovery that reference driver doesn't have
- "Fix" something the reference driver doesn't fix
- Add register writes not in Binary Ninja decompilation
- Implement "better" logic than reference driver
- Add protective code not in reference driver

### 💡 Remember:
**The reference driver works. Our job is to match it exactly, not improve it.**

If the reference driver has bugs or limitations, we replicate those bugs and limitations.
If the reference driver doesn't handle an error case, we don't handle it either.
If the reference driver has timing issues, we replicate the same timing.

**When in doubt, do LESS, not more.**
