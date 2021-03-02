// Requires the arduino MIDI Library by Francois Best and lathoub.
// FortySevenEffects on Github This example simply triggers the envelope and
// changes the note when MIDI messages come in It works with the TRS midi on the
// Daisy Pod, NOT the USB Midi
#include <MIDI.h>

#include "DaisyDuino.h"
#include <string>

MIDI_CREATE_DEFAULT_INSTANCE();

DaisyHardware hw;

class Voice {
public:
  Voice() {}
  ~Voice() {}
  void Init(float samplerate) {
    active_ = false;
    osc_.Init(samplerate);
    osc_.SetAmp(0.75f);
    osc_.SetWaveform(Oscillator::WAVE_POLYBLEP_SAW);
    env_.Init(samplerate);
    env_.SetSustainLevel(0.5f);
    env_.SetTime(ADSR_SEG_ATTACK, 0.005f);
    env_.SetTime(ADSR_SEG_DECAY, 0.005f);
    env_.SetTime(ADSR_SEG_RELEASE, 0.2f);
    filt_.Init(samplerate);
    filt_.SetFreq(6000.f);
    filt_.SetRes(0.6f);
    filt_.SetDrive(0.8f);
  }

  float Process() {
    if (active_) {
      float sig, amp;
      amp = env_.Process(env_gate_);
      if (!env_.IsRunning())
        active_ = false;
      sig = osc_.Process();
      filt_.Process(sig);
      return filt_.Low() * (velocity_ / 127.f) * amp;
    }
    return 0.f;
  }

  void OnNoteOn(float note, float velocity) {
    note_ = note;
    velocity_ = velocity;
    osc_.SetFreq(mtof(note_));
    active_ = true;
    env_gate_ = true;
  }

  void OnNoteOff() { env_gate_ = false; }

  void SetCutoff(float val) { filt_.SetFreq(val); }

  inline bool IsActive() const { return active_; }
  inline float GetNote() const { return note_; }

private:
  Oscillator osc_;
  Svf filt_;
  Adsr env_;
  float note_, velocity_;
  bool active_;
  bool env_gate_;
};

template <size_t max_voices> class VoiceManager {
public:
  VoiceManager() {}
  ~VoiceManager() {}

  void Init(float samplerate) {
    for (size_t i = 0; i < max_voices; i++) {
      voices[i].Init(samplerate);
    }
  }

  float Process() {
    float sum;
    sum = 0.f;
    for (size_t i = 0; i < max_voices; i++) {
      sum += voices[i].Process();
    }
    return sum;
  }

  void OnNoteOn(float notenumber, float velocity) {
    Voice *v = FindFreeVoice();
    if (v == NULL)
      return;
    v->OnNoteOn(notenumber, velocity);
  }

  void OnNoteOff(float notenumber, float velocity) {
    for (size_t i = 0; i < max_voices; i++) {
      Voice *v = &voices[i];
      if (v->IsActive() && v->GetNote() == notenumber) {
        v->OnNoteOff();
      }
    }
  }

  void FreeAllVoices() {
    for (size_t i = 0; i < max_voices; i++) {
      voices[i].OnNoteOff();
    }
  }

  void SetCutoff(float all_val) {
    for (size_t i = 0; i < max_voices; i++) {
      voices[i].SetCutoff(all_val);
    }
  }

private:
  Voice voices[max_voices];
  Voice *FindFreeVoice() {
    Voice *v = NULL;
    for (size_t i = 0; i < max_voices; i++) {
      if (!voices[i].IsActive()) {
        v = &voices[i];
        break;
      }
    }
    return v;
  }
};

static VoiceManager<24> voice_handler;

void AudioCallback(float *in, float *out, size_t size) {
  float sum = 0.f;
  hw.ProcessDigitalControls();
  hw.ProcessAnalogControls();
  if (hw.buttons[0].FallingEdge()) {
    voice_handler.FreeAllVoices();
  }
  voice_handler.SetCutoff(250.f + hw.GetKnobValue(0) * 8000.f);

  for (size_t i = 0; i < size; i += 2) {
    sum = 0.f;
    sum = voice_handler.Process() * 0.5f;
    out[i] = sum;
    out[i + 1] = sum;
  }
}

void handleNoteOn(byte inChannel, byte inNote, byte inVelocity) {
  // Note Off can come in as Note On w/ 0 Velocity
  if (inVelocity == 0.f) {
    voice_handler.OnNoteOff(inNote, inVelocity);
  } 
  else {
    voice_handler.OnNoteOn(inNote, inVelocity);
  }
}


void  handleNoteOff(byte inChannel, byte inNote, byte inVelocity) {
  voice_handler.OnNoteOff(inNote, inVelocity);
}


// Main -- Init, and Midi Handling
void setup() {
  // Init
  float samplerate;
  hw = DAISY.init(DAISY_FIELD, AUDIO_SR_48K);
  samplerate = DAISY.AudioSampleRate();
  voice_handler.Init(samplerate);

  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Listen to all incoming messages

  DAISY.begin(AudioCallback);
}
void loop() {
  MIDI.read();
}