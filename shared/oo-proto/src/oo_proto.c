#include "oo_proto.h"

#include <stddef.h>

static void oo_write_u32_le(uint8_t *dst, uint32_t value) {
    dst[0] = (uint8_t)(value & 0xffu);
    dst[1] = (uint8_t)((value >> 8) & 0xffu);
    dst[2] = (uint8_t)((value >> 16) & 0xffu);
    dst[3] = (uint8_t)((value >> 24) & 0xffu);
}

static void oo_write_u64_le(uint8_t *dst, uint64_t value) {
    dst[0] = (uint8_t)(value & 0xffu);
    dst[1] = (uint8_t)((value >> 8) & 0xffu);
    dst[2] = (uint8_t)((value >> 16) & 0xffu);
    dst[3] = (uint8_t)((value >> 24) & 0xffu);
    dst[4] = (uint8_t)((value >> 32) & 0xffu);
    dst[5] = (uint8_t)((value >> 40) & 0xffu);
    dst[6] = (uint8_t)((value >> 48) & 0xffu);
    dst[7] = (uint8_t)((value >> 56) & 0xffu);
}

static uint32_t oo_read_u32_le(const uint8_t *src) {
    return ((uint32_t)src[0]) |
           ((uint32_t)src[1] << 8) |
           ((uint32_t)src[2] << 16) |
           ((uint32_t)src[3] << 24);
}

static uint64_t oo_read_u64_le(const uint8_t *src) {
    return ((uint64_t)src[0]) |
           ((uint64_t)src[1] << 8) |
           ((uint64_t)src[2] << 16) |
           ((uint64_t)src[3] << 24) |
           ((uint64_t)src[4] << 32) |
           ((uint64_t)src[5] << 40) |
           ((uint64_t)src[6] << 48) |
           ((uint64_t)src[7] << 56);
}

int oo_layer_from_u8(uint8_t value, OOLayer *out_layer) {
    switch (value) {
        case OO_LAYER_COGNITIVE:
        case OO_LAYER_KERNEL:
        case OO_LAYER_SIMULATION:
        case OO_LAYER_RESEARCH:
        case OO_LAYER_EVOLUTION:
        case OO_LAYER_META:
        case OO_LAYER_INTERFACE:
        case OO_LAYER_BROADCAST:
            if (out_layer != NULL) {
                *out_layer = (OOLayer)value;
            }
            return 1;
        default:
            return 0;
    }
}

const char *oo_layer_name(OOLayer layer) {
    switch (layer) {
        case OO_LAYER_COGNITIVE:  return "cognitive";
        case OO_LAYER_KERNEL:     return "kernel";
        case OO_LAYER_SIMULATION: return "simulation";
        case OO_LAYER_RESEARCH:   return "research";
        case OO_LAYER_EVOLUTION:  return "evolution";
        case OO_LAYER_META:       return "meta";
        case OO_LAYER_INTERFACE:  return "interface";
        case OO_LAYER_BROADCAST:  return "broadcast";
        default:                  return "unknown";
    }
}

int oo_event_from_u8(uint8_t value, OOEvent *out_event) {
    switch (value) {
        case OO_EVENT_THINK:
        case OO_EVENT_ACT:
        case OO_EVENT_OBSERVE:
        case OO_EVENT_EVOLVE:
        case OO_EVENT_PATCH:
        case OO_EVENT_QUERY:
        case OO_EVENT_RESPONSE:
        case OO_EVENT_JOURNAL:
        case OO_EVENT_ALARM:
        case OO_EVENT_BOOT:
        case OO_EVENT_HEARTBEAT:
            if (out_event != NULL) {
                *out_event = (OOEvent)value;
            }
            return 1;
        default:
            return 0;
    }
}

const char *oo_event_name(OOEvent event_kind) {
    switch (event_kind) {
        case OO_EVENT_THINK:     return "think";
        case OO_EVENT_ACT:       return "act";
        case OO_EVENT_OBSERVE:   return "observe";
        case OO_EVENT_EVOLVE:    return "evolve";
        case OO_EVENT_PATCH:     return "patch";
        case OO_EVENT_QUERY:     return "query";
        case OO_EVENT_RESPONSE:  return "response";
        case OO_EVENT_JOURNAL:   return "journal";
        case OO_EVENT_ALARM:     return "alarm";
        case OO_EVENT_BOOT:      return "boot";
        case OO_EVENT_HEARTBEAT: return "heartbeat";
        default:                 return "unknown";
    }
}

void oo_msg_init(
    OOMessageHeader *header,
    OOLayer from,
    OOLayer to,
    OOEvent kind,
    uint64_t seq,
    uint64_t ts,
    uint32_t payload_len) {
    size_t i;

    if (header == NULL) {
        return;
    }

    header->version = OO_PROTO_VERSION;
    header->from = (uint8_t)from;
    header->to = (uint8_t)to;
    header->kind = (uint8_t)kind;
    header->seq = seq;
    header->ts = ts;
    header->payload_len = payload_len;
    for (i = 0; i < sizeof(header->reserved); ++i) {
        header->reserved[i] = 0;
    }
}

int oo_msg_validate(const OOMessageHeader *header) {
    OOLayer ignored_layer;
    OOEvent ignored_event;

    if (header == NULL || header->version != OO_PROTO_VERSION) {
        return 0;
    }

    if (!oo_layer_from_u8(header->from, &ignored_layer)) {
        return 0;
    }
    if (!oo_layer_from_u8(header->to, &ignored_layer)) {
        return 0;
    }
    if (!oo_event_from_u8(header->kind, &ignored_event)) {
        return 0;
    }

    return 1;
}

void oo_msg_to_bytes(const OOMessageHeader *header, uint8_t out_bytes[OO_MSG_HEADER_SIZE]) {
    size_t i;

    if (header == NULL || out_bytes == NULL) {
        return;
    }

    out_bytes[0] = header->version;
    out_bytes[1] = header->from;
    out_bytes[2] = header->to;
    out_bytes[3] = header->kind;
    oo_write_u64_le(&out_bytes[4], header->seq);
    oo_write_u64_le(&out_bytes[12], header->ts);
    oo_write_u32_le(&out_bytes[20], header->payload_len);
    for (i = 0; i < sizeof(header->reserved); ++i) {
        out_bytes[24 + i] = header->reserved[i];
    }
}

int oo_msg_from_bytes(const uint8_t in_bytes[OO_MSG_HEADER_SIZE], OOMessageHeader *out_header) {
    size_t i;

    if (in_bytes == NULL || out_header == NULL) {
        return 0;
    }

    out_header->version = in_bytes[0];
    out_header->from = in_bytes[1];
    out_header->to = in_bytes[2];
    out_header->kind = in_bytes[3];
    out_header->seq = oo_read_u64_le(&in_bytes[4]);
    out_header->ts = oo_read_u64_le(&in_bytes[12]);
    out_header->payload_len = oo_read_u32_le(&in_bytes[20]);
    for (i = 0; i < sizeof(out_header->reserved); ++i) {
        out_header->reserved[i] = in_bytes[24 + i];
    }

    return oo_msg_validate(out_header);
}