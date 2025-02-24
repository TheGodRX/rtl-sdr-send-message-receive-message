#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <rtl-sdr.h>
#include <sndfile.h>
#include <liquid/liquid.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>  // For memset

#define SAMPLE_RATE 2400000  // Sample rate in Hz
#define AUDIO_RATE 48000     // Audio sample rate in Hz
#define CARRIER_FREQ 144000000  // Carrier frequency (144 MHz)
#define FM_DEVIATION 75000   // FM deviation (75 kHz)
#define TX_GAIN 20           // Transmit gain (arbitrary units)

int do_exit = 0;

// Signal handler for graceful exit
static void sighandler(int signum) {
    (void)signum;
    do_exit = 1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <audio_file.wav>\n", argv[0]);
        return 1;
    }

    const char *audio_file = argv[1];

    // Initialize SF_INFO structure
    SF_INFO sfinfo;
    memset(&sfinfo, 0, sizeof(SF_INFO));

    // Open audio file
    SNDFILE *sndfile = sf_open(audio_file, SFM_READ, &sfinfo);
    if (!sndfile) {
        fprintf(stderr, "Error: Could not open audio file.\n");
        return 1;
    }

    // Check audio format
    if (sfinfo.samplerate != AUDIO_RATE) {
        fprintf(stderr, "Error: Audio file must be 48 kHz.\n");
        sf_close(sndfile);
        return 1;
    }

    // Initialize RTL-SDR
    rtlsdr_dev_t *dev = NULL;
    int ret = rtlsdr_open(&dev, 0);
    if (ret < 0) {
        fprintf(stderr, "Failed to open RTL-SDR device.\n");
        sf_close(sndfile);
        return 1;
    }

    // Configure RTL-SDR
    rtlsdr_set_sample_rate(dev, SAMPLE_RATE);
    rtlsdr_set_tuner_gain_mode(dev, 1);
    rtlsdr_set_tuner_gain(dev, TX_GAIN);
    rtlsdr_set_center_freq(dev, CARRIER_FREQ);

    // Set up signal handler
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);

    printf("Transmitting audio from %s on %u Hz...\n", audio_file, CARRIER_FREQ);

    // FM modulator
    freqmod mod = freqmod_create(1.0f);  // FM modulator
    float audio_buffer[AUDIO_RATE * 2];  // Stereo buffer (1 second)
    float mono_buffer[AUDIO_RATE];       // Mono buffer (1 second)
    liquid_float_complex mod_buffer[SAMPLE_RATE];  // Modulated buffer (1 second)

    while (!do_exit) {
        // Read audio samples
        sf_count_t samples_read = sf_read_float(sndfile, audio_buffer, AUDIO_RATE * 2);
        if (samples_read <= 0) break;  // End of file

        // Convert stereo to mono
        for (int i = 0; i < AUDIO_RATE; i++) {
            mono_buffer[i] = (audio_buffer[2 * i] + audio_buffer[2 * i + 1]) / 2.0f;
        }

        // Modulate audio
        freqmod_modulate_block(mod, mono_buffer, AUDIO_RATE, mod_buffer);

        // Transmit modulated signal
        for (unsigned int i = 0; i < SAMPLE_RATE; i++) {
            unsigned int tx_freq = CARRIER_FREQ + (unsigned int)(crealf(mod_buffer[i]) * FM_DEVIATION);
            rtlsdr_set_center_freq(dev, tx_freq);
            usleep(1000000 / SAMPLE_RATE);  // Wait for one sample period
        }
    }

    // Clean up
    freqmod_destroy(mod);
    sf_close(sndfile);
    rtlsdr_close(dev);
    printf("Transmission complete.\n");

    return 0;
}
