# Computer Networks Lab

Laboratory programs for the Computer Networks course, covering network layering, data framing protocols, and error detection mechanisms.

All programs are written in **C**.

## Exercise List

| Exercise | Topic | Description |
|----------|-------|-------------|
| EX0 | Layering and Framing (DDCMP) | Network layer simulation with DNS hash table, IP/MAC resolution, and DDCMP byte-count framing |
| EX1 | Character Stuffing (BISYNC) | BISYNC character stuffing/destuffing with binary conversion, plus the full layering pipeline with DDCMP framing |
| EX2 | CRC Error Detection | Byte-count framing with CRC (Cyclic Redundancy Check) for error detection, modular sender-receiver design |

## Features

- **DNS Hash Table** — URL to IP/MAC address mapping using open-addressing hash table with CRUD operations
- **Network Layer Simulation** — Application, Transport, Network, and Data Link layer processing
- **BISYNC Framing** — Character stuffing and destuffing for control characters (DLE, STX, ETX, SOH, SYN)
- **DDCMP Framing** — Byte-count based framing with SYN synchronization and CRC validation
- **CRC Error Detection** — Polynomial division (mod-2) based error detection with optional error injection for testing

## Compilation

**EX0 and EX1 (single file):**
```bash
gcc LayeringandFraming.c -o output
./output
```

**EX1 — BISYNC (single file):**
```bash
gcc BISYNC.c -o output
./output
```

**EX2 (multi-file):**
```bash
gcc Main.c Sender.c Reciever.c -o output
./output
```

## Input Files

- `message.txt` / `message1.txt` — Text message files used as input payload for framing and transmission
