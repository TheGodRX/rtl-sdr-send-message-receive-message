# RTL-SDR Text and Audio Transmission

This repository contains scripts for transmitting and receiving **text messages** and **audio broadcasts** using the **local oscillator (LO) leakage** of an RTL-SDR dongle. While RTL-SDR devices are primarily designed for reception, these scripts exploit LO leakage to allow low-power signal transmission.

## 🛠️ Features

### 📡 Text Transmission
- **Transmit**: Send text messages using **Frequency Shift Keying (FSK)**.
- **Receive**: Decode and display text messages using another RTL-SDR dongle.

### 🔊 Audio Broadcast
- **Transmit**: Send audio (e.g., music or voice) using **Frequency Modulation (FM)**.
- **Receive**: Play the audio signal using another RTL-SDR dongle or an FM radio receiver.

---

## 📋 Prerequisites

### 🧰 Hardware:
- **RTL-SDR Dongle**: Any **RTL2832U**-based SDR dongle.
- **Antenna**: A **1-2 ft vertical antenna** for both transmission and reception.

### 💻 Software:
- **Linux**: Tested on Ubuntu.
- Required packages:
    - `librtlsdr` (for RTL-SDR communication)
    - `libsndfile` (for audio handling)
    - `liquid-dsp` (for modulation/demodulation)
    - `portaudio` (for real-time audio playback)

---

## 📦 Installation

1. **Install dependencies:**
```bash
sudo apt update
sudo apt install librtlsdr-dev libsndfile1-dev liquid-dsp portaudio19-dev

    Clone the repository:

git clone https://github.com/TheGodRX/rtl-sdr-send-message-receive-message.git
cd rtl-sdr-send-message-receive-message

    Compile the programs:

gcc -o lo_tx lo_tx.c -lrtlsdr -lm
gcc -o lo_rx lo_rx.c -lrtlsdr -lm -lliquid
gcc -o audio_tx audio_tx.c -lrtlsdr -lsndfile -lliquid -lm
gcc -o audio_rx audio_rx.c -lrtlsdr -lliquid -lportaudio -lm

📨 Sending and Receiving Text Messages
1. Transmit Text

To send a text message on 3 MHz, run:

./lo_tx 3000000 "Hello, world!"

    Note: To suppress harmless [R82XX] PLL not locked! messages:

./lo_tx 3000000 "Hello, world!" 2>/dev/null

2. Receive Text

To receive and decode the transmitted message:

./lo_rx

🔊 Sending and Receiving Audio
1. Prepare Your Audio File

Convert your audio to 48 kHz, mono WAV:

ffmpeg -i input.wav -ar 48000 -ac 1 mono_audio.wav

2. Transmit Audio

Send the audio file on 3 MHz:

./audio_tx mono_audio.wav

3. Receive Audio

Listen to the audio broadcast:

./audio_rx

📡 Simultaneous Transmission & Local Playback

While the RTL-SDR cannot transmit and receive simultaneously, you can monitor your outgoing signal locally.
For Text Transmission:

./lo_tx 3000000 "Hello, world!" | tee >(cat -)

For Audio Transmission:

./audio_tx mono_audio.wav | tee >(play -t wav -)

🧪 Troubleshooting

    Device Not Found: Ensure your RTL-SDR is properly connected:

rtl_test -t

    Suppress PLL Warnings: Redirect errors to /dev/null:

./lo_tx 3000000 "Hello, world!" 2>/dev/null

📚 Notes

    Ensure legal compliance—unauthorized transmission may violate local regulations.
    Adjust frequencies carefully to avoid interference with critical systems.
    Use a suitable antenna to improve range and signal clarity.
