/*
 * oo_proto.h — OO Message Bus protocol types for C.
 * Shared across the full OO stack: bare-metal, host tooling, and interface.
 */
#ifndef OO_PROTO_H
#define OO_PROTO_H

#include <stdint.h>
#include <stddef.h>

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
#pragma pack(push, 1)
typedef struct {
    uint8_t  version;
    uint8_t  from;        /* OOLayer */
    uint8_t  to;          /* OOLayer */
    uint8_t  kind;        /* OOEvent */
    uint64_t seq;
    uint64_t ts;
    uint32_t payload_len;
    uint8_t  reserved[8];
} OOMessageHeader;
#pragma pack(pop)

#define OO_STATIC_ASSERT(name, expr) typedef char name[(expr) ? 1 : -1]
OO_STATIC_ASSERT(oo_message_header_is_32_bytes, sizeof(OOMessageHeader) == OO_MSG_HEADER_SIZE);

/* ============================================================
 * Helpers
 * ============================================================ */

int oo_layer_from_u8(uint8_t value, OOLayer *out_layer);
const char *oo_layer_name(OOLayer layer);

int oo_event_from_u8(uint8_t value, OOEvent *out_event);
const char *oo_event_name(OOEvent event_kind);

/* Fill a header in-place. */
void oo_msg_init(
    OOMessageHeader *header,
    OOLayer from, OOLayer to, OOEvent kind,
    uint64_t seq, uint64_t ts, uint32_t payload_len);

/* Validate a received header. Returns 1 if valid, 0 if not. */
int oo_msg_validate(const OOMessageHeader *header);

/* Serialize a header to the fixed 32-byte wire format. */
void oo_msg_to_bytes(const OOMessageHeader *header, uint8_t out_bytes[OO_MSG_HEADER_SIZE]);

/* Deserialize a header from the fixed 32-byte wire format. */
int oo_msg_from_bytes(const uint8_t in_bytes[OO_MSG_HEADER_SIZE], OOMessageHeader *out_header);

#ifdef __cplusplus
}
#endif

#endif /* OO_PROTO_H */
