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
    puts("Go-Back-N Sliding Window Simulation (frames numbered from 1)");
    long long window = read_number("Window size: ", 1, INT_MAX);
    long long total = read_number("Number of frames: ", 1, INT_MAX);
    long long fault = read_number("Frame to lose or corrupt (0 for none): ", 0, total);
    long long mode = fault ? read_number("Fault type (1 = lost, 2 = corrupted): ", 1, 2) : 0;
    long long base = 1, next = 1, expected = 1;
    long long transmissions = 0, retransmissions = 0, timeouts = 0;
    int injected = 0, retry = 0;

    puts("\nACK k acknowledges every frame through k; ACK 0 acknowledges none.");
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

        /* Send the available window before processing this wave's ACKs. */
        for (long long frame = start; frame < stop; ++frame) {
            printf("%s frame %lld\n", retry ? "RETRANSMIT" : "TRANSMIT", frame);
            ++transmissions;
            if (retry)
                ++retransmissions;
        }
        for (long long frame = start; frame < stop; ++frame) {
            if (frame == fault && !injected) {
                injected = 1;
                printf("  Frame %lld %s\n", frame,
                       mode == 1 ? "LOST in transit" : "CORRUPTED: receiver discards it");
                if (mode == 1)
                    continue;
            } else if (frame == expected) {
                printf("  Receiver ACCEPTS frame %lld\n", frame);
                ++expected;
            } else {
                printf("  Receiver DISCARDS out-of-order frame %lld (expects %lld)\n",
                       frame, expected);
            }
            long long ack = expected - 1;
            if (ack >= base) {
                printf("  ACK %lld received: frames through %lld acknowledged\n", ack, ack);
                base = ack + 1;
            } else {
                printf("  Duplicate ACK %lld received: window does not advance\n", ack);
            }
        }
        retry = 0;
        /* Slide and fill any newly available slots before the timer expires. */
        if (base < next && (next == base + window || next > total)) {
            ++timeouts;
            printf("TIMEOUT for frame %lld: retransmit outstanding frames %lld through %lld\n",
                   base, base, next - 1);
            retry = 1;
        }
    }

    puts("\nTransmission analysis");
    printf("Frames successfully delivered: %lld\n", expected - 1);
    printf("Original transmissions: %lld\n", total);
    printf("Retransmissions: %lld\n", retransmissions);
    printf("Timeouts: %lld\n", timeouts);
    printf("Total data-frame transmissions: %lld + %lld = %lld\n",
           total, retransmissions, transmissions);
    printf("Transmission efficiency: %.2f%%\n", 100.0 * total / transmissions);
    return EXIT_SUCCESS;
}
