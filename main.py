import os
import time
import winsound
import pvporcupine
import pyaudio
import speech_recognition as sr
from dotenv import load_dotenv
from led_client import send_brightness_command
from led_client import send_triad
from speech import listen_for_command
from wakeword import detect_wakeword


load_dotenv()
IP = os.getenv("ESP_IP")
ACCESS_KEY = os.getenv("PORCUPINE_KEY")

is_active_mode = False
session_deadline = 0
SESSION_TIMEOUT = 15


# INITIALIZATION

porcupine = pvporcupine.create(
    access_key=ACCESS_KEY,
    keyword_paths=["wakeword/alexo.ppn"]
)

pa = pyaudio.PyAudio()
audio_stream = None


def start_wakeword_stream():
    global audio_stream

    if audio_stream is None:
        audio_stream = pa.open(
            rate=porcupine.sample_rate,
            channels=1,
            format=pyaudio.paInt16,
            input=True,
            frames_per_buffer=porcupine.frame_length,
        )


def stop_wakeword_stream():
    global audio_stream

    if audio_stream is not None:
        if audio_stream.is_active():
            audio_stream.stop_stream()
        audio_stream.close()
        audio_stream = None

recognizer = sr.Recognizer()
mic = sr.Microphone()

print("Calibrating microphone...")
with mic as source:
    recognizer.adjust_for_ambient_noise(source, duration=1)
print("System ready.")
start_wakeword_stream()


# MAIN LOOP

try:
    while True:
        if is_active_mode:
            if time.time() > session_deadline:
                winsound.PlaySound(
                    "wakeword/shutdown.wav",
                    winsound.SND_FILENAME | winsound.SND_ASYNC,
                )

                is_active_mode = False
                start_wakeword_stream()
                print("Session ended.")
                continue

            print("Listening...")
            transcript = listen_for_command(recognizer, mic)

            if transcript:
                print("You said:", transcript)

                command_sent = send_triad(transcript, IP)
                brightness_sent = send_brightness_command(transcript, IP)

                if command_sent or brightness_sent:
                    session_deadline = time.time() + SESSION_TIMEOUT
                    print("Session renewed.")

                if "parar" in transcript or "encerrar" in transcript:
                    is_active_mode = False
                    start_wakeword_stream()
                    print("Session ended by voice command.")

        # PASSIVE MODE
        else:
            if detect_wakeword(porcupine, audio_stream):
                print("Wakeword detected.")
                stop_wakeword_stream()

                winsound.PlaySound(
                    "wakeword/alexo_active.wav",
                    winsound.SND_FILENAME | winsound.SND_ASYNC,
                )

                is_active_mode = True
                session_deadline = time.time() + SESSION_TIMEOUT
                print("Listening...")
except KeyboardInterrupt:
    print("System stopped.")
finally:
    stop_wakeword_stream()
    porcupine.delete()
    pa.terminate()
