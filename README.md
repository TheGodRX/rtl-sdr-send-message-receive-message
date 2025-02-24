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

To send a text message on custom frequecy / MHz edit the lo_tx.c & lo_rx.c to have your desired frequency and recompile using the gcc commands listed then, run:

./lo_tx <REPLACE_WITH DESIRED FREQUENCY>"Hello, world!"
EXAMPLE:  ./lo_tx 3000000 "Hello, world!"

..SET FREQUENCY MUST BE THE SAME AS IN .c FILES BEFORE COMPILATION!



    Note: To suppress harmless [R82XX] PLL not locked! messages:

./lo_tx 3000000 "Hello, world!" 2>/dev/null

2. Receive Text

To receive and decode the transmitted message:

./lo_rx

🔊 Sending and Receiving Audio

Send the audio file
```'
bash sendauidio.sh
or
sox -v 0.9 audio.wav -t wav -rate 22050 | rtl_fm -f 89M -s 22050 -r 22050 - | tee >(play -t raw -r 22050 -e signed -b 16 -c 1 -)

Listen to the audio broadcast:

bash receiveaudio.sh
or
 rtl_fm -f 89M -s 22050 -r 22050 | play -t raw -r 22050 -e signed -b 16 -c 1 -
(89M should be chnaged to the freq you want to receive on & vs versa for sending)


🧪 Troubleshooting

    Device Not Found: Ensure your RTL-SDR is properly connected:

rtl_test -t

    Suppress PLL Warnings: Redirect errors to /dev/null:

./lo_tx 3000000 "Hello, world!" 2>/dev/null

📚 Notes

    Ensure legal compliance—unauthorized transmission may violate local regulations.
    Adjust frequencies carefully to avoid interference with critical systems.
    Use a suitable antenna to improve range and signal clarity.
