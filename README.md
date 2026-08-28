# Ark

Ark is a local voice interaction library for video games.

It provides Siri-like conversational capabilities for games by combining speech recognition, a local language model, and text-to-speech into a single pipeline for interactive dialogue with game characters.

Ark is designed to run entirely locally, without requiring cloud APIs.

## Overview

Ark provides the following interaction pipeline:

```text
Player Speech
     |
     v
    ASR
     |
     v
Local LLM
     |
     v
    TTS
     |
     v
Character Speech
```

This allows players to speak naturally to in-game characters and receive dynamically generated spoken responses.

Ark is intended to be engine-independent and can be integrated with multiple game engines.

## Features

* Fully local speech and language processing
* Real-time interactive dialogue
* Local automatic speech recognition
* Local LLM inference
* Local text-to-speech
* Bring-your-own voice model support
* Plug-and-play voice configuration
* Multiple game engine integrations
* Multiplayer support
* Peer-to-peer compute distribution
* No cloud API dependency

## Multiplayer

For multiplayer games, Ark can distribute the ASR, LLM, and TTS workload between peers.

```text
Player A                 Player B
--------                 --------

  ASR                       TTS
   |                         ^
   v                         |
  LLM  <---- P2P ---->      LLM
   |                         ^
   v                         |
  TTS                       ASR
```

Individual stages of the voice pipeline can be executed by different peers, allowing available compute resources to be shared across a multiplayer session.

This makes it possible to reduce redundant inference workloads and better utilize the hardware available across connected players.

## Tech Stack

| Component | Technology  | Purpose                               |
| --------- | ----------- | ------------------------------------- |
| ASR       | whisper.cpp | Local speech-to-text                  |
| LLM       | llama.cpp   | Local language model inference        |
| TTS       | Piper       | Local text-to-speech                  |
| Voice     | Pluggable   | Bring your own compatible voice model |

### whisper.cpp

Used for local automatic speech recognition and converting player speech into text.

### llama.cpp

Used for running language models locally and generating character dialogue.

### Piper

Used for fast local text-to-speech generation.

### Voice Models

Ark's voice layer is designed to be interchangeable.

Games can provide their own compatible voice models, allowing individual characters to use different voices without changing the rest of the dialogue pipeline.

## Architecture

```text
              Ark
               |
      +--------+--------+
      |        |        |
     ASR      LLM      TTS
      |        |        |
 whisper.cpp llama.cpp Piper
      |
      +----------------------+
                             |
                      Voice Models
```

Game integrations communicate with Ark through a common interface rather than directly depending on the underlying inference implementations.

## Game Engine Integration

Ark is designed as a standalone library rather than being tied to a specific engine.

Engine-specific integrations can expose Ark functionality to engines such as:

* Unreal Engine
* Unity
* Godot
* Custom engines

The core dialogue pipeline remains independent of the game engine.

## Goals

Ark is built around a few core principles:

* Local-first inference
* Low dependency on external services
* Engine independence
* Replaceable AI components
* Efficient multiplayer compute sharing
* Simple integration into existing games

## Status

Ark is currently under development.
