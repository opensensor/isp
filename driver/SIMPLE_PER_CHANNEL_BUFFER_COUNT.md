# Simple Per-Channel Buffer Count Solution

## The Simple Truth

**Every channel sends 0x3000008 event with its buffer count. VIC stores Channel 0's count for hardware, ignores others.**

## The Fix

### 1. Pass Channel ID in Event

**tx-isp-module.c** - REQBUFS handler:

```c
/* Pass both channel ID and buffer count */
struct {
    int32_t channel_id;
    int32_t buffer_count;
} event_data = {
    .channel_id = channel,
    .buffer_count = reqbuf.count
};

tx_isp_send_event_to_remote(&vic_dev->sd, 0x3000008, &event_data);
```

### 2. VIC Stores Only Channel 0's Count

**tx_isp_vic.c** - 0x3000008 event handler:

```c
channel_id = event_data->channel_id;
buffer_count = event_data->buffer_count;

if (channel_id == 0) {
    /* Channel 0 controls VIC hardware */
    vic_dev->active_buffer_count = (buffer_count > 5) ? 5 : buffer_count;
} else {
    /* Channel 1+ are software-only, don't update VIC hardware count */
}
```

## How It Works

```
Channel 0 REQBUFS(count=4)
  ↓
Send 0x3000008 event: {channel_id=0, buffer_count=4}
  ↓
VIC: channel_id==0, so set vic->active_buffer_count = 4 ✅
  ↓
Channel 1 REQBUFS(count=2)
  ↓
Send 0x3000008 event: {channel_id=1, buffer_count=2}
  ↓
VIC: channel_id==1, so DON'T update vic->active_buffer_count ✅
  ↓
vic->active_buffer_count = 4 (unchanged) ✅
  ↓
STREAMON writes VIC[0x300] = 0x80040020 (4 buffers) ✅
```

## Why This Works

- **Channel 0**: Main video stream, controls VIC hardware
- **Channel 1+**: Software-only streams, don't control VIC hardware
- **VIC hardware**: Only cares about Channel 0's buffer count
- **Each channel**: Stores its own buffer count in channel state

## Files Modified

1. **driver/tx-isp-module.c** lines 2953-2982:
   - Pass struct with `{channel_id, buffer_count}` instead of just buffer count

2. **driver/tx_isp_vic.c** lines 1642-1687:
   - Extract channel_id from event data
   - Only update `vic->active_buffer_count` if channel_id == 0

## Expected Logs

```
*** REQBUFS: Channel 0 sending 0x3000008 event with buffer_count=4 ***
*** vic_core_ops_ioctl: 0x3000008 - Channel 0 buffer allocation: count=4 ***
*** vic_core_ops_ioctl: Channel 0 - VIC active_buffer_count set to 4 ***

*** REQBUFS: Channel 1 sending 0x3000008 event with buffer_count=2 ***
*** vic_core_ops_ioctl: 0x3000008 - Channel 1 buffer allocation: count=2 ***
*** vic_core_ops_ioctl: Channel 1 - VIC active_buffer_count unchanged (4) - software-only ***
```

## Success Criteria

✅ Channel 0 sets `vic->active_buffer_count = 4`
✅ Channel 1 does NOT modify `vic->active_buffer_count`
✅ `vic->active_buffer_count` remains 4 throughout streaming
✅ VIC register 0x300 = 0x80040020 (4 buffers)
✅ No control limit errors
✅ Continuous interrupts

Done! Simple and clean. 🎉

