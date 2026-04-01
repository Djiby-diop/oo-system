#include "oo_proto.h"

#include <stdio.h>
#include <string.h>

static int check(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "test_oo_proto: %s\n", message);
        return 0;
    }
    return 1;
}

int main(void) {
    OOMessageHeader header;
    OOMessageHeader roundtrip;
    uint8_t bytes[OO_MSG_HEADER_SIZE];

    oo_msg_init(&header, OO_LAYER_INTERFACE, OO_LAYER_COGNITIVE, OO_EVENT_QUERY,
        42u, 123456789u, 17u);

    if (!check(oo_msg_validate(&header), "header should validate after init")) {
        return 1;
    }
    if (!check(strcmp(oo_layer_name(OO_LAYER_INTERFACE), "interface") == 0,
        "interface layer name should match")) {
        return 1;
    }
    if (!check(strcmp(oo_event_name(OO_EVENT_QUERY), "query") == 0,
        "query event name should match")) {
        return 1;
    }

    oo_msg_to_bytes(&header, bytes);
    if (!check(bytes[0] == OO_PROTO_VERSION, "version byte should match protocol version")) {
        return 1;
    }
    if (!check(oo_msg_from_bytes(bytes, &roundtrip), "roundtrip decoding should succeed")) {
        return 1;
    }
    if (!check(roundtrip.seq == 42u, "sequence number should roundtrip")) {
        return 1;
    }
    if (!check(roundtrip.ts == 123456789u, "timestamp should roundtrip")) {
        return 1;
    }
    if (!check(roundtrip.payload_len == 17u, "payload length should roundtrip")) {
        return 1;
    }
    if (!check(roundtrip.from == OO_LAYER_INTERFACE, "from layer should roundtrip")) {
        return 1;
    }
    if (!check(roundtrip.to == OO_LAYER_COGNITIVE, "to layer should roundtrip")) {
        return 1;
    }
    if (!check(roundtrip.kind == OO_EVENT_QUERY, "event kind should roundtrip")) {
        return 1;
    }

    bytes[0] = 99u;
    if (!check(!oo_msg_from_bytes(bytes, &roundtrip), "invalid version should be rejected")) {
        return 1;
    }

    puts("test_oo_proto: ok");
    return 0;
}