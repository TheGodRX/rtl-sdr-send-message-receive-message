#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <rtl-sdr.h>
#include <liquid/liquid.h>
#include <portaudio.h>
#include <signal.h>

#define SAMPLE_RATE 2400000  // Sample rate in Hz
#define AUDIO_RATE 48000     // Audio sample rate in Hz
#define CARRIER_FREQ 144000000  // Carrier frequency (144 MHz)
#define FM_DEVIATION 75000   // FM deviation (75 kHz)
#define RX_GAIN 20           // Receive gain (arbitrary units)

int do_exit = 0;

// Signal handler for graceful exit
static void sighandler(int signum) {
    (void)signum;
    do_exit = 1;
}

// PortAudio callback
static int pa_callback(const void *input, void *output, unsigned long frameCount,
                       const PaStreamCallbackTimeInfo *timeInfo,
                       PaStreamCallbackFlags statusFlags, void *userData) {
    (void)input;
    (void)timeInfo;
    (void)statusFlags;
    float *out = (float *)output;
    float *audio_buffer = (float *)userData;

    for (unsigned long i = 0; i < frameCount; i++) {
        out[i] = audio_buffer[i];
    }

    return 0;
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
    rtlsdr_set_center_freq(dev, CARRIER_FREQ);

    // Set up signal handler
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGQUIT, sighandler);

    printf("Listening for audio on %u Hz...\n", CARRIER_FREQ);

    // FM demodulator
    freqdem dem = freqdem_create(1.0f);  // FM demodulator
    liquid_float_complex mod_buffer[SAMPLE_RATE];  // Modulated buffer (1 second)
    float audio_buffer[AUDIO_RATE];      // Audio buffer (1 second)

    // Initialize PortAudio
    Pa_Initialize();
    PaStream *stream;
    Pa_OpenDefaultStream(&stream, 0, 1, paFloat32, AUDIO_RATE, 256, pa_callback, audio_buffer);
    Pa_StartStream(stream);

    while (!do_exit) {
        // Read samples from RTL-SDR
        uint8_t buffer[SAMPLE_RATE * 2];
        int n_read;
        rtlsdr_read_sync(dev, buffer, sizeof(buffer), &n_read);

        // Convert to complex float
        liquid_float_complex complex_buffer[SAMPLE_RATE];
        for (int i = 0; i < SAMPLE_RATE; i++) {
            complex_buffer[i] = (float)buffer[i] / 128.0f - 1.0f;
        }

        // Demodulate signal
        freqdem_demodulate_block(dem, complex_buffer, SAMPLE_RATE, audio_buffer);

        // Play audio
        Pa_WriteStream(stream, audio_buffer, AUDIO_RATE);
    }

    // Clean up
    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();
    freqdem_destroy(dem);
    rtlsdr_close(dev);
    printf("Reception complete.\n");

    return 0;
}
