#Python simulation of signal

import numpy as np
import matplotlib.pyplot as plt

# Parameters
amplitude = 2047
offset = 2047
samples = 256
frequency = 5.0           # Hz
noise_amplitude = 100

# Generate base sine harmonics
angles = 2 * np.pi * np.arange(samples) / samples
y1 = np.sin(angles)
y2 = np.sin(2 * angles)
y3 = np.sin(3 * angles)

# Composite waveform
y = 4 * y1 + 2 * y2 + y3
y /= np.max(np.abs(y))  # Normalize

# Scale and offset
clean_sine = offset + amplitude * y

# Function to generate noisy waveform for one cycle
def generate_noisy_cycle():
    noise = np.random.normal(0, noise_amplitude, samples)
    noisy = clean_sine + noise
    return np.clip(noisy, 0, 4095)

# Number of cycles to simulate
num_cycles = 20

# Generate multiple cycles concatenated
long_signal = np.concatenate([generate_noisy_cycle() for _ in range(num_cycles)])

# Time axis (in samples)
time_axis = np.arange(len(long_signal))

# Plot longer time waveform
plt.figure(figsize=(12, 5))
plt.plot(time_axis, long_signal, label='Noisy Composite Wave (20 cycles)')
plt.title(f'Noisy Composite Wave Over {num_cycles} Cycles')
plt.xlabel('Sample Number')
plt.ylabel('Amplitude (12-bit DAC units)')
plt.grid(True)
plt.legend()
plt.show()
