#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#include "Sender.h"
#include "Reciever.h"

#define TABLE_SIZE 20
#define MAX_MESSAGE_LEN 1000
#define MAX_BITS 8000
#define MAX_PACKETS 600
#define PACKET_BITS 16

#define SLOT_EMPTY 0
#define SLOT_OCCUPIED 1
#define SLOT_DELETED -1

// ---------- HASH TABLE ----------

typedef struct {
    char url[100];
    char ip[16];
    char mac[18];
    int occupied;
} HashEntry;

typedef struct { int src, dest; } Ports;

HashEntry table[TABLE_SIZE];

unsigned int hashFunc(char url[]) {
    unsigned int sum = 0;
    for (int i = 0; url[i] != '\0'; i++) sum += (unsigned char)url[i];
    return sum % TABLE_SIZE;
}

void initTable(void) {
    for (int i = 0; i < TABLE_SIZE; i++) table[i].occupied = SLOT_EMPTY;
}

int insert(char url[], char ip[], char mac[]) {
    unsigned int idx = hashFunc(url);
    for (int probe = 0; probe < TABLE_SIZE; probe++) {
        unsigned int i = (idx + probe) % TABLE_SIZE;
        if (table[i].occupied != SLOT_OCCUPIED || strcmp(table[i].url, url) == 0) {
            strcpy(table[i].url, url);
            strcpy(table[i].ip, ip);
            strcpy(table[i].mac, mac);
            table[i].occupied = SLOT_OCCUPIED;
            return 0;
        }
    }
    return -1;
}

int lookup(char url[]) {
    unsigned int idx = hashFunc(url);
    for (int probe = 0; probe < TABLE_SIZE; probe++) {
        unsigned int i = (idx + probe) % TABLE_SIZE;
        if (table[i].occupied == SLOT_OCCUPIED && strcmp(table[i].url, url) == 0) return i;
        if (table[i].occupied == SLOT_EMPTY) break;
    }
    return -1;
}

int updateEntry(char url[], char newIp[], char newMac[]) {
    unsigned int idx = hashFunc(url);
    for (int probe = 0; probe < TABLE_SIZE; probe++) {
        unsigned int i = (idx + probe) % TABLE_SIZE;
        if (table[i].occupied == SLOT_OCCUPIED && strcmp(table[i].url, url) == 0) {
            strcpy(table[i].ip, newIp);
            strcpy(table[i].mac, newMac);
            return 0;
        }
        if (table[i].occupied == SLOT_EMPTY) break;
    }
    return -1;
}

int deleteEntry(char url[]) {
    unsigned int idx = hashFunc(url);
    for (int probe = 0; probe < TABLE_SIZE; probe++) {
        unsigned int i = (idx + probe) % TABLE_SIZE;
        if (table[i].occupied == SLOT_OCCUPIED && strcmp(table[i].url, url) == 0) {
            table[i].occupied = SLOT_DELETED;
            return 0;
        }
        if (table[i].occupied == SLOT_EMPTY) break;
    }
    return -1;
}

void preloadDomains(void) {
    insert("www.mepcoeng.ac.in", "192.168.100.1", "00:1A:2B:3C:4D:5E");
    insert("www.google.com", "142.250.10.1", "AA:11:22:33:44:55");
    insert("www.youtube.com", "142.250.10.2", "AA:11:22:33:44:66");
    insert("www.facebook.com", "157.240.1.1", "BB:22:33:44:55:66");
    insert("www.amazon.com", "205.251.1.1", "CC:33:44:55:66:77");
    insert("www.wikipedia.org", "198.35.26.1", "DD:44:55:66:77:88");
    insert("mail.google.com", "142.250.10.3", "10:9F:8E:7D:6C:5B");
    insert("web.whatsapp.com", "157.240.12.1", "2C:54:91:A7:B8:C9");
    insert("discord.com", "162.159.136.232", "48:6F:73:12:34:56");
    insert("teams.microsoft.com", "52.112.0.1", "9A:BC:DE:F1:23:45");
}

// ---------- LAYER HELPERS ----------

int readMessage(char filename[], char buffer[]) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    int len = 0, ch;
    while ((ch = fgetc(fp)) != EOF && len < MAX_MESSAGE_LEN) {
        if (ch == '\n' || ch == '\r') continue;
        buffer[len++] = (char)ch;
    }
    buffer[len] = '\0';
    fclose(fp);
    return 0;
}

