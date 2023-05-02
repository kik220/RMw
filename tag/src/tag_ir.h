#ifndef TAG_IR_HEADERS
#define TAG_IR_HEADERS

#include <stdlib.h>
#include <string.h>
#include <core/check.h>
#include <lib/infrared/worker/infrared_transmit.h>
#include <lib/infrared/worker/infrared_worker.h>

#include "tag_app.h"

typedef enum {
    InfraredUninitialised,
    InfraredReady,
    InfraredListening,
    InfraredSending
} TagIrMode;

/**
 * Current state of the infrared.
 */
TagIrMode tag_ir_mode_get();

/**
 * Initialise infrared worker.
 * protocol: protocol to use for all messages
 * repeats: number of times to repeat each message
 * address: fixed address portion of each message
 */
void tag_ir_init(InfraredProtocol protocol, int repeats, uint32_t address);

/**
 * Start listening for infrared signals.
 */
void tag_ir_rx_start(InfraredWorkerReceivedSignalCallback callback, void* context);

/**
 * Stop listening for infrared signals.
 */
void tag_ir_rx_stop();

/**
 * Destroy infrared worker.
 */
void tag_ir_destroy();

/**
 * Create an app specific "firing" message for the identity provided.
 */
InfraredMessage* tag_ir_create_firing_message(int identity);

/**
 * Free resources from the message.
 */
void tag_ir_free_message(InfraredMessage* message);

/**
 * Send the given message.
 */
void tag_ir_send(InfraredMessage* msg);

/** 
 * Validate the message address and command.
 * Borrowed from: https://github.com/Hong5489/ir_remote/blob/main/infrared_signal.c#L27 
 */
bool tag_ir_message_valid(InfraredMessage* message);

#endif
