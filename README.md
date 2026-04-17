# MimiClaw — XIAO ESP32S3 Sense Build

<p align="center">
  <strong><a href="README.md">English</a> | <a href="README_CN.md">中文</a> | <a href="README_JA.md">日本語</a></strong>
</p>

A voice + vision AI gadget running on a **Seeed XIAO ESP32S3 Sense** ($10 board).  
Hold a touch sensor → records your voice → Deepgram transcribes → Gemini responds on Telegram.  
Tap once → takes a photo → AI describes what it sees.

> **Forked from** [luoluoter/mimiclaw](https://github.com/luoluoter/mimiclaw) · commit `8390390`  
> See [CHANGES.md](CHANGES.md) for a full list of modifications.

---

## What this build adds over upstream

| Feature | Upstream | This build |
|---------|----------|------------|
| LLM | Zhipu / Anthropic / OpenAI | + **OpenRouter** (free Gemini 2.5 Flash) + **Gemini native API** |
| Speech-to-text | Zhipu ASR | **Deepgram Nova-3** (free tier, much better accuracy) |
| Web search | Brave Search | + **Tavily** (1000 free/month, no credit card) |
| Touch input | Built-in boot button | **External TTP223 on GPIO 4** (boot button was broken) |
| Voice recording | Fixed duration after release | **Records while held, stops on release** |
| Camera | Manual focus | **OV5640 autofocus** triggered before each capture |
| Microphone | Raw PDM output | **6× software gain** for clean transcription |
| WAV header | fseek patch (broken on SPIFFS) | **Write raw PCM first, assemble WAV after** |
| Silence handling | Send to ASR anyway | **Skip API call**, send feedback to Telegram |

---

## Hardware

| Part | Details |
|------|---------|
| Board | [Seeed XIAO ESP32S3 Sense](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html) |
| Camera | OV5640 (on-board, autofocus) |
| Microphone | PDM (on-board, pins 41/42) |
| Touch sensor | TTP223 capacitive module → GPIO 4 |

### Touch sensor wiring

```
TTP223 module        XIAO ESP32S3 Sense
─────────────        ──────────────────
  VCC         ──────  3.3V  (pin 3V3)
  GND         ──────  GND   (pin GND)
  OUT         ──────  GPIO 4 (pin D2)
```

The TTP223 output is active-high: HIGH when touched, LOW when released.

---

## Quick Start

### Requirements

- **ESP-IDF v5.5.2** — v6.0 is not compatible (`json` component renamed to `cjson`)
- Windows: use the ESP-IDF PowerShell shortcut
- Linux/macOS: `. "$HOME/.espressif/esp-idf-v5.5.2/export.sh"`

### 1) Clone

```bash
git clone https://github.com/subashchandraboseanuradha/miniclaw.git
cd miniclaw
```

### 2) Configure secrets

```bash
cp main/mimi_secrets.h.example main/mimi_secrets.h
```

Edit `main/mimi_secrets.h` — minimum working config:

```c
#define MIMI_SECRET_WIFI_SSID           "your_wifi"
#define MIMI_SECRET_WIFI_PASS           "your_password"

#define MIMI_SECRET_TG_TOKEN            "your_telegram_bot_token"  // from @BotFather

#define MIMI_SECRET_API_KEY             "your_openrouter_key"
#define MIMI_SECRET_MODEL               "google/gemini-2.5-flash"
#define MIMI_SECRET_MODEL_PROVIDER      "openrouter"

#define MIMI_SECRET_DEEPGRAM_KEY        "your_deepgram_key"
#define MIMI_SECRET_SEARCH_KEY          "tvly-your_tavily_key"
```

### 3) Build and flash

```bash
idf.py set-target esp32s3
idf.py build
idf.py -p COM3 flash monitor        # Windows
idf.py -p /dev/ttyACM0 flash monitor  # Linux
```

---

## Free API services

| Service | Free tier | What it does |
|---------|-----------|-------------|
| [OpenRouter](https://openrouter.ai) | Free models available | LLM (Gemini 2.5 Flash) |
| [Deepgram](https://deepgram.com) | Free tier | Speech-to-text |
| [Tavily](https://tavily.com) | 1000 searches/month | Web search |
| [Telegram](https://t.me/BotFather) | Free | Chat interface |

---

## How to use

### Single tap
Tap the TTP223 sensor briefly → device takes a photo → AI describes what it sees → sends to Telegram.

### Hold to talk
Hold the sensor → recording starts after 500ms → speak → release → Deepgram transcribes → AI responds.  
The "🎤 You said: ..." echo appears first so you can confirm what was heard.

### CLI (serial monitor)
```text
mimi> wifi_status
mimi> chat hello
mimi> set_api_key YOUR_KEY
mimi> set_model_provider openrouter
mimi> set_model google/gemini-2.5-flash
mimi> set_tg_token 123456:ABCDEF...
mimi> set_search_key tvly-YOUR_TAVILY_KEY
mimi> config_show
mimi> restart
```

### Web UI
```
http://<device_ip>:18789/
```

---

## How it works

1. Boot → load `mimi_secrets.h` → override from NVS (CLI-set values)
2. Touch tap → `observe_scene` tool → vision API → Telegram
3. Touch hold → I2S PDM recording → stop on GPIO release → WAV assembled → Deepgram → agent loop → Telegram
4. All messages (Telegram / CLI / WebSocket) enter the same agent loop
5. Agent uses tools: `web_search`, `observe_scene`, `listen_and_transcribe`, `get_current_time`, file read/write, cron

---

## Links

- [CHANGES.md](CHANGES.md) — full list of modifications from upstream
- [Seeed XIAO ESP32S3 Sense](https://www.seeedstudio.com/XIAO-ESP32S3-p-5627.html)
- [Seeed Wiki](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/)
- Upstream repo: [luoluoter/mimiclaw](https://github.com/luoluoter/mimiclaw)
