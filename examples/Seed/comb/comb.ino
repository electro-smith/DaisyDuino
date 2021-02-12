#include "DaisyDuino.h"

static DaisyHardware seed;
Comb flt;
static WhiteNoise noise;

float buf[9600];

static void AudioCallback(float **in, float **out, size_t size) {
  float output;
  for (size_t i = 0; i < size; i++) {
    output = noise.Process();

    output = 0.5 * flt.Process(output);

    out[0][i] = out[1][i] = output;
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
