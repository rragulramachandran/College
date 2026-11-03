#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long long read_number(const char *prompt, long long min, long long max)
{
    char line[128], *end;
    for (;;) {
        printf("%s", prompt);
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin)) {
            fputs("\nInput ended before configuration was complete.\n", stderr);
            exit(EXIT_FAILURE);
        }
        /* Reject overlong lines instead of treating their suffix as new input. */
        if (!strchr(line, '\n') && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            puts("Input is too long.");
            continue;
        }
        errno = 0;
        long long value = strtoll(line, &end, 10);
        int parsed = end != line;
        while (isspace((unsigned char)*end))
            ++end;
        if (parsed && errno != ERANGE && *end == '\0' &&
            value >= min && value <= max)
            return value;
        printf("Enter an integer from %lld to %lld.\n", min, max);
    }
}

int main(void)
{
    puts("Selective Repeat Sliding Window Simulation (frames numbered from 1)");
    long long window = read_number("Window size: ", 1, INT_MAX);
    long long total = read_number("Number of frames: ", 1, INT_MAX);
    long long fault = read_number("Frame to lose or corrupt (0 for none): ", 0, total);
    long long mode = fault ? read_number("Fault type (1 = lost, 2 = corrupted): ", 1, 2) : 0;
    long long base = 1, next = 1, expected = 1;
    long long transmissions = 0, retransmissions = 0, timeouts = 0;
    int injected = 0, retry = 0;
    unsigned char *acked = calloc((size_t)total, sizeof *acked);
    unsigned char *received = calloc((size_t)total, sizeof *received);
    if (!acked || !received) {
        fputs("Unable to allocate frame state.\n", stderr);
        free(acked);
        free(received);
        return EXIT_FAILURE;
    }

    puts("\nACK k acknowledges only frame k; out-of-order frames are buffered.");
    /* Like Go-Back-N, use logical timeouts, reliable ACKs and no sequence wrap. */
    while (base <= total) {
        long long start, stop;
        if (retry) {
            start = base;
            stop = next;
        } else {
            start = next;
            stop = base + window;
            if (stop > total + 1)
                stop = total + 1;
            next = stop;
        }

        /* Each unacknowledged frame has its own logical timer. */
        for (long long frame = start; frame < stop; ++frame) {
            if (acked[frame - 1])
                continue;
            if (retry) {
                ++timeouts;
                printf("TIMEOUT for frame %lld: retransmit only frame %lld\n", frame, frame);
                ++retransmissions;
            }
            printf("%s frame %lld\n", retry ? "RETRANSMIT" : "TRANSMIT", frame);
            ++transmissions;
        }
        for (long long frame = start; frame < stop; ++frame) {
            if (acked[frame - 1])
                continue;
            if (frame == fault && !injected) {
                injected = 1;
                printf("  Frame %lld %s\n", frame,
                       mode == 1 ? "LOST in transit" : "CORRUPTED: receiver discards it");
                continue;
            }
            if (frame >= expected && frame < expected + window) {
                received[frame - 1] = 1;
                printf("  Receiver %s frame %lld\n",
                       frame == expected ? "ACCEPTS" : "BUFFERS out-of-order", frame);
                /* Deliver the contiguous prefix, including previously buffered frames. */
                while (expected <= total && received[expected - 1]) {
                    printf("  Receiver DELIVERS frame %lld\n", expected);
                    ++expected;
                }
            } else if (frame < expected) {
                printf("  Receiver DISCARDS duplicate frame %lld\n", frame);
            } else {
                printf("  Receiver DISCARDS frame %lld outside its window\n", frame);
                continue;
            }
            acked[frame - 1] = 1;
            printf("  ACK %lld received: only frame %lld acknowledged\n", frame, frame);
            /* A later ACK cannot slide the sender window past a missing ACK. */
            while (base <= total && acked[base - 1])
                ++base;
        }
        retry = 0;
        /* Fill newly opened slots before advancing to outstanding timers. */
        if (base < next && (next == base + window || next > total))
            retry = 1;
    }

    puts("\nTransmission analysis");
    printf("Frames successfully delivered: %lld\n", expected - 1);
    printf("Original transmissions: %lld\n", total);
    printf("Retransmissions: %lld\n", retransmissions);
    printf("Timeouts: %lld\n", timeouts);
    printf("Total data-frame transmissions: %lld + %lld = %lld\n",
           total, retransmissions, transmissions);
    printf("Transmission efficiency: %.2f%%\n", 100.0 * total / transmissions);
    free(acked);
    free(received);
    return EXIT_SUCCESS;
}
