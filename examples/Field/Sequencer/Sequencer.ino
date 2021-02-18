#include "daisy_field.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

class Sequence{
	public:
		Sequence() {}
		~Sequence() {}
	
		void Init(float sr){
			step_ = 0;
			len_ = 8;
			isFwd = true;
			freq_ = .5f;
			state_ = 0;
			
			for(int i = 0; i < 8; i++){
				vals_[i] = 0.f;
				gates_[i] = false;
			}

			tick_.Init(2.f, sr);
		}

		void Tick(){
			state_ <<= 1;
			if(tick_.Process()){
				if(isFwd){
					Inc();
				}
				else{
					Dec();
				}
				state_ = gates_[step_] ? 0xff : 0;
			}
		}

		void Inc() { step_ = (step_ + 1) % len_; }

		void Dec(){ 
			step_--;
			if(step_ == 255){
				step_ = len_ - 1;
			}
		}

		//fwd rev and speed in one control
		void SetFreq(int inc){
			freq_ += inc * .005f;
			freq_ = fclamp(freq_, -1.f, 1.f);
			tick_.SetFreq(fabsf(freq_) * 10.f + .25f);
			isFwd = freq_ > 0.f;
		}

		//public vars make easy testing
		uint8_t step_;
		uint8_t len_;
		float vals_[8];
		bool gates_[8];
		bool isFwd;
		uint8_t state_;

		float freq_;

		Metro tick_;
	
	private:
};

Sequence seq;

DaisyField hw;
void AudioCallback(float **in, float **out, size_t size)
{
	seq.Tick();
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = out[1][i] = 0.f;
	}
}

int main(void)
{
	hw.Init();
	float cr = hw.AudioCallbackRate();
	
	seq.Init(cr);
	
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	while(1) {
		hw.ProcessAllControls();

		//everything that happens 8 times
		for(int i = 0; i < 8; i++){
			seq.gates_[i] ^= hw.KeyboardRisingEdge(i);
			
			hw.led_driver.SetLed(i, 0.f);
			hw.led_driver.SetLed(15 - i, (float)seq.gates_[i]);
			hw.led_driver.SetLed(i + 16, hw.GetKnobValue(i));
		
			seq.vals_[i] = hw.GetKnobValue(i);
		}

		hw.led_driver.SetLed(seq.step_, 1.f);

		seq.SetFreq(hw.sw[1].Pressed());
		seq.SetFreq(hw.sw[0].Pressed() * -1.f);

		hw.SetCvOut1(seq.vals_[seq.step_] * 4095);
		dsy_gpio_write(&hw.gate_out, seq.state_ > 0);
		
		hw.led_driver.SwapBuffersAndTransmit();		
		hw.DelayMs(6);
	}
}
