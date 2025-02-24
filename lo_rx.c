#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <rtl-sdr.h>
#include <signal.h>
#include <liquid/liquid.h>

#define SAMPLE_RATE 2400000  // Sample rate in Hz
#define BAUD_RATE 45         // Baud rate for FSK
#define FREQ_0 3000000       // Frequency for '0' (3 MHz)
#define FREQ_1 3000440       // Frequency for '1' (3 MHz + 440 Hz)
#define RX_GAIN 20           // Receive gain (arbitrary units)

int do_exit = 0;

// Signal handler for graceful exit
static void sighandler(int signum) {
    (void)signum;
    do_exit = 1;
}

// Decode binary data to text
void binary_to_text(const uint8_t *binary, int binary_len, char *text) {
    int text_len = 0;
    for (int i = 0; i < binary_len; i += 8) {
        char c = 0;
        for (int j = 0; j < 8; j++) {
            c |= binary[i + j] << (7 - j);
        }
        text[text_len++] = c;
    }
    text[text_len] = '\0';
}

int main() {
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
    rtlsdr_set_tuner_gain(dev, RX_GAIN);
    rtlsdr_set_center_freq(dev, FREQ_0);

    // Set up signal handler
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);

    printf("Listening for transmission...\n");

    // Receive and decode FSK signal
    uint8_t binary[1024];
    int binary_len = 0;

    while (!do_exit) {
        uint8_t buffer[1024];
        int n_read;
        rtlsdr_read_sync(dev, buffer, sizeof(buffer), &n_read);

        // Simple FSK demodulation
        for (int i = 0; i < n_read; i++) {
            binary[binary_len++] = buffer[i] > 128 ? 1 : 0;
            if (binary_len >= 1024) break;
        }
    }

    // Decode binary to text
    char text[128];
    binary_to_text(binary, binary_len, text);
    printf("Received message: %s\n", text);

    // Clean up
    rtlsdr_close(dev);
    return 0;
}
