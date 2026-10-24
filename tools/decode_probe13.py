#!/usr/bin/env python3
"""Read-only decoder for the diagnostic CACHE mmap file.

Reason: validate committed lengths/checksums and extract bounded boot slots.
Files: explicitly named new output directory only; input is never modified.
Verify: actual AArch64 persistence/error selftests and corrupted-frame rejection.
"""
import argparse
import hashlib
import json
import struct
from pathlib import Path

SLOT_SIZE = 1024 * 1024
SLOT_COUNT = 32
SLOT_MAGIC = 0x31544F4C53333150
FRAME_MAGIC = 0x1331CAFE


def checksum(data):
    value = 14695981039346656037
    for byte in data:
        value = ((value ^ byte) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return value


def decode(data):
    if len(data) != 4096 + SLOT_SIZE * SLOT_COUNT:
        raise ValueError('unexpected mmap file size')
    slots = []
    for index in range(SLOT_COUNT):
        block = memoryview(data)[4096 + index*SLOT_SIZE:4096 + (index+1)*SLOT_SIZE]
        values = struct.unpack_from('<4Q5q3Q', block)
        magic, generation, used, frames = values[:4]
        if magic == 0:
            continue
        if magic != SLOT_MAGIC or not 4096 <= used <= SLOT_SIZE:
            raise ValueError(f'invalid slot header {index}')
        position = 4096
        records = []
        while position < used:
            if position + 32 > used:
                raise ValueError(f'truncated frame header slot {index}')
            tag, length, ms, expected = struct.unpack_from('<4Q', block, position)
            if tag != FRAME_MAGIC or length > 65536 or position + 32 + length > used:
                raise ValueError(f'invalid frame length/tag slot {index} offset {position}')
            payload = bytes(block[position+32:position+32+length])
            if checksum(payload) != expected:
                raise ValueError(f'checksum mismatch slot {index} offset {position}')
            records.append((ms, payload))
            position += (32 + length + 7) & ~7
        if position != used or len(records) != frames:
            raise ValueError(f'frame-count/length mismatch slot {index}')
        slots.append({
            'slot': index, 'generation': generation, 'bytes_used': used,
            'frames': frames, 'last_msync_rc': values[4], 'last_fsync_rc': values[5],
            'last_vfs_write_rc': values[6], 'last_kmsg_read_rc': values[7],
            'last_private_kmsg_relabel_rc': values[8], 'dropped_frames': values[9],
            'filtered_kmsg_records': values[10], 'last_heartbeat_ms': values[11],
            'records': records,
        })
    return sorted(slots, key=lambda s: s['generation'])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('input', type=Path)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    data = args.input.read_bytes()
    slots = decode(data)
    args.output.mkdir(parents=True, exist_ok=False)
    for slot in slots:
        records = slot.pop('records')
        text = b''.join(b'\n[ms=' + str(ms).encode() + b'] ' + payload
                        for ms, payload in records)
        filename = f"boot-{slot['generation']:04d}.txt"
        (args.output / filename).write_bytes(text.replace(b'\x00', b' '))
        slot['text_file'] = filename
    result = {'result': 'PASS', 'input_sha256': hashlib.sha256(data).hexdigest(),
              'input_bytes': len(data), 'slots': slots}
    (args.output / 'summary.json').write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
