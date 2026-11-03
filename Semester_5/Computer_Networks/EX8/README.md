# EX8 · Reliable Data Transmission in C

**Exploring how acknowledgements, sequence numbers, and retransmissions turn unreliable delivery into ordered communication.**

This computer networks laboratory exercise implements Stop-and-Wait over UDP and deterministic Go-Back-N and Selective Repeat sliding-window simulations. Both make failure recovery visible: a frame is sent, an acknowledgement advances the sender, and a timeout triggers another attempt when delivery fails.

The central engineering trade-off is easy to observe: allowing multiple outstanding frames improves the opportunity for progress, but Go-Back-N may have to resend correctly transmitted frames after an earlier frame is lost.

## Project at a glance

| Protocol | Status | Execution model | What it demonstrates |
| --- | --- | --- | --- |
| Stop-and-Wait | Implemented | Separate C sender and receiver using UDP | Alternating sequence bits, actual receive timeouts, retries, duplicate suppression |
| Go-Back-N | Implemented | One C program simulating both endpoints | Sliding windows, cumulative ACKs, out-of-order discards, retransmission counts |
| Selective Repeat | Implemented | One C program simulating both endpoints | Individual ACKs, receiver buffering, selective retransmission |

## Repository layout

```text
EX8/
├── README.md
├── Stop-and-Wait/
│   ├── sender.c
│   └── reciever.c
├── Go-Back-N/
│   └── go_back_n.c
└── Selective-Repeat/
    └── selective_repeat.c
```

The receiver filename is currently spelled `reciever.c`; the build commands below match it.

## Build and run

### Requirements

- A C compiler supporting C11, such as GCC or Clang.
- macOS, Linux, or a POSIX environment such as WSL for the UDP programs.
- Two terminals for Stop-and-Wait; one terminal for either sliding-window simulation.

There are no third-party library dependencies. Run the following commands from the **EX8 directory**.

### Go-Back-N: quickest demonstration

```sh
cc -std=c11 -Wall -Wextra -Wpedantic Go-Back-N/go_back_n.c -o Go-Back-N/go_back_n
./Go-Back-N/go_back_n
```

Example configuration:

```text
Window size: 4
Number of frames: 10
Frame to lose or corrupt (0 for none): 3
Fault type (1 = lost, 2 = corrupted): 1
```

To reproduce this run without entering values interactively:

```sh
printf '4\n10\n3\n1\n' | ./Go-Back-N/go_back_n
```

### Selective Repeat

```sh
cc -std=c11 -Wall -Wextra -Wpedantic Selective-Repeat/selective_repeat.c -o Selective-Repeat/selective_repeat
./Selective-Repeat/selective_repeat
# Same inputs as Go-Back-N: window 4, ten frames, frame 3 lost once.
printf '4\n10\n3\n1\n' | ./Selective-Repeat/selective_repeat
```

ACK `k` acknowledges only frame `k`. The receiver buffers valid out-of-order frames and delivers them in order once the gap is filled. The sender advances its window only across consecutive acknowledged frames and fills available slots before simulating timer expiration. Only unacknowledged frames are retransmitted.

For the example above, frames 4–6 are buffered while frame 3 is missing. Only frame 3 is retransmitted, yielding 11 transmissions, one timeout, and 90.91% efficiency. With no fault, there are no retransmissions; with either configured one-time fault, there is exactly one. Frame-state arrays use `O(N)` memory and runtime is `O(N)` under this single-fault model.

### Stop-and-Wait: live UDP demonstration

