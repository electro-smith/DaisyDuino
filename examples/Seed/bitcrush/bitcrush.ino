// Title: bitcrush
// Description: Steps through different bitdepth and downsample settings
// Hardware: Daisy Seed
// Author: Ben Sergentanis

#include "DaisyDuino.h"

DaisyHardware hw;

size_t num_channels;

static Oscillator osc, lfo;
static Bitcrush bitcrush;
static Metro metro;

uint8_t depth;
void MyCallback(float **in, float **out, size_t size) {
  float sig;
  for (size_t i = 0; i < size; i++) {

    sig = osc.Process();

    if (metro.Process()) {
      depth++;
      depth %= 6;
    }

    bitcrush.SetBitDepth(depth + 2);
    bitcrush.SetCrushRate((depth + 2) * 2000);
    sig = bitcrush.Process(sig);

    for (size_t chn = 0; chn < num_channels; chn++) {
      out[chn][i] = sig;
    }
  }
}

void setup() {
  float sample_rate;
  // Initialize for Daisy pod at 48kHz
  hw = DAISY.init(DAISY_SEED, AUDIO_SR_48K);
  num_channels = hw.num_channels;
  sample_rate = DAISY.get_samplerate();

  depth = 1;
  osc.Init(sample_rate);
  bitcrush.Init(sample_rate);
  metro.Init(1.0f, sample_rate);

  // Set parameters for oscillator
  osc.SetWaveform(osc.WAVE_SIN);
  osc.SetFreq(440);
  osc.SetAmp(0.5);

  // set parameters for bitcrusher
  bitcrush.SetBitDepth(6);
  bitcrush.SetCrushRate(10000);

  DAISY.begin(MyCallback);
}

void loop() {}
