#include "DaisyDuino.h"

static DaisyHardware seed;
Comb flt;
static WhiteNoise noise;

float buf[9600];

static void AudioCallback(float *in, float *out, size_t size) {
  float output;
  for (size_t i = 0; i < size; i += 2) {
    output = noise.Process();

    output = 0.5 * flt.Process(output);

    // left out
    out[i] = output;

    // right out
    out[i + 1] = output;
  }
}

void setup() {
  // initialize seed hardware and daisysp modules
  float sample_rate;
  seed = DAISY.init(DAISY_SEED, AUDIO_SR_48K);

  sample_rate = DAISY.get_samplerate();

  for (int i = 0; i < 9600; i++) {
    buf[i] = 0.0f;
  }

  // initialize Comb object
  flt.Init(sample_rate, buf, 9600);
  flt.SetFreq(500);

  // initialize noise
  noise.Init();

  // start callback
  DAISY.begin(AudioCallback);
}

void loop() {}
