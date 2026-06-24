import struct

import pvporcupine


def detect_wakeword(porcupine, audio_stream):
    pcm = audio_stream.read(
        porcupine.frame_length,
        exception_on_overflow=False
    )
    pcm = struct.unpack_from("h" * porcupine.frame_length, pcm)

    keyword_index = porcupine.process(pcm)
    return keyword_index >= 0
