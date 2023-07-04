#pragma once

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>

#include "../nfc_protocol_support_render_common.h"

void nfc_render_iso14443_3a_info(
    const Iso14443_3aData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);
