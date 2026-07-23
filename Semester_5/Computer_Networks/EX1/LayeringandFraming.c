#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TABLE_SIZE 20
#define MAX_MESSAGE_LEN 1000
#define MAX_BITS 8000
#define MAX_PACKETS 600
#define MAX_FRAMES 1200
#define PACKET_BITS 16
#define FRAME_BITS 8

#define SLOT_EMPTY 0
#define SLOT_OCCUPIED 1
#define SLOT_DELETED -1

typedef struct {
  char url[100];
  char ip[16];
  char mac[18];
  int occupied;
} HashEntry;

typedef struct {
  int src;
  int dest;
} Ports;

typedef struct {
  unsigned char syn1;
  unsigned char syn2;
  unsigned char frameClass;
  unsigned int count : 14;
  char header[2000];
  char body[MAX_BITS + 1];
  unsigned short crc;
} DDCMPFrame;

DDCMPFrame currentFrame;

HashEntry table[TABLE_SIZE];

unsigned int hashFunc(char url[]) {
  unsigned int sum = 0;
  for (int i = 0; url[i] != '\0'; i++) {
    sum += (unsigned char)url[i];
  }
  return sum % TABLE_SIZE;
}

void initTable(void) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    table[i].occupied = SLOT_EMPTY;
  }
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
    if (table[i].occupied == SLOT_OCCUPIED && strcmp(table[i].url, url) == 0)
      return i;
    if (table[i].occupied == SLOT_EMPTY)
      break;
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

    if (table[i].occupied == SLOT_EMPTY)
      break;
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

    if (table[i].occupied == SLOT_EMPTY)
      break;
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

