import re
import requests
from commands import commands


def send_brightness_command(transcript, ip_address):
    transcript = transcript.strip().lower()
    if "brilho" not in transcript:
        return False

    match = re.search(r"\d+", transcript)
    if not match:
        return False

    brightness = int(match.group())

    try:
        response = requests.get(
            f"http://{ip_address}/bright?value={brightness}",
             timeout=5
             )
             
        response.raise_for_status()
        return True
    except requests.RequestException as exc:
        print(f"Error connecting to ESP: {exc}")
        return False


def send_triad(transcript, ip_address):
    transcript = transcript.strip().lower()
    for preset_name, preset_config in commands.items():
        if preset_name in transcript:
            print(f"Command {preset_name} found.")
            try:
                response = requests.post(
                f"http://{ip_address}/apply",
                json=preset_config,
                timeout=5
                )
                response.raise_for_status()
                return True
            except requests.RequestException as exc:
                print(f"Cannot reach ESP: {exc}")
                return False
    return False

