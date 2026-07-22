from pathlib import Path
from time import perf_counter
from faster_whisper import WhisperModel

DEFAULT_MODEL = "small.en"

model = WhisperModel(
    DEFAULT_MODEL,
    device="cpu",
    compute_type="int8"
)


def transcribe_audio(audio_path: str) -> tuple[str, float]:

    segments, info = model.transcribe(
        audio_path,
    )

    partes_transcricao = []

    # É durante esta iteração que a transcrição realmente é processada
    for segment in segments:
        texto_segmento = segment.text.strip()
        partes_transcricao.append(texto_segmento)

        print(
            "[%.2fs -> %.2fs] %s"
            % (segment.start, segment.end, segment.text)
        )

    transcript = " ".join(partes_transcricao).strip()

    return transcript


if __name__ == "__main__":
    audio_paths = [
        Path("debug_audio") / "aurora.m4a",
        Path("debug_audio") / "blossom.m4a",
        Path("debug_audio") / "candy.m4a",
        Path("debug_audio") / "cinammon.m4a",
        Path("debug_audio") / "crystal.m4a",
        Path("debug_audio") / "ember.m4a",
        Path("debug_audio") / "firefly.m4a",
        Path("debug_audio") / "forest.m4a",
        Path("debug_audio") / "glacier.m4a",
        Path("debug_audio") / "lagoon.m4a",
        Path("debug_audio") / "lavender.m4a",
        Path("debug_audio") / "mirage.m4a",
        Path("debug_audio") / "nebula.m4a",
        Path("debug_audio") / "ocean.m4a",
        Path("debug_audio") / "sakura.m4a",
        Path("debug_audio") / "serenity.m4a",
        Path("debug_audio") / "sunset.m4a",
        Path("debug_audio") / "velvet.m4a",
    ]


    for audio_path in audio_paths:
        print(f"\nTestando: {audio_path.name}")

        if not audio_path.exists():
            print(f"Arquivo não encontrado: {audio_path}")
            continue

        texto = transcribe_audio(str(audio_path))


        print(f"Transcrição final: {texto}")
