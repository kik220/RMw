#include "mag_helpers.h"
#include "../mag_i.h"

#define GPIO_PIN_A &gpio_ext_pa6
#define GPIO_PIN_B &gpio_ext_pa7
#define RFID_PIN &gpio_rfid_carrier_out

#define ZERO_PREFIX 25 // n zeros prefix
#define ZERO_BETWEEN 53 // n zeros between tracks
#define ZERO_SUFFIX 25 // n zeros suffix
//#define US_CLOCK 240
//#define US_INTERPACKET 10

// bits per char on a given track
const uint8_t bitlen[] = {7, 5, 5};
// char offset by track
const int sublen[] = {32, 48, 48};
uint8_t bit_dir = 0;

void play_bit_rfid(uint8_t send_bit, MagSetting* setting) {
    // internal TX over RFID coil
    bit_dir ^= 1;
    furi_hal_gpio_write(RFID_PIN, bit_dir);
    furi_delay_us(setting->us_clock);

    if(send_bit) {
        bit_dir ^= 1;
        furi_hal_gpio_write(RFID_PIN, bit_dir);
    }
    furi_delay_us(setting->us_clock);

    furi_delay_us(setting->us_interpacket);
}

void play_bit_gpio(uint8_t send_bit, MagSetting* setting) {
    // external TX over motor driver wired to PIN_A and PIN_B
    bit_dir ^= 1;
    furi_hal_gpio_write(GPIO_PIN_A, bit_dir);
    furi_hal_gpio_write(GPIO_PIN_B, !bit_dir);
    furi_delay_us(setting->us_clock);

    if(send_bit) {
        bit_dir ^= 1;
        furi_hal_gpio_write(GPIO_PIN_A, bit_dir);
        furi_hal_gpio_write(GPIO_PIN_B, !bit_dir);
    }
    furi_delay_us(setting->us_clock);

    furi_delay_us(setting->us_interpacket);
}

bool play_bit(uint8_t send_bit, MagSetting* setting) {
    // Initialize configured TX method
    if(setting->tx == MagTxStateRFID) {
        play_bit_rfid(send_bit, setting);
    } else if(setting->tx == MagTxStateGPIOA6A7) {
        play_bit_gpio(send_bit, setting);
    } else {
        return false;
    }

    return true;
}