int readMessage(char filename[], char buffer[]) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
    return -1;

  int len = 0;
  int ch;
  while ((ch = fgetc(fp)) != EOF && len < MAX_MESSAGE_LEN) {
    if (ch == '\n' || ch == '\r')
      continue;
    buffer[len] = (char)ch;
    len++;
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
  toBinary(a, 8, octet);
  strcat(out, octet);
  toBinary(b, 8, octet);
  strcat(out, octet);
  toBinary(c, 8, octet);
  strcat(out, octet);
  toBinary(d, 8, octet);
  strcat(out, octet);
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

int applicationLayer(char message[], char bitstreamOut[]) {
  bitstreamOut[0] = '\0';
  char charBits[9];
  for (int i = 0; message[i] != '\0'; i++) {
    toBinary((unsigned char)message[i], 8, charBits);
    strcat(bitstreamOut, charBits);
  }
  return (int)strlen(message) * 8;
}

Ports transportLayer(void) {
  Ports p;
  p.src = 1024 + rand() % (65535 - 1024);
  p.dest = 1024 + rand() % (65535 - 1024);
  return p;
}

int networkLayer(char bitstream[], int totalBits,
                 char packets[][PACKET_BITS + 1]) {
  int packetCount = 0;
  for (int pos = 0; pos < totalBits; pos += PACKET_BITS) {
    char chunk[PACKET_BITS + 1];
    int i;
    for (i = 0; i < PACKET_BITS && (pos + i) < totalBits; i++)
      chunk[i] = bitstream[pos + i];
    for (; i < PACKET_BITS; i++)
      chunk[i] = '0';
    chunk[PACKET_BITS] = '\0';

    strcpy(packets[packetCount], chunk);
    packetCount++;
  }
  return packetCount;
}

void buildNetworkFinal(char networkFinal[], int packetCount,
                       char packets[][PACKET_BITS + 1], char srcPortBits[],
                       char destPortBits[], char srcIPBin[], char destIPBin[]) {
  networkFinal[0] = '\0';
  for (int i = 0; i < packetCount; i++) {
    strcat(networkFinal, packets[i]);
  }
  strcat(networkFinal, srcPortBits);
  strcat(networkFinal, destPortBits);
  strcat(networkFinal, srcIPBin);
  strcat(networkFinal, destIPBin);
}

// Data Link Layer functions from DDCMP are implemented directly in main for
// simplicity

void MakeFrame(char *headerStr, char *bodyData) {
  currentFrame.syn1 = 0xAA;
  currentFrame.syn2 = 0xAA;
  currentFrame.frameClass = 0x10;
  currentFrame.count = (unsigned int)strlen(bodyData) / 8;
  strcpy(currentFrame.header, headerStr);
  strcpy(currentFrame.body, bodyData);
  currentFrame.crc = 0x0000;
}

void SendFrame() {
  printf("\n========== SENDER SIDE ==========\n");
  printf("SYN1   : 0x%02X\n", currentFrame.syn1);
  printf("SYN2   : 0x%02X\n", currentFrame.syn2);
  printf("Class  : 0x%02X\n", currentFrame.frameClass);
  printf("Count  : %u bytes\n", currentFrame.count);
  printf("Header : %s\n", currentFrame.header);
  printf("Body   : %s\n", currentFrame.body);
  printf("CRC    : 0x%04X\n", currentFrame.crc);
  printf("=================================\n");
}

void ReceiveFrame() {
  printf("\n========= RECEIVER SIDE =========\n");

  // 1. Check SYN bytes
  if (currentFrame.syn1 != 0xAA || currentFrame.syn2 != 0xAA) {
    printf("[Error] Synchronization failed! Invalid SYN bytes.\n");
    return;
  }
  printf("[Receiver] SYN bytes verified.\n");

  // 2. Check Class type
  if (currentFrame.frameClass != 0x10) {
    printf("[Error] Invalid frame class! Expected data frame.\n");
    return;
  }
  printf("[Receiver] Class type verified (Data Frame).\n");

  // 3. Check Count (Framing Error check)
  unsigned int actualBodyLen = (unsigned int)strlen(currentFrame.body) / 8;
  if (currentFrame.count != actualBodyLen) {
    printf("[Error] Framing Error! Count field (%u) does not match the actual "
           "body size (%u).\n",
           currentFrame.count, actualBodyLen);
    return;
  }
  printf("[Receiver] Count field verified (%u bytes).\n", currentFrame.count);

  printf("SYN1   : 0x%02X\n", currentFrame.syn1);
  printf("SYN2   : 0x%02X\n", currentFrame.syn2);
  printf("Class  : 0x%02X\n", currentFrame.frameClass);
  printf("Count  : %u bytes\n", currentFrame.count);
  printf("Header : %s\n", currentFrame.header);
  printf("Body   : %s\n", currentFrame.body);
  printf("CRC    : 0x%04X\n", currentFrame.crc);
  printf("[Receiver] Frame received successfully.\n");
  printf("=================================\n");
}

int main(void) {
  srand((unsigned int)time(NULL));

  initTable();
  preloadDomains();

  int choice = 0;

  while (choice != 5) {
    printf("\n======= MENU =======\n");
    printf("1. Display Hash Table\n");
    printf("2. Add New URL Entry\n");
    printf("3. Update Existing Entry\n");
    printf("4. Delete Entry\n");
    printf("5. Network Layers\n");
    printf("6. Exit\n");
    printf("Select an option (1-6): ");

    if (scanf("%d", &choice) != 1) {
      printf("Invalid input. Exiting.\n");
      exit(1);
    }

    switch (choice) {
    case 1:
    case 5: {
      if (choice == 5)
        printf("\nNetwork Layers & Framing...\n");
      printf(
          "\n==================== CURRENT HASH TABLE =====================\n");
      printf("%-5s | %-25s | %-15s | %-20s\n", "Index", "URL", "IP Address",
             "MAC Address");
      printf("-----------------------------------------------------------------"
             "-----\n");
      int count = 0;
      for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i].occupied == SLOT_OCCUPIED) {
          printf("%-5d | %-25s | %-15s | %-20s\n", i, table[i].url, table[i].ip,
                 table[i].mac);
          count++;
        }
      }
      if (count == 0)
        printf(" (Table is completely empty)\n");
      printf("-----------------------------------------------------------------"
             "-----\n");
      break;
    }
    case 2: {
      char u[100], ipAddr[16], m[18];
      printf("Enter URL: ");
      scanf("%99s", u);
      printf("Enter IP (e.g. 192.168.1.1): ");
      scanf("%15s", ipAddr);
      printf("Enter MAC (e.g. AA:BB:CC:DD:EE:FF): ");
      scanf("%17s", m);
      if (insert(u, ipAddr, m) == 0)
        printf("Entry added.\n");
      else
        printf("Error: Hash table full! Could not insert %s\n", u);
      break;
    }
    case 3: {
      char u[100], ipAddr[16], m[18];
      printf("Enter URL to update: ");
      scanf("%99s", u);
      printf("Enter NEW IP: ");
      scanf("%15s", ipAddr);
      printf("Enter NEW MAC: ");
      scanf("%17s", m);
      if (updateEntry(u, ipAddr, m) == 0)
        printf("Successfully updated %s\n", u);
      else
        printf("Error: URL '%s' not found. Cannot update.\n", u);
      break;
    }
    case 4: {
      char u[100];
      printf("Enter URL to delete: ");
      scanf("%99s", u);
      if (deleteEntry(u) == 0)
        printf("Successfully deleted %s\n", u);
      else
        printf("Error: URL '%s' not found. Cannot delete.\n", u);
      break;
    }
    case 6:
      printf("Exiting program.\n");
      exit(0);
    default:
      printf("Invalid choice. Try again.\n");
    }
  }

  char message[MAX_MESSAGE_LEN + 1];
  if (readMessage("message1.txt", message) != 0) {
    printf("ERROR: Could not open message.txt\n");
    exit(1);
  }

  char srcURL[100], destURL[100];
  printf("\n================ INPUT =======================\n");
  printf("Enter Source URL      : ");
  scanf("%99s", srcURL);
  printf("Enter Destination URL : ");
  scanf("%99s", destURL);

  int srcIndex = lookup(srcURL);
  if (srcIndex == -1) {
    printf("\nERROR: Source URL '%s' not found!\n", srcURL);
    return 1;
  }

  int destIndex = lookup(destURL);
  if (destIndex == -1) {
    printf("\nERROR: Destination URL '%s' not found!\n", destURL);
    return 1;
  }

  printf("\n================ HASH TABLE ==================\n");
  printf("Source      URL: %-25s -> IP: %-15s MAC: %s\n", table[srcIndex].url,
         table[srcIndex].ip, table[srcIndex].mac);
  printf("Destination URL: %-25s -> IP: %-15s MAC: %s\n", table[destIndex].url,
         table[destIndex].ip, table[destIndex].mac);

  char bitstream[MAX_BITS + 1];
  int totalBits = applicationLayer(message, bitstream);

  printf("\n================ LAYER 1 : APPLICATION LAYER ================\n");
  printf("Original Message : %s\n", message);
  printf("Message in Binary    :\n%s\n", bitstream);
  printf("Total bits generated : %d\n", totalBits);

  Ports connectionPorts = transportLayer();
  char srcPortBits[17], destPortBits[17];
  toBinary(connectionPorts.src, 16, srcPortBits);
  toBinary(connectionPorts.dest, 16, destPortBits);

  printf("\n================ LAYER 2 : TRANSPORT LAYER ==================\n");
  printf("Source Port      : %d  -> Binary: %s\n", connectionPorts.src,
         srcPortBits);
  printf("Destination Port : %d  -> Binary: %s\n", connectionPorts.dest,
         destPortBits);
  printf("Message Payload  : %s\n", bitstream);

  static char packets[MAX_PACKETS][PACKET_BITS + 1];
  int packetCount = networkLayer(bitstream, totalBits, packets);

  char srcIPBin[33], destIPBin[33];
  ipToBinary32(table[srcIndex].ip, srcIPBin);
  ipToBinary32(table[destIndex].ip, destIPBin);

  printf("\n================ LAYER 3 : NETWORK LAYER =====================\n");
  static char networkFinal[MAX_PACKETS * PACKET_BITS + 500];
  buildNetworkFinal(networkFinal, packetCount, packets, srcPortBits,
                    destPortBits, srcIPBin, destIPBin);

  for (int i = 0; i < packetCount; i++) {
    printf("\n-- Packet %d --\n", i + 1);
    printf("  Source Port          : %s (%d)\n", srcPortBits,
           connectionPorts.src);
    printf("  Destination Port     : %s (%d)\n", destPortBits,
           connectionPorts.dest);
    printf("  Source IP            : %s (%s)\n", srcIPBin, table[srcIndex].ip);
    printf("  Destination IP       : %s (%s)\n", destIPBin,
           table[destIndex].ip);
    printf("  Data (16 bits)       : %s\n", packets[i]);
  }
  printf("\n Final Output         : %s\n", networkFinal);
  printf("\nTotal Packets Created : %d\n", packetCount);

  char srcMACBin[49], destMACBin[49];
  macToBinary48(table[srcIndex].mac, srcMACBin);
  macToBinary48(table[destIndex].mac, destMACBin);

  printf("\n================ LAYER 4 : DATA LINK LAYER (DDCMP) ============\n");

  // Header: IP Address, Port Address, MAC Address
  char headerStr[2000] = "";
  strcat(headerStr, srcPortBits);
  strcat(headerStr, destPortBits);
  strcat(headerStr, srcIPBin);
  strcat(headerStr, destIPBin);
  strcat(headerStr, srcMACBin);
  strcat(headerStr, destMACBin);

  // Generating two 8-bit DDCMP frames for each 16-bit packet
  int frameCount = 0;
  for (int i = 0; i < packetCount; i++) {
    printf("\n--- Processing Packet %d (16 bits) ---\n", i + 1);

    char frameData1[FRAME_BITS + 1];
    char frameData2[FRAME_BITS + 1];

    strncpy(frameData1, packets[i], FRAME_BITS);
    frameData1[FRAME_BITS] = '\0';

    strncpy(frameData2, packets[i] + FRAME_BITS, FRAME_BITS);
    frameData2[FRAME_BITS] = '\0';

    printf("\n--- Generating Frame %d (8 bits) ---\n", ++frameCount);
    MakeFrame(headerStr, frameData1);
    SendFrame();
    ReceiveFrame();

    printf("\n--- Generating Frame %d (8 bits) ---\n", ++frameCount);
    MakeFrame(headerStr, frameData2);
    SendFrame();
    ReceiveFrame();
  }

  printf("\n================ SUMMARY ======================================\n");
  printf("Total bits in message : %d\n", totalBits);
  printf("Total packets created : %d\n", packetCount);
  printf("Total frames created  : %d (DDCMP format)\n", frameCount);

  return 0;
}