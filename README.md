# RTL-SDR Text and Audio Transmission

This repository contains two scripts for transmitting **text messages** and **audio broadcasts** using the **LO leakage** of an RTL-SDR dongle. The scripts leverage the RTL-SDR's local oscillator (LO) to create a very low-power transmitter.

---

## Features

- **Text Transmission**:
  - Transmit text messages using **Frequency Shift Keying (FSK)**.
  - Receive and decode text messages using another RTL-SDR.

- **Audio Broadcast**:
  - Transmit audio (e.g., music or voice) using **Frequency Modulation (FM)**.
  - Receive and play the audio broadcast using another RTL-SDR or an FM radio receiver.

---

## Prerequisites

### Hardware
- **RTL-SDR Dongle**: Any RTL2832U-based SDR dongle.
- **Antenna**: A vertical antenna (1–2 feet long) for transmission and reception.

### Software
- **Linux**: Tested on Ubuntu.
- **Dependencies**:
  - `librtlsdr`
  - `libsndfile` (for audio transmission)
  - `liquid-dsp` (for modulation/demodulation)
  - `portaudio` (for audio playback)

Install the dependencies using:
```bash
sudo apt-get install librtlsdr-dev libsndfile1-dev liquid-dsp portaudio19-dev
git clone https://github.com/your-username/rtl-sdr-transmission.git
cd rtl-sdr-transmission
gcc -o lo_tx lo_tx.c -lrtlsdr -lm
gcc -o lo_rx lo_rx.c -lrtlsdr -lm -lliquid
gcc -o audio_tx audio_tx.c -lrtlsdr -lsndfile -lliquid -lm
gcc -o audio_rx audio_rx.c -lrtlsdr -lliquid -lportaudio -lm

SEND TEXT
./lo_tx 3000000 "Hello, world!"

RECEIVE TEXT
./lo_rx

    Convert your audio file to 48 kHz mono WAV format:
    ffmpeg -i input.wav -ar 48000 -ac 1 mono_audio.wav
   Transmit the Audio:
   ./audio_tx mono_audio.wav

   Receive the Audio Broadcast:
  ./audio_rx


''''
The [R82XX] PLL not locked! messages are harmless but can be suppressed by redirecting stderr:
bash
Copy

./lo_tx "Hello, world!" 2>/dev/null

