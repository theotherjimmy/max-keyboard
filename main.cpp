#include "mbed.h"
#include "rtos.h"
#include "USBKeyboard.h"

DigitalOut rLED(LED1, LED_OFF);
DigitalOut gLED(LED2, LED_OFF);
DigitalOut bLED(LED3, LED_OFF);
DigitalIn button(P2_7, PullUp);

#define num_rows (6)
DigitalIn *rows[num_rows];
#define num_columns (6)
DigitalOut *columns[num_columns];
USBKeyboard keyboard;
typedef void kbcb(uint8_t row, uint8_t column, void* report);

const uint8_t matrix_definition[num_rows * num_columns] = {
        0x2e, 0x1e, 0x1f, 0x20, 0x21, 0x22,
        0x2b, 0x14, 0x1A, 0x08, 0x15, 0x17,
        0x29, 0x04, 0x16, 0x07, 0x09, 0x0a,
        0xE1, 0x1d, 0x1b, 0x06, 0x19, 0x05,
        0x00, 0x35, 0x31, 0x50, 0x4f, 0x00,
        0x2c, 0xE1, 0x4d, 0x4a, 0xe2, 0xe0,
};

struct key_report {
        uint8_t report_id;
        uint8_t modifiers;
        uint8_t zero;
        uint8_t usage_codes[6];
};

void add_key_to_report(uint8_t usage_code, struct key_report* report) {
        switch (usage_code) {
        case 0x00: {
                break;
        }
        case 0xe0: case 0xe1: case 0xe2: case 0xe3:
        case 0xe4: case 0xe5: case 0xe6: case 0xe7: {
                report->modifiers |= (1 << (usage_code & 0x07));
                break;
        }
        default: {
                for (int i = 3; i < 9; i++){
                        if (report->usage_codes[i] == 0) {
                                report->usage_codes[i] = usage_code;
                                return;
                        }
                }
        }
        }
}

void scan_keys_callback(uint8_t rows, uint8_t columns, void* report) {
        if (rows < 6 && columns < 6) {
                uint8_t usage_code = matrix_definition[columns + (6*rows)];
                add_key_to_report(usage_code, (struct key_report*) report);
        }
}

void read_matrix(kbcb* cb, void* data) {
        for (int col = 0; col < num_columns; col++) {
                for (int j = 0; j < num_columns; j++) {
                        *(columns[j]) = (col == j);
                }
                for (int row = 0; row < num_rows; row++){
                        if (*(rows[row])) {
                                cb(row, col, data);
                                bLED = LED_ON;
                        }
                }
        }
}

int main()
{
        gLED = LED_OFF;
        bLED = LED_OFF;
        rLED = LED_ON;

        rows[5] = new DigitalIn(P0_2, PullDown);
        rows[4] = new DigitalIn(P0_3, PullDown);
        rows[3] = new DigitalIn(P0_4, PullDown);
        rows[2] = new DigitalIn(P0_5, PullDown);
        rows[1] = new DigitalIn(P0_6, PullDown);
        rows[0] = new DigitalIn(P0_7, PullDown);

        columns[0] = new DigitalOut(P1_6, 0);
        columns[1] = new DigitalOut(P1_7, 0);
        columns[2] = new DigitalOut(P4_4, 0);
        columns[3] = new DigitalOut(P4_5, 0);
        columns[4] = new DigitalOut(P4_6, 0);
        columns[5] = new DigitalOut(P4_7, 0);

        while (true) {
                HID_REPORT report;
                struct key_report *data = (struct key_report*) &report.data;
                bLED = LED_OFF;
                report.length = 9;
                data->report_id = 1;
                data->modifiers = 0;
                data->zero = 0;
                for (int i = 0; i < 6; i++){
                        data->usage_codes[i] = 0;
                }
                Thread::wait(8);
                read_matrix(scan_keys_callback, data);
                gLED = !gLED;
                keyboard.send(&report);
        }
}
