import re

import requests

from commands import voice_commands






def send_led_command(transcript, ip_address):
    transcript = transcript.strip().lower()

    for keyword, endpoint in voice_commands.items():
        if keyword in transcript:
            print(f"[ESP] -> {endpoint}")
            try:
                requests.get(f"http://{ip_address}/{endpoint}", timeout=5)
            except:
                print("Erro ao conectar no ESP")
            return True

    return False


def send_brightness_command(transcript, ip_address):
    transcript = transcript.strip().lower()
    if "brilho" not in transcript:
        return False

    match = re.search(r"\d+", transcript)
    if not match:
        return False

    brightness = int(match.group())

    try:
        requests.get(f"http://{ip_address}/bright?value={brightness}", timeout=5)
        return True
    except:
        print("Erro ao conectar no ESP")
        return False



