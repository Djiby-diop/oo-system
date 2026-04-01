#![no_std]
//! OO Message Bus — protocol types shared across all OO layers.
//! `no_std` by default: works in bare-metal (Cognitive Core) and host (all other layers).

// ============================================================
// Layers
// ============================================================

/// Identifies which OO layer a message comes from or goes to.
#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum OOLayer {
    Cognitive  = 1,  // llm-baremetal (UEFI kernel + LLM)
    Kernel     = 2,  // oo-host (Rust agent runtime)
    Simulation = 3,  // oo-sim
    Research   = 4,  // oo-lab
    Evolution  = 5,  // oo-dplus (D+ policy engine)
    Meta       = 6,  // oo-system/meta (self-modification)
    Interface  = 7,  // oo-system/interface (CLI/API/bridge)
    Broadcast  = 255,// all layers
}

impl OOLayer {
    pub fn from_u8(v: u8) -> Option<Self> {
        match v {
            1 => Some(Self::Cognitive),
            2 => Some(Self::Kernel),
            3 => Some(Self::Simulation),
            4 => Some(Self::Research),
            5 => Some(Self::Evolution),
            6 => Some(Self::Meta),
            7 => Some(Self::Interface),
            255 => Some(Self::Broadcast),
            _ => None,
        }
    }

    pub fn name(&self) -> &'static str {
        match self {
            Self::Cognitive  => "cognitive",
            Self::Kernel     => "kernel",
            Self::Simulation => "simulation",
            Self::Research   => "research",
            Self::Evolution  => "evolution",
            Self::Meta       => "meta",
            Self::Interface  => "interface",
            Self::Broadcast  => "broadcast",
        }
    }
}

// ============================================================
// Events
// ============================================================

/// Type of event carried by an OOMessage.
#[repr(u8)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum OOEvent {
    Think    = 1,  // Cognitive: reasoning output
    Act      = 2,  // Kernel: agent action
    Observe  = 3,  // any layer: sensor/environment data
    Evolve   = 4,  // Evolution: policy or module mutation
    Patch    = 5,  // Meta: self-modification proposal
    Query    = 6,  // Interface: question to any layer
    Response = 7,  // any layer: answer to Query
    Journal  = 8,  // any layer: audit log entry
    Alarm    = 9,  // any layer: safety alert
    Boot     = 10, // Cognitive: boot phase event
    Heartbeat= 11, // any layer: alive signal
}

impl OOEvent {
    pub fn from_u8(v: u8) -> Option<Self> {
        match v {
            1  => Some(Self::Think),
            2  => Some(Self::Act),
            3  => Some(Self::Observe),
            4  => Some(Self::Evolve),
            5  => Some(Self::Patch),
            6  => Some(Self::Query),
            7  => Some(Self::Response),
            8  => Some(Self::Journal),
            9  => Some(Self::Alarm),
            10 => Some(Self::Boot),
            11 => Some(Self::Heartbeat),
            _  => None,
        }
    }

    pub fn name(&self) -> &'static str {
        match self {
            Self::Think     => "think",
            Self::Act       => "act",
            Self::Observe   => "observe",
            Self::Evolve    => "evolve",
            Self::Patch     => "patch",
            Self::Query     => "query",
            Self::Response  => "response",
            Self::Journal   => "journal",
            Self::Alarm     => "alarm",
            Self::Boot      => "boot",
            Self::Heartbeat => "heartbeat",
        }
    }
}

// ============================================================
// Message header (fixed 32 bytes — wire format)
// ============================================================

/// Fixed-size message header for the OO Message Bus.
/// Payload follows immediately after in the wire format.
///
/// Wire layout (little-endian, 32 bytes):
///   [0]      version  : u8
///   [1]      from     : OOLayer as u8
///   [2]      to       : OOLayer as u8
///   [3]      kind     : OOEvent as u8
///   [4..11]  seq      : u64 (global sequence counter)
///   [12..19] ts       : u64 (ns since boot or UNIX epoch)
///   [20..23] payload_len : u32
///   [24..31] reserved : [u8; 8]
#[repr(C, packed)]
#[derive(Debug, Clone, Copy)]
pub struct OOMessageHeader {
    pub version:     u8,
    pub from:        u8,   // OOLayer as u8
    pub to:          u8,   // OOLayer as u8
    pub kind:        u8,   // OOEvent as u8
    pub seq:         u64,
    pub ts:          u64,
    pub payload_len: u32,
    pub reserved:    [u8; 8],
}

impl OOMessageHeader {
    pub const SIZE: usize = 32;
    pub const VERSION: u8 = 1;

    pub fn new(from: OOLayer, to: OOLayer, kind: OOEvent, seq: u64, ts: u64, payload_len: u32) -> Self {
        Self {
            version: Self::VERSION,
            from: from as u8,
            to: to as u8,
            kind: kind as u8,
            seq,
            ts,
            payload_len,
            reserved: [0u8; 8],
        }
    }

    /// Serialize to fixed 32-byte array (little-endian).
    pub fn to_bytes(&self) -> [u8; 32] {
        let mut b = [0u8; 32];
        b[0]  = self.version;
        b[1]  = self.from;
        b[2]  = self.to;
        b[3]  = self.kind;
        b[4..12].copy_from_slice(&self.seq.to_le_bytes());
        b[12..20].copy_from_slice(&self.ts.to_le_bytes());
        b[20..24].copy_from_slice(&self.payload_len.to_le_bytes());
        b
    }

    /// Deserialize from 32-byte slice.
    pub fn from_bytes(b: &[u8; 32]) -> Option<Self> {
        if b[0] != Self::VERSION { return None; }
        Some(Self {
            version:     b[0],
            from:        b[1],
            to:          b[2],
            kind:        b[3],
            seq:         u64::from_le_bytes([b[4],b[5],b[6],b[7],b[8],b[9],b[10],b[11]]),
            ts:          u64::from_le_bytes([b[12],b[13],b[14],b[15],b[16],b[17],b[18],b[19]]),
            payload_len: u32::from_le_bytes([b[20],b[21],b[22],b[23]]),
            reserved:    [0u8; 8],
        })
    }

    pub fn from_layer(&self) -> Option<OOLayer>  { OOLayer::from_u8(self.from) }
    pub fn to_layer(&self)   -> Option<OOLayer>  { OOLayer::from_u8(self.to) }
    pub fn event(&self)      -> Option<OOEvent>  { OOEvent::from_u8(self.kind) }
}

// ============================================================
// Convenience constants for bare-metal C interop
// ============================================================

pub const OO_PROTO_VERSION: u8     = 1;
pub const OO_MSG_HEADER_SIZE: usize = 32;
pub const OO_MAX_PAYLOAD: usize    = 4096; // maximum safe payload for shared-mem transport
