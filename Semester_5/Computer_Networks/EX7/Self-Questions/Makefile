CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -O2
LDLIBS = -pthread
BUILD_DIR ?= /tmp/ex7-systems-bin

SERVICES = examination hotel files chat
SERVERS = $(addprefix $(BUILD_DIR)/,$(addsuffix _server,$(SERVICES)))
CLIENTS = $(addprefix $(BUILD_DIR)/,$(addsuffix _client,$(SERVICES)))

.PHONY: all check clean

all: $(SERVERS) $(CLIENTS)

$(BUILD_DIR):
	mkdir -p "$@"

$(BUILD_DIR)/examination_server: Examination/server.c common/net.c common/net.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) Examination/server.c common/net.c -o "$@" $(LDLIBS)

$(BUILD_DIR)/hotel_server: Hotel/server.c common/net.c common/net.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) Hotel/server.c common/net.c -o "$@" $(LDLIBS)

$(BUILD_DIR)/files_server: FileSharing/server.c common/net.c common/net.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) FileSharing/server.c common/net.c -o "$@" $(LDLIBS)

$(BUILD_DIR)/chat_server: ChatNotification/server.c common/net.c common/net.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) ChatNotification/server.c common/net.c -o "$@" $(LDLIBS)

$(BUILD_DIR)/examination_client $(BUILD_DIR)/hotel_client $(BUILD_DIR)/chat_client: common/terminal.c common/net.c common/net.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) common/terminal.c common/net.c -o "$@" $(LDLIBS)

$(BUILD_DIR)/files_client: FileSharing/client.c common/net.c common/net.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) FileSharing/client.c common/net.c -o "$@" $(LDLIBS)

check:
	python3 tests/check_systems.py

clean:
	rm -f "$(BUILD_DIR)/examination_server" "$(BUILD_DIR)/hotel_server" \
	      "$(BUILD_DIR)/files_server" "$(BUILD_DIR)/chat_server" \
	      "$(BUILD_DIR)/examination_client" "$(BUILD_DIR)/hotel_client" \
	      "$(BUILD_DIR)/files_client" "$(BUILD_DIR)/chat_client"
