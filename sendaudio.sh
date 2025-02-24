sox -v 0.9 audio.wav -t wav - rate 22050 | tee >(play -t wav -) | rtl_fm -f 89M -s 22050 -r 22050