void toBinary(unsigned long val, int bits, char out[]) {
    out[bits] = '\0';
    for (int i = bits - 1; i >= 0; i--) {
        out[i] = (val & 1) ? '1' : '0';
        val >>= 1;
    }
}

void ipToBinary32(char ip[], char out[]) {
    int a, b, c, d;
    sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d);
    char octet[9];
    out[0] = '\0';
    toBinary(a, 8, octet); strcat(out, octet);
    toBinary(b, 8, octet); strcat(out, octet);
    toBinary(c, 8, octet); strcat(out, octet);
    toBinary(d, 8, octet); strcat(out, octet);
}

void macToBinary48(char mac[], char out[]) {
    unsigned int b[6];
    sscanf(mac, "%x:%x:%x:%x:%x:%x", &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]);
    char byteBits[9];
    out[0] = '\0';
    for (int i = 0; i < 6; i++) {
        toBinary(b[i], 8, byteBits);
        strcat(out, byteBits);
    }
}

// Application Layer: message -> bitstream
int applicationLayer(char message[], char bitstreamOut[]) {
    bitstreamOut[0] = '\0';
    char charBits[9];
    for (int i = 0; message[i]; i++) {
        toBinary((unsigned char)message[i], 8, charBits);
        strcat(bitstreamOut, charBits);
    }
    return (int)strlen(message) * 8;
}

// Transport Layer: random ports
Ports transportLayer(void) {
    Ports p;
    p.src = 1024 + rand() % (65535 - 1024);
    p.dest = 1024 + rand() % (65535 - 1024);
    return p;
}

// Network Layer: split bitstream into 16-bit packets
int networkLayer(char bitstream[], int totalBits, char packets[][PACKET_BITS + 1]) {
    int packetCount = 0;
    for (int pos = 0; pos < totalBits; pos += PACKET_BITS) {
        char chunk[PACKET_BITS + 1];
        int i;
        for (i = 0; i < PACKET_BITS && (pos + i) < totalBits; i++)
            chunk[i] = bitstream[pos + i];
        for (; i < PACKET_BITS; i++) chunk[i] = '0';
        chunk[PACKET_BITS] = '\0';
        strcpy(packets[packetCount], chunk);
        packetCount++;
    }
    return packetCount;
}

// Build network layer header (ports + IPs)
void buildNetworkHeader(char header[], char srcPortBits[], char destPortBits[],
                        char srcIPBin[], char destIPBin[]) {
    header[0] = '\0';
    strcat(header, srcPortBits);
    strcat(header, destPortBits);
    strcat(header, srcIPBin);
    strcat(header, destIPBin);
}

// ---------- MENU ----------

void printMenu() {
    printf("\n======= MENU =======\n");
    printf("1. Display Hash Table\n");
    printf("2. Add URL Entry\n");
    printf("3. Update Entry\n");
    printf("4. Delete Entry\n");
    printf("5. Run Network Layers (Byte-Count + CRC)\n");
    printf("6. Exit\n");
    printf("Select (1-6): ");
}

// ---------- MAIN ----------

