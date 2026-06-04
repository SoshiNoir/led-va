import winsound
import pvporcupine
import pyaudio
import struct
import speech_recognition as sr
import requests
import time
import os
from dotenv import load_dotenv

load_dotenv()

IP = os.getenv("ESP_IP")
ACCESS_KEY = os.getenv("PORCUPINE_KEY")

# =========================
TEMPO_SESSAO = 15  # segundos que o modo ativo dura

# =========================
# ESTADO DO SISTEMA
# =========================
modo_ativo = False
tempo_limite = 0

# =========================
# COMANDOS

commands = {
    #cyan e white bugados
                    
    "vermelho": "red",
    "verde": "green",
    "azul": "blue",
    "branco": "white",
    "amarelo": "yellow",
    "roxo": "purple",
    "turco": "cyan",
    "laranja": "orange",
    "grade": "gradient",
    "rosa": "pink",
    "dourado": "gold",
    "lavanda": "lavender",
    "gelado": "iceblue",
    "laranjinho" : "prisma",
    "vamos" : "trianim",
    "desligar" : "off"
}


def led_command(texto):
    texto = texto.strip().lower()
    for palavra, endpoint in commands.items():
        if palavra in texto:
            print(f"[ESP] -> {endpoint}")
            try:
                requests.get(f"http://{IP}/{endpoint}", timeout=5)
            except:
                print("Erro ao conectar no ESP")
            return True
    return False


def ouvir_comando(recognizer, mic):
    try:
        with mic as source:
            audio = recognizer.listen(source, timeout=2, phrase_time_limit=3)

        texto = recognizer.recognize_google(audio, language="pt-BR")
        return texto.lower()

    except sr.WaitTimeoutError:
        return None

    except sr.UnknownValueError:
        print("Não entendi")
        return None


# =========================
# FUNÇÃO: ESCUTAR WAKEWORD
# =========================

def detectar_wakeword(porcupine, audio_stream):
    pcm = audio_stream.read(
        porcupine.frame_length,
        exception_on_overflow=False
    )
    pcm = struct.unpack_from("h" * porcupine.frame_length, pcm)

    keyword_index = porcupine.process(pcm)
    return keyword_index >= 0


# =========================
# INICIALIZAÇÃO
# =========================

porcupine = pvporcupine.create(
    access_key=ACCESS_KEY,
    keyword_paths=["wakeword/alexo.ppn"]
)

pa = pyaudio.PyAudio()

audio_stream = pa.open(
    rate=porcupine.sample_rate,
    channels=1,
    format=pyaudio.paInt16,
    input=True,
    frames_per_buffer=porcupine.frame_length
)

recognizer = sr.Recognizer()
mic = sr.Microphone()

print("Calibrando microfone...")
with mic as source:
    recognizer.adjust_for_ambient_noise(source, duration=1)

print("Sistema pronto!")

# =========================
# LOOP PRINCIPAL
# =========================
while True:

    # ============================================
    #MODO ATIVO
    # ============================================
    if modo_ativo:

        # verifica timeout
        if time.time() > tempo_limite:
            modo_ativo = False
            print("  ❌ Sessão encerrada")
            continue

        print("🎤 (ativo) ouvindo...")
        texto = ouvir_comando(recognizer, mic)

        if texto:
            print("Você disse:", texto)

            # executa comando
            sucesso = led_command(texto)

            # renova sessão se comando válido
            if sucesso:
                tempo_limite = time.time() + TEMPO_SESSAO
                print("⏱️ Sessão renovada")

            # comando manual de saída
            if "parar" in texto or "encerrar" in texto:
                modo_ativo = False
                print("❌ Encerrado por voz")

    # =============
    # MODO PASSIVO
    # =============
    
    else:

        if detectar_wakeword(porcupine, audio_stream):

            print(" ✔️ Wakeword detectada!")

            winsound.PlaySound(
                "wakeword/alexo_active.wav",
                winsound.SND_FILENAME | winsound.SND_ASYNC
            )

            modo_ativo = True
            tempo_limite = time.time() + TEMPO_SESSAO

            print("🎤 Pode falar...")