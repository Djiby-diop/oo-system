/*
 * oo_proto.h — OO Message Bus protocol types for C (bare-metal / UEFI)
 * Generated from shared/oo-proto/src/lib.rs — keep in sync.
 * Freestanding: no libc, no stdlib.
 */
#ifndef OO_PROTO_H
#define OO_PROTO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================
 * Protocol constants
 * ============================================================ */
#define OO_PROTO_VERSION     1
#define OO_MSG_HEADER_SIZE   32
#define OO_MAX_PAYLOAD       4096

/* ============================================================
 * OO Layers
 * ============================================================ */
typedef enum {
    OO_LAYER_COGNITIVE   = 1,
    OO_LAYER_KERNEL      = 2,
    OO_LAYER_SIMULATION  = 3,
    OO_LAYER_RESEARCH    = 4,
    OO_LAYER_EVOLUTION   = 5,
    OO_LAYER_META        = 6,
    OO_LAYER_INTERFACE   = 7,
    OO_LAYER_BROADCAST   = 255,
} OOLayer;

/* ============================================================
 * OO Events
 * ============================================================ */
typedef enum {
    OO_EVENT_THINK      = 1,
    OO_EVENT_ACT        = 2,
    OO_EVENT_OBSERVE    = 3,
    OO_EVENT_EVOLVE     = 4,
    OO_EVENT_PATCH      = 5,
    OO_EVENT_QUERY      = 6,
    OO_EVENT_RESPONSE   = 7,
    OO_EVENT_JOURNAL    = 8,
    OO_EVENT_ALARM      = 9,
    OO_EVENT_BOOT       = 10,
    OO_EVENT_HEARTBEAT  = 11,
} OOEvent;

/* ============================================================
 * Message header — 32 bytes, little-endian, packed
 * Wire layout:
 *   [0]      version      : uint8_t
 *   [1]      from         : OOLayer (uint8_t)
 *   [2]      to           : OOLayer (uint8_t)
 *   [3]      kind         : OOEvent (uint8_t)
 *   [4..11]  seq          : uint64_t LE
 *   [12..19] ts           : uint64_t LE (ns since boot)
 *   [20..23] payload_len  : uint32_t LE
 *   [24..31] reserved     : uint8_t[8]
 * ============================================================ */
typedef struct __attribute__((packed)) {
    uint8_t  version;
    uint8_t  from;        /* OOLayer */
    uint8_t  to;          /* OOLayer */
    uint8_t  kind;        /* OOEvent */
    uint64_t seq;
    uint64_t ts;
    uint32_t payload_len;
    uint8_t  reserved[8];
} OOMessageHeader;

/* ============================================================
 * Helpers
 * ============================================================ */

/* Fill a header in-place (little-endian). */
static inline void oo_msg_init(
    OOMessageHeader *h,
    OOLayer from, OOLayer to, OOEvent kind,
    uint64_t seq, uint64_t ts, uint32_t payload_len)
{
    h->version     = OO_PROTO_VERSION;
    h->from        = (uint8_t)from;
    h->to          = (uint8_t)to;
    h->kind        = (uint8_t)kind;
    h->seq         = seq;
    h->ts          = ts;
    h->payload_len = payload_len;
    for (int i = 0; i < 8; i++) h->reserved[i] = 0;
}

/* Validate a received header. Returns 1 if valid, 0 if not. */
static inline int oo_msg_validate(const OOMessageHeader *h) {
    return h != 0 && h->version == OO_PROTO_VERSION;
}

#ifdef __cplusplus
}
#endif

#endif /* OO_PROTO_H */
