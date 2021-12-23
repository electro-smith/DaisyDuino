/* AudioSettings.ino

author: beserge

This example shows how to set custom audio settings (callback rate, block size)
Once it sets up the custom audio, it simply passes audio through from the input to the output.

To test this example, you should connect audio jacks to the Patch SM's audio IO.
Check the figures from the Patch SM datasheet for help connecting peripherals.
You can also use the patch.Init().

*/

#include "DaisyDuino.h"

DaisyHardware patch;

/** Callback for processing and synthesizing audio
 *
 *  The default size is very small (just 4 samples per channel). This means the
 * callback is being called at 16kHz.
 *
 *  This size is acceptable for many applications, and provides an extremely low
 * latency from input to output. However, you can change this size by calling
 * patch.SetAudioBlockSize(desired_size). When running complex DSP it can be more
 * efficient to do the processing on larger chunks at a time.
 *
 */
static void AudioCallback(float**  in, float** out, size_t size)
{
    for(size_t i = 0; i < size; i++)
    {
        /** set the left output to the current left input */
        OUT_L[i] = IN_L[i];

        /** set the right output to the current right input */
        OUT_R[i] = IN_R[i];
    }
}

/** Start here */
void setup(){
    /** Initialize the DaisyHardware object 
    * Set the board to the Patch SM
    * Sets the samplerate to its default of 48kHz.
    * Sets the blocksize to its default of 4 samples.
    */
    patch = DAISY.init(DAISY_PATCH_SM);

    /** Set the samplerate to 96kHz */
    DAISY.SetAudioSampleRate(SaiHandle::Config:: SampleRate::SAI_96KHZ);

    /** Set the blocksize to 8 samples */
    DAISY.SetAudioBlockSize(8);

    /** Start the audio callback */
    DAISY.StartAudio(AudioCallback);
}

/** Loop forever */
void loop(){}