void tx_init_rfid() {
    // initialize RFID system for TX
    furi_hal_power_enable_otg();

    furi_hal_ibutton_start_drive();
    furi_hal_ibutton_pin_low();

    // Initializing at GpioSpeedLow seems sufficient for our needs; no improvements seen by increasing speed setting

    // this doesn't seem to make a difference, leaving it in
    furi_hal_gpio_init(&gpio_rfid_data_in, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_rfid_data_in, false);

    // false->ground RFID antenna; true->don't ground
    // skotopes (RFID dev) say normally you'd want RFID_PULL in high for signal forming, while modulating RFID_OUT
    // dunaevai135 had it low in their old code. Leaving low, as it doesn't seem to make a difference on my janky antenna
    furi_hal_gpio_init(&gpio_nfc_irq_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_write(&gpio_nfc_irq_rfid_pull, false);

    furi_hal_gpio_init(RFID_PIN, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);

    // confirm this delay is needed / sufficient? legacy from hackathon...
    furi_delay_ms(300);
}

void tx_reset_rfid() {
    // reset RFID system
    furi_hal_gpio_write(RFID_PIN, 0);

    furi_hal_rfid_pins_reset();
    furi_hal_power_disable_otg();
}

void tx_init_gpio() {
    furi_hal_power_enable_otg();
    // gpio_item_configure_all_pins(GpioModeOutputPushPull);
    furi_hal_gpio_init(GPIO_PIN_A, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    furi_hal_gpio_init(GPIO_PIN_B, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
}

void tx_reset_gpio() {
    furi_hal_gpio_write(GPIO_PIN_A, 0);
    furi_hal_gpio_write(GPIO_PIN_B, 0);

    //gpio_item_configure_all_pins(GpioModeAnalog);
    furi_hal_power_disable_otg();
}

bool tx_init(MagSetting* setting) {
    // Initialize configured TX method
    if(setting->tx == MagTxStateRFID) {
        tx_init_rfid();
    } else if(setting->tx == MagTxStateGPIOA6A7) {
        tx_init_gpio();
    } else {
        return false;
    }

    return true;
}

bool tx_reset(MagSetting* setting) {
    // Reset configured TX method
    if(setting->tx == MagTxStateRFID) {
        tx_reset_rfid();
    } else if(setting->tx == MagTxStateGPIOA6A7) {
        tx_reset_gpio();
    } else {
        return false;
    }

    return true;
}

void track_to_bits(uint8_t* bit_array, const char* track_data, uint8_t track_index) {
    // convert individual track to bits

    int tmp, crc, lrc = 0;
    int i = 0;

    // convert track data to bits
    for(uint8_t j = 0; track_data[i] != '\0'; j++) {
        crc = 1;
        tmp = track_data[j] - sublen[track_index];

        for(uint8_t k = 0; k < bitlen[track_index] - 1; k++) {
            crc ^= tmp & 1;
            lrc ^= (tmp & 1) << k;
            bit_array[i] = tmp & 1;
            i++;
            tmp >>= 1;
        }
        bit_array[i] = crc;
        i++;
    }

    // finish calculating final "byte" (LRC)
    tmp = lrc;
    crc = 1;
    for(uint8_t j = 0; j < bitlen[track_index] - 1; j++) {
        crc ^= tmp & 1;
        bit_array[i] = tmp & 1;
        i++;
        tmp >>= 1;
    }
    bit_array[i] = crc;
    i++;

    // My makeshift end sentinel. All other values 0/1
    bit_array[i] = 2;
    i++;

    //bool is_correct_length = (i == (strlen(track_data) * bitlen[track_index]));
    //furi_assert(is_correct_length);
}

/*
void mag_spoof_single_track_rfid(FuriString* track_str, uint8_t track_index) {
    // Quick testing...

    tx_init_rfid();

    size_t from;
    size_t to;

    // TODO ';' in first track case
    if(track_index == 0) {
        from = furi_string_search_char(track_str, '%');
        to = furi_string_search_char(track_str, '?', from);
    } else if(track_index == 1) {
        from = furi_string_search_char(track_str, ';');
        to = furi_string_search_char(track_str, '?', from);
    } else {
        from = 0;
        to = furi_string_size(track_str);
    }
    if(from >= to) {
        return;
    }
    furi_string_mid(track_str, from, to - from + 1);

    const char* data = furi_string_get_cstr(track_str);
    uint8_t bit_array[(strlen(data) * bitlen[track_index]) + 1];
    track_to_bits(bit_array, data, track_index);

    FURI_CRITICAL_ENTER();
    for(uint8_t i = 0; i < ZERO_PREFIX; i++) play_bit_rfid(0);
    for(uint8_t i = 0; bit_array[i] != 2; i++) play_bit_rfid(bit_array[i] & 1);
    for(uint8_t i = 0; i < ZERO_SUFFIX; i++) play_bit_rfid(0);
    FURI_CRITICAL_EXIT();

    tx_reset_rfid();
}

void mag_spoof_two_track_rfid(FuriString* track1, FuriString* track2) {
    // Quick testing...

    tx_init_rfid();

    const char* data1 = furi_string_get_cstr(track1);
    uint8_t bit_array1[(strlen(data1) * bitlen[0]) + 1];
    const char* data2 = furi_string_get_cstr(track2);
    uint8_t bit_array2[(strlen(data2) * bitlen[1]) + 1];

    track_to_bits(bit_array1, data1, 0);
    track_to_bits(bit_array2, data2, 1);

    FURI_CRITICAL_ENTER();
    for(uint8_t i = 0; i < ZERO_PREFIX; i++) play_bit_rfid(0);
    for(uint8_t i = 0; bit_array1[i] != 2; i++) play_bit_rfid(bit_array1[i] & 1);
    for(uint8_t i = 0; i < ZERO_BETWEEN; i++) play_bit_rfid(0);
    for(uint8_t i = 0; bit_array2[i] != 2; i++) play_bit_rfid(bit_array2[i] & 1);
    for(uint8_t i = 0; i < ZERO_SUFFIX; i++) play_bit_rfid(0);
    FURI_CRITICAL_EXIT();

    tx_reset_rfid();
}*/

void mag_spoof(Mag* mag) {
    MagSetting* setting = mag->setting;

    // precompute tracks (WIP; ignores reverse and 3rd track)
    // likely will be reworked to Samy's bitmap method anyway...
    const char* data1 = furi_string_get_cstr(mag->mag_dev->dev_data.track[0].str);
    const char* data2 = furi_string_get_cstr(mag->mag_dev->dev_data.track[0].str);
    uint8_t bit_array1[(strlen(data1) * bitlen[0]) + 1];
    uint8_t bit_array2[(strlen(data2) * bitlen[1]) + 1];
    track_to_bits(bit_array1, data1, 0);
    track_to_bits(bit_array2, data2, 1);

    bool spoofed = false;
    do {
        // Initialize configured TX method
        if(!tx_init(setting)) break;

        // Critical timing section (need to eliminate ifs? does this impact timing?)
        FURI_CRITICAL_ENTER();
        // Prefix of zeros
        for(uint8_t i = 0; i < ZERO_PREFIX; i++) {
            if(!play_bit(0, setting)) break;
        }

        // Track 1
        if((setting->track == MagTrackStateAll) || (setting->track == MagTrackStateOne)) {
            for(uint8_t i = 0; bit_array1[i] != 2; i++) {
                if(!play_bit((bit_array1[i] & 1), setting)) break;
            }
        }

        // Zeros between tracks
        if(setting->track == MagTrackStateAll) {
            for(uint8_t i = 0; i < ZERO_BETWEEN; i++) {
                if(!play_bit(0, setting)) break;
            }
        }

        // Track 2 (TODO: Reverse track)
        if((setting->track == MagTrackStateAll) || (setting->track == MagTrackStateTwo)) {
            for(uint8_t i = 0; bit_array2[i] != 2; i++) {
                if(!play_bit((bit_array2[i] & 1), setting)) break;
            }
        }

        // Suffix of zeros
        for(uint8_t i = 0; i < ZERO_SUFFIX; i++) {
            if(!play_bit(0, setting)) break;
        }
        FURI_CRITICAL_EXIT();

        // Reset configured TX method
        if(!tx_reset(setting)) break;
        spoofed = true;
    } while(0);

    UNUSED(spoofed);
    /*if(!spoofed) {
        // error handling?
        // cleanup?
    }*/
}

//// @antirez's code from protoview for bitmapping. May want to refactor to use this...

/* Set the 'bitpos' bit to value 'val', in the specified bitmap
 * 'b' of len 'blen'.
 * Out of range bits will silently be discarded. */
void set_bit(uint8_t* b, uint32_t blen, uint32_t bitpos, bool val) {
    uint32_t byte = bitpos / 8;
    uint32_t bit = bitpos & 7;
    if(byte >= blen) return;
    if(val)
        b[byte] |= 1 << bit;
    else
        b[byte] &= ~(1 << bit);
}

/* Get the bit 'bitpos' of the bitmap 'b' of 'blen' bytes.
 * Out of range bits return false (not bit set). */
bool get_bit(uint8_t* b, uint32_t blen, uint32_t bitpos) {
    uint32_t byte = bitpos / 8;
    uint32_t bit = bitpos & 7;
    if(byte >= blen) return 0;
    return (b[byte] & (1 << bit)) != 0;
}

/*uint32_t convert_signal_to_bits(uint8_t *b, uint32_t blen, RawSamplesBuffer *s, uint32_t idx, uint32_t count, uint32_t rate) {
    if (rate == 0) return 0; // We can't perform the conversion.
    uint32_t bitpos = 0;
    for (uint32_t j = 0; j < count; j++) {
        uint32_t dur;
        bool level;
        raw_samples_get(s, j+idx, &level, &dur);

        uint32_t numbits = dur / rate; // full bits that surely fit. 
        uint32_t rest = dur % rate;    // How much we are left with. 
        if (rest > rate/2) numbits++;  // There is another one.
        while(numbits--) set_bit(b,blen,bitpos++,s[j].level);
    }
    return bitpos;
}*/