Build both endpoints:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic Stop-and-Wait/reciever.c -o Stop-and-Wait/receiver
cc -std=c11 -Wall -Wextra -Wpedantic Stop-and-Wait/sender.c -o Stop-and-Wait/sender
```

Start the receiver in terminal 1:

```sh
./Stop-and-Wait/receiver
```

Start the sender in terminal 2:

```sh
./Stop-and-Wait/sender
```

The sender targets `127.0.0.1:8085`; the receiver binds UDP port `8085` on all local interfaces. Choose a mode, then enter messages:

| Mode | Sender action | Observable result |
| --- | --- | --- |
| `1` — Normal | Sends one message and waits for its ACK | Receiver accepts the frame; sequence bit alternates |
| `2` — Drop/retry | Marks the first attempt of each message for deliberate receiver-side discard | No ACK arrives; the sender retries after its 3-second receive timeout |
| `3` — Duplicate | Offers to resend a frame after its ACK arrives | Receiver discards the duplicate and returns its current ACK |

Type `exit` at the message prompt to stop the sender. Stop the receiver with `Ctrl+C`.

## How Stop-and-Wait works

The sender permits one outstanding frame. Each frame carries a sequence bit, alternating between `0` and `1`. The receiver maintains the bit it expects next.

1. The sender transmits a frame with its current sequence bit.
2. The receiver accepts the frame only if that bit matches `expected`.
3. After acceptance, the receiver toggles `expected` and sends it as the ACK value.
4. The sender advances only when the received ACK equals `seq ^ 1`.
5. A receive timeout or invalid ACK causes the same frame to be retried.

If an ACK is lost, the sender may transmit a frame that the receiver has already accepted. The unchanged sequence bit lets the receiver suppress duplicate delivery while sending the ACK again. The duplicate mode exposes this receiver behavior directly.

**ACK convention:** in this implementation, an ACK identifies the **next expected sequence bit**. For example, accepting frame bit `0` produces ACK `1`.

## How Go-Back-N works

The Go-Back-N program models a sender and receiver in one process. Frames are numbered from `1` through `N`, and the configured window size `W` limits the number of outstanding frames.

### State and invariants

| State | Meaning |
| --- | --- |
| `base` | Oldest frame not yet acknowledged |
| `next` | Next frame not yet sent for the first time |
| `expected` | Next frame the receiver will accept |

Outstanding frames occupy `[base, next)`, with `next - base <= W`. The receiver accepts only `expected`, so accepted frames remain contiguous and ordered. Delivery completes when `base > N`.

**ACK convention:** ACK `k` acknowledges every frame through `k`; ACK `0` means no frames have been accepted. This differs from the next-expected-bit convention used by the Stop-and-Wait program.

### Transmission and recovery

1. Send a wave of frames that fits inside the available window.
2. Process receiver responses: accept the next expected frame and discard any out-of-order frame.
3. Advance `base` using cumulative ACKs and fill newly available window slots.
4. If frames remain outstanding and no more frames can be sent, simulate expiration of the oldest frame's timer.
5. Retransmit **every outstanding frame**, beginning at `base`.

The chosen frame fails only on its first attempt. A lost frame produces no receiver response. A corrupted frame is discarded and produces an ACK for the last contiguous frame accepted. Later out-of-order frames produce duplicate ACKs; they do not advance the window or trigger fast retransmit.

Timeouts are logical events: the simulation advances immediately to the timeout rather than sleeping. ACKs are assumed reliable, and corruption is a configured event rather than an actual checksum calculation.

### Worked example: window 4, ten frames, frame 3 lost

| Stage | Frames sent | Receiver outcome | Sender outcome |
| --- | --- | --- | --- |
| Initial window | `1, 2, 3, 4` | Accepts 1–2; 3 is lost; discards 4 | ACK 2 moves the window base to 3 |
| Fill available slots | `5, 6` | Discards both while waiting for 3 | Outstanding frames are now 3–6 |
| Timeout recovery | `3, 4, 5, 6` | Accepts all four in order | ACKs advance the base to 7 |
| Complete delivery | `7, 8, 9, 10` | Accepts all remaining frames | Every frame is acknowledged |

Frames 5 and 6 are sent before the timeout because ACKs for 1 and 2 open two new window slots. This demonstrates a sliding window rather than independent fixed-size batches.

The program ends with:

```text
Transmission analysis
Frames successfully delivered: 10
Original transmissions: 10
Retransmissions: 4
Timeouts: 1
Total data-frame transmissions: 10 + 4 = 14
Transmission efficiency: 71.43%
```

## Transmission cost and complexity

For this simulation's timing model, let `N` be the frame count, `W` the window size, and `F` the frame that fails once.

```text
No fault:
    retransmissions R = 0

One lost or corrupted frame:
    retransmissions R = min(W, N - F + 1)

