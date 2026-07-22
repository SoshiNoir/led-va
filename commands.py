
import re


commands = {
    "brisa" : {
    "colors" : [
    (196,184,230),
    (168,192,216),
    (224,232,240)
    ],
    "mode" : "triad",
    "brightness" : 50
    }
}


def parse_command(transcript):
    """Convert a transcript into actions the application can execute."""
    text = transcript.strip().casefold()

    if re.search(r"\b(parar|encerrar)\b", text):
        return [{"type": "end_session"}]

    actions = []

    for preset_name in commands:
        if re.search(rf"\b{re.escape(preset_name)}\b", text):
            actions.append({"type": "preset", "name": preset_name})
            break

    brightness_match = re.search(r"\bbrilho(?:\s+de)?\s+(\d{1,3})\b", text)
    if brightness_match:
        brightness = int(brightness_match.group(1))
        if 0 <= brightness <= 255:
            actions.append({"type": "brightness", "value": brightness})
        else:
            print("Brightness must be between 0 and 255.")

    return actions
