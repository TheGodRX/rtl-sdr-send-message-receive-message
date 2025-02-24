#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <rtl-sdr.h>
#include <signal.h>
#include <math.h>

#define SAMPLE_RATE 2400000  // Sample rate in Hz
#define BAUD_RATE 45         // Baud rate for FSK
#define FREQ_0 3000000       // Frequency for '0' (3 MHz)
#define FREQ_1 3000440       // Frequency for '1' (3 MHz + 440 Hz)
#define TX_GAIN 20           // Transmit gain (arbitrary units)

int do_exit = 0;

// Signal handler for graceful exit
static void sighandler(int signum) {
    (void)signum;
    do_exit = 1;
}

// Convert text to binary (ASCII)
void text_to_binary(const char *text, uint8_t *binary, int *binary_len) {
    *binary_len = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        for (int j = 7; j >= 0; j--) {
            binary[(*binary_len)++] = (text[i] >> j) & 1;
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <frequency> <message>\n", argv[0]);
        return 1;
    }

    unsigned int freq = atoi(argv[1]);
    const char *message = argv[2];

    // Convert message to binary
    uint8_t binary[1024];
    int binary_len;
    text_to_binary(message, binary, &binary_len);

    // Initialize RTL-SDR
    rtlsdr_dev_t *dev = NULL;
    int ret = rtlsdr_open(&dev, 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to open RTL-SDR device.\n");
        return 1;
    }

    // Configure RTL-SDR
    rtlsdr_set_sample_rate(dev, SAMPLE_RATE);
    rtlsdr_set_tuner_gain_mode(dev, 1);
    rtlsdr_set_tuner_gain(dev, TX_GAIN);

    // Set up signal handler
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);

    printf("Transmitting message: %s\n", message);

    // Transmit binary data using FSK
    for (int i = 0; i < binary_len && !do_exit; i++) {
        unsigned int tx_freq = binary[i] ? FREQ_1 : FREQ_0;
        rtlsdr_set_center_freq(dev, tx_freq);
        usleep(1000000 / BAUD_RATE);  // Wait for one baud period
    }

    // Clean up
    rtlsdr_close(dev);
    printf("Transmission complete.\n");

    return 0;
}