Total data-frame transmissions T = N + R
Transmission efficiency         = (N / T) × 100%
```

Every data-frame send attempt counts, including attempts that are lost, corrupted, or discarded. ACK messages are excluded. The efficiency metric measures useful delivered frames per send attempt; it is not measured network throughput or link utilization.

| N | W | Failure | Retransmissions | Total transmissions | Efficiency |
| --- | --- | --- | --- | --- | --- |
| 10 | 4 | None | 0 | 10 | 100.00% |
| 10 | 4 | Frame 3 | 4 | 14 | 71.43% |
| 10 | 4 | Frame 10 | 1 | 11 | 90.91% |
| 10 | 1 | Frame 3 | 1 | 11 | 90.91% |
| 10 | 20 | Frame 3 | 8 | 18 | 55.56% |

A larger window can increase the number of frames resent after a loss. Its benefit under real network latency is allowing multiple frames to remain in flight; this program does not measure that latency benefit. These counts depend on the documented single-fault and timeout assumptions, rather than being universal counts for every Go-Back-N network.

The simulator performs `O(N + R)` work and uses `O(1)` auxiliary memory. It tracks frame identifiers and counters rather than storing payloads or a retransmission buffer. With the single configured fault, `R <= N`, so runtime is also `O(N)`.

## Validation and reproducible checks

The original three C source files were compiled with `-std=c11 -Wall -Wextra -Wpedantic` without compiler warnings in the development environment.

The Go-Back-N implementation was also checked with a temporary automated harness across **840 configurations**: frame counts 1–12, window sizes 1, 2, 4, 8, and 16, every possible failed-frame position, and both loss and corruption. Checks confirmed ordered, exactly-once acceptance, the expected transmission totals, and timeout counts. Invalid-input recovery and early end-of-input were checked separately. This harness is not currently committed to the repository.

After building, use these commands for a quick regression walkthrough:

```sh
# No fault: 10 total transmissions, 0 timeouts.
printf '4\n10\n0\n' | ./Go-Back-N/go_back_n

# Loss: 14 total transmissions, 1 timeout.
printf '4\n10\n3\n1\n' | ./Go-Back-N/go_back_n

# Corruption: 14 total transmissions, 1 timeout.
printf '4\n10\n3\n2\n' | ./Go-Back-N/go_back_n

# Window size 1: 11 total transmissions, 1 timeout.
printf '1\n10\n3\n1\n' | ./Go-Back-N/go_back_n

# Last-frame loss: 11 total transmissions, 1 timeout.
printf '4\n10\n10\n1\n' | ./Go-Back-N/go_back_n
```

For Stop-and-Wait, run each interactive mode and observe acceptance, timeout recovery, and duplicate rejection in the two terminals. Its runtime behavior is not covered by the Go-Back-N harness.

## Design boundaries

These are focused laboratory implementations with deliberately small protocol models:

- **Go-Back-N:** deterministic one-time failure, reliable ACKs, no payload storage, no sequence-number wraparound, and no real network or elapsed-time measurement. Window size and frame count must be positive integers up to `INT_MAX`; a window larger than the frame count is accepted.
- **Stop-and-Wait:** one sender/receiver session, fixed port and timeout, fixed-size message storage, and a shared in-memory C struct layout sent directly over UDP. There is no portable wire encoding or checksum implementation. The sender retries indefinitely if the receiver never responds.
- **Selective Repeat:** uses the same single-fault, reliable-ACK, logical-timeout and non-wrapping frame-number assumptions as Go-Back-N. Receiver buffers store frame receipt state rather than payloads. For a future finite sequence-number implementation, the sequence space must be at least twice the window size.

## Engineering takeaways

- **A timeout signals missing confirmation.** It cannot, by itself, distinguish a lost data frame from a lost ACK.
- **Sequence numbers protect delivery semantics.** They let the receiver recognize retransmitted data and avoid delivering it twice.
- **Cumulative ACKs describe a contiguous prefix.** A later frame cannot advance the receiver past a missing earlier frame in this Go-Back-N model.
- **Recovery policy has a measurable cost.** Discarding out-of-order frames simplifies receiver state but increases retransmission work.
- **Simulation assumptions shape results.** Separating frame-count efficiency from throughput makes the reported measurements interpretable.

## Possible extensions

- Add configurable ACK loss, multiple failures, and seeded random fault injection.
- Introduce finite sequence-number spaces and test wraparound behavior.
- Build a socket-based Go-Back-N variant with explicit packet serialization and payload integrity checks.
- Commit an automated regression harness and add continuous integration.
- Measure throughput under controlled delay and loss to compare window sizes experimentally.
