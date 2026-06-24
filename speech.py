import speech_recognition as sr


def listen_for_command(recognizer, mic):
    try:
        with mic as source:
            audio = recognizer.listen(source, timeout=2, phrase_time_limit=3)

        transcript = recognizer.recognize_google(audio, language="pt-BR")
        return transcript.lower()

    except sr.WaitTimeoutError:
        return None

    except sr.UnknownValueError:
        print("I dont get it.")
        return None
