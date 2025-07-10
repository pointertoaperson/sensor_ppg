/*******************Arduino Script for generating test PPG signal 
********************using DAC************************************
*****************************************************************/

#define AMP_FREQ0 1
#define AMP_FREQ3 0.2f
#define AMP_FREQ5 0.1f
#define AMP_NOISE 0.3f

const int amplitude = 2047;
const int offset = 2047;
const int samples = 256;
volatile uint16_t sineWave[samples];
volatile int sampleIdx = 0;

const float HR = 55;
const float freq_0 = HR / 60.0f;
const float freq_3 = 3 * freq_0;
const float freq_5 = 5 * freq_0;

const float sampleRate = samples * freq_0;  // ~2837 Hz
const int noiseAmplitude = 100;

void setup() {
  analogWriteResolution(12);
  randomSeed(analogRead(0));
  generateSineWave();

  // Start Timer Counter for 2837 Hz interrupts
  const uint32_t rc = (84000000 / 2) / sampleRate;  // Due clock: 84 MHz, TC clock = 42 MHz
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(ID_TC0);
  TC_Configure(TC0, 0, TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC);
  TC_SetRC(TC0, 0, rc);
  TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
  TC0->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;
  NVIC_EnableIRQ(TC0_IRQn);
  TC_Start(TC0, 0);
}

void loop() {
  // Nothing here — interrupt handles output
}

void TC0_Handler() {
  TC_GetStatus(TC0, 0); // Must be called to clear interrupt flag
  analogWrite(DAC1, sineWave[sampleIdx]);
  sampleIdx++;
  if (sampleIdx >= samples) sampleIdx = 0;
}

void generateSineWave() {
  for (int i = 0; i < samples; i++) {
    float angle = (2 * PI * i) / samples;
    float noise = gaussianRandom() * noiseAmplitude;
    int noisySample = offset + amplitude * (
                        AMP_FREQ0 * sin(angle) +
                        AMP_FREQ3 * sin((freq_3 / freq_0) * angle) +
                        AMP_FREQ5 * sin((freq_5 / freq_0) * angle)
                      ) + AMP_NOISE * noise;

    if (noisySample < 0) noisySample = 0;
    if (noisySample > 4095) noisySample = 4095;
    sineWave[i] = noisySample;
  }
}

float gaussianRandom() {
  static bool hasSpare = false;
  static float spare;

  if (hasSpare) {
    hasSpare = false;
    return spare;
  }

  hasSpare = true;
  float u, v, s;
  do {
    u = 2.0 * random(0, 10000) / 10000.0 - 1.0;
    v = 2.0 * random(0, 10000) / 10000.0 - 1.0;
    s = u * u + v * v;
  } while (s >= 1.0 || s == 0.0);

  s = sqrt(-2.0 * log(s) / s);
  spare = v * s;
  return u * s;
}
