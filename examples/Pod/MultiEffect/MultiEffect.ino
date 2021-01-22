// Encoder: select effect
// Led: blue = reverb, green = delay, purple = bitcrush / LPF
// Reverb: Knob 1 = Dry/wet, Knob 2 = Reverb time
// Delay: Knob 1 = Delay time, Knob 2 = Feedback
// Crush/LPF: Knob 1 = LPF cutoff, Knob 2 = Downsample

#include "DaisyDuino.h"

// Set max delay time to 0.75 of samplerate.
#define MAX_DELAY static_cast<size_t>(48000 * 2.5f)
#define REV 0
#define DEL 1
#define CRU 2

static DaisyHardware pod;

static ReverbSc rev;
static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS dell;
static DelayLine<float, MAX_DELAY> DSY_SDRAM_BSS delr;
static Tone filter;
int mode = REV;

float sample_rate;
float currentDelay, feedback, delayTarget, cutoff;

int crushmod, crushcount;
float crushsl, crushsr, drywet;

// Helper functions
void Controls();

void GetReverbSample(float &outl, float &outr, float inl, float inr);

void GetDelaySample(float &outl, float &outr, float inl, float inr);

void GetCrushSample(float &outl, float &outr, float inl, float inr);

void AudioCallback(float **in, float **out, size_t size) {
  float outl, outr, inl, inr;

  Controls();

  // audio
  for (size_t i = 0; i < size; i ++) {
    inl = in[0][i];
    inr = in[1][i];

    switch (mode) {
    case REV:
      GetReverbSample(outl, outr, inl, inr);
      break;
    case DEL:
      GetDelaySample(outl, outr, inl, inr);
      break;
    case CRU:
      GetCrushSample(outl, outr, inl, inr);
      break;
    default:
      outl = outr = 0;
    }

    // left out
    out[0][i] = outl;

    // right out
    out[1][i] = outr;
  }
}

void setup() {
  // Inits and sample rate
  pod = DAISY.init(DAISY_POD, AUDIO_SR_48K);
  sample_rate = DAISY.get_samplerate();
  rev.Init(sample_rate);
  dell.Init();
  delr.Init();
  filter.Init(sample_rate);

  // reverb parameters
  rev.SetLpFreq(18000.0f);
  rev.SetFeedback(0.85f);

  // delay parameters
  currentDelay = delayTarget = sample_rate * 0.75f;
  dell.SetDelay(currentDelay);
  delr.SetDelay(currentDelay);

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {}

void UpdateKnobs(float &k1, float &k2) {
  k1 = analogRead(PIN_POD_POT_1) / 1023.f;
  k2 = analogRead(PIN_POD_POT_2) / 1023.f;

  float m = (float)MAX_DELAY - .05 * sample_rate;

  switch (mode) {
  case REV:
    drywet = k1;
    rev.SetFeedback(k2);
    break;
  case DEL:
	  delayTarget = k1 * m + .05 * sample_rate;
    feedback = k2;
    break;
  case CRU:
    cutoff = k1 * 19500.f + 500.f;
    filter.SetFreq(cutoff);
    crushmod = (int) (k2 * 20.f);
  }
}

void UpdateEncoder() {
  mode = mode + pod.encoder.Increment();
  mode = (mode % 3 + 3) % 3;
}

void UpdateLeds(float k1, float k2) {
  pod.leds[0].Set(mode == 2, mode == 1, mode == 0 || mode == 2);
  pod.leds[1].Set(mode == 2, mode == 1, mode == 0 || mode == 2);
}

void Controls() {
  float k1, k2;
  delayTarget = feedback = drywet = 0;

  pod.DebounceControls();

  UpdateKnobs(k1, k2);

  UpdateEncoder();

  UpdateLeds(k1, k2);
}

void GetReverbSample(float &outl, float &outr, float inl, float inr) {
  rev.Process(inl, inr, &outl, &outr);
  outl = drywet * outl + (1 - drywet) * inl;
  outr = drywet * outr + (1 - drywet) * inr;
}

void GetDelaySample(float &outl, float &outr, float inl, float inr) {
  fonepole(currentDelay, delayTarget, .00007f);
  delr.SetDelay(currentDelay);
  dell.SetDelay(currentDelay);
  outl = dell.Read();
  outr = delr.Read();

  dell.Write((feedback * outl) + inl);
  outl = (feedback * outl) + ((1.0f - feedback) * inl);

  delr.Write((feedback * outr) + inr);
  outr = (feedback * outr) + ((1.0f - feedback) * inr);
}

void GetCrushSample(float &outl, float &outr, float inl, float inr) {
  crushcount++;
  crushcount %= crushmod;
  if (crushcount == 0) {
    crushsr = inr;
    crushsl = inl;
  }
  outl = filter.Process(crushsl);
  outr = filter.Process(crushsr);
}