int main(void) {
    srand((unsigned int)time(NULL));

    initTable();
    preloadDomains();

    int choice = 0;
    while (choice != 6) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1: {
                printf("\n======= HASH TABLE =======\n");
                printf("%-5s | %-25s | %-15s | %-20s\n", "Idx", "URL", "IP", "MAC");
                printf("--------------------------------------------------------------\n");
                for (int i = 0; i < TABLE_SIZE; i++) {
                    if (table[i].occupied == SLOT_OCCUPIED) {
                        printf("%-5d | %-25s | %-15s | %-20s\n",
                               i, table[i].url, table[i].ip, table[i].mac);
                    }
                }
                printf("--------------------------------------------------------------\n");
                break;
            }
            case 2: {
                char u[100], ip[16], m[18];
                printf("Enter URL: "); scanf("%99s", u);
                printf("Enter IP: "); scanf("%15s", ip);
                printf("Enter MAC (AA:BB:CC:DD:EE:FF): "); scanf("%17s", m);
                if (insert(u, ip, m) == 0) printf("Added.\n");
                else printf("Table full!\n");
                break;
            }
            case 3: {
                char u[100], ip[16], m[18];
                printf("URL to update: "); scanf("%99s", u);
                printf("New IP: "); scanf("%15s", ip);
                printf("New MAC: "); scanf("%17s", m);
                if (updateEntry(u, ip, m) == 0) printf("Updated.\n");
                else printf("Not found.\n");
                break;
            }
            case 4: {
                char u[100];
                printf("URL to delete: "); scanf("%99s", u);
                if (deleteEntry(u) == 0) printf("Deleted.\n");
                else printf("Not found.\n");
                break;
            }
            case 5: {
                // ----- NETWORK LAYER PROCESSING -----
                char message[MAX_MESSAGE_LEN + 1];
                if (readMessage("message1.txt", message) != 0) {
                    printf("Error: Cannot open message1.txt\n");
                    break;
                }

                char srcURL[100], destURL[100];
                printf("\nEnter Source URL: "); scanf("%99s", srcURL);
                printf("Enter Destination URL: "); scanf("%99s", destURL);

                int srcIdx = lookup(srcURL);
                int destIdx = lookup(destURL);
                if (srcIdx == -1 || destIdx == -1) {
                    printf("Error: URL not found in table.\n");
                    break;
                }

                // Layer 1: Application
                char bitstream[MAX_BITS + 1];
                int totalBits = applicationLayer(message, bitstream);

                printf("\n======== LAYER 1: APPLICATION ========\n");
                printf("Message: %s\n", message);
                printf("Bitstream (%d bits):\n%s\n", totalBits, bitstream);

                // Layer 2: Transport
                Ports ports = transportLayer();
                char srcPortBits[17], destPortBits[17];
                toBinary(ports.src, 16, srcPortBits);
                toBinary(ports.dest, 16, destPortBits);

                printf("\n======== LAYER 2: TRANSPORT ========\n");
                printf("Src Port:  %d (%s)\n", ports.src, srcPortBits);
                printf("Dest Port: %d (%s)\n", ports.dest, destPortBits);

                // Layer 3: Network
                static char packets[MAX_PACKETS][PACKET_BITS + 1];
                int packetCount = networkLayer(bitstream, totalBits, packets);

                char srcIPBin[33], destIPBin[33];
                ipToBinary32(table[srcIdx].ip, srcIPBin);
                ipToBinary32(table[destIdx].ip, destIPBin);

                printf("\n======== LAYER 3: NETWORK ========\n");
                printf("Src IP:  %s (%s)\n", srcIPBin, table[srcIdx].ip);
                printf("Dest IP: %s (%s)\n", destIPBin, table[destIdx].ip);
                printf("Packets: %d\n", packetCount);
                for (int i = 0; i < packetCount; i++) {
                    printf("  Packet %d: %s\n", i + 1, packets[i]);
                }

                // Layer 4: Data Link (Byte-Count Framing + CRC)
                char srcMACBin[49], destMACBin[49];
                macToBinary48(table[srcIdx].mac, srcMACBin);
                macToBinary48(table[destIdx].mac, destMACBin);

                // CRC Key (generator polynomial)
                char crcKey[] = "1101";  // x^3 + x^2 + 1

                // Build network header (ports + IPs + MACs)
                char netHeader[500];
                buildNetworkHeader(netHeader, srcPortBits, destPortBits, srcIPBin, destIPBin);
                strcat(netHeader, srcMACBin);
                strcat(netHeader, destMACBin);

                printf("\n======== LAYER 4: DATA LINK (Byte-Count + CRC) ========\n");
                printf("CRC Polynomial: %s\n", crcKey);

                // Process each packet as a frame
                for (int i = 0; i < packetCount; i++) {
                    // Frame payload = network header + packet data
                    char framePayload[MAX_FRAME_SIZE];
                    snprintf(framePayload, MAX_FRAME_SIZE, "%s%s", netHeader, packets[i]);

                    ByteCountFrame frame;
                    buildFrame(&frame, framePayload, crcKey);
                    sendFrame(&frame, crcKey);

                    // Ask user if they want to inject error
                    char injectChoice;
                    printf("\nInject error in this frame? (y/n): ");
                    scanf(" %c", &injectChoice);
                    bool inject = (injectChoice == 'y' || injectChoice == 'Y');

                    receiveFrame(&frame, crcKey, inject);
                    printf("\n");
                }
                break;
            }
            case 6:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}
