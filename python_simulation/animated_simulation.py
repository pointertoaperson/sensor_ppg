import numpy as np
from matplotlib import pyplot as plt
from matplotlib.animation import FuncAnimation
import adc_20ms_log

T = 20e-3
Fs = 50  # 50Hz sampling
samp_N = 256  # number of samples
freq_res = Fs / samp_N  # frequency resolution

population = adc_20ms_log.ADC_Value

# --- Helper Functions ---

def peak_detect(inp_arr):
    peak_Idx = []
    for i in range(1, len(inp_arr) - 1):
        if inp_arr[i - 1] < inp_arr[i] > inp_arr[i + 1]:
            peak_Idx.append(i)
    return peak_Idx

def calculate_HR(peaks):
    if len(peaks) < 2:
        return 0
    mean_buff = np.mean([peaks[i + 1] - peaks[i] for i in range(len(peaks) - 1)])
    return mean_buff

def confidence(filtered_arr, max_Idx):
    if max_Idx + 1 < len(filtered_arr):
        return filtered_arr[max_Idx] / np.mean(filtered_arr[max_Idx + 1:])
    return 0

# --- Plot Setup ---
fig, ax = plt.subplots()
line_raw, = ax.plot([], [], label='Raw', color='blue')
line_filtered, = ax.plot([], [], label='FFT+IFFT', color='red')
line_peaks, = ax.plot([], [], 'go', label='Peaks')
text_hr = ax.text(10, 150, '', fontsize=12, color='red')

ax.set_xlim(0, samp_N)
ax.set_ylim(-200, 200)  # Adjust based on your signal range
ax.set_xlabel('Sample Index')
ax.set_ylabel('Amplitude')
ax.legend()

# --- Animation Update Function ---
def update(frame_idx):
    start = frame_idx * samp_N
    end = start + samp_N
    if end > len(population):
        return line_raw, line_filtered, line_peaks, text_hr

    sample = np.array(population[start:end])
    sample = sample - np.mean(sample)

    freq_transform = np.fft.fft(sample)
    mag = np.abs(freq_transform)
    mag = mag[:samp_N // 2].copy()
    mag_copy = mag.copy()
    mag[:3] = 0
    mag[26:] = 0

    max_Idx = np.argmax(mag)
    freq_filtered = np.zeros_like(freq_transform)
    freq_filtered[max_Idx] = freq_transform[max_Idx]
    ifft_transform = np.fft.ifft(freq_filtered)
    ifft_real = np.real(ifft_transform)

    peaks = peak_detect(ifft_real)
    HR_val = 60 / (calculate_HR(peaks) * T) if len(peaks) >= 2 else 0
    conf_val = confidence(mag_copy, max_Idx)

    # Update plot data
    line_raw.set_data(np.arange(samp_N), sample)
    line_filtered.set_data(np.arange(samp_N), ifft_real)
    line_peaks.set_data(peaks, [ifft_real[i] for i in peaks])
    text_hr.set_text(f'HR: {HR_val:.2f} BPM | Conf: {conf_val:.2f}')

    return line_raw, line_filtered, line_peaks, text_hr

# --- Run and Save Animation ---
frame_count = len(population) // samp_N
ani = FuncAnimation(fig, update, frames=frame_count, interval=500, blit=True, repeat=False)

# Save as MP4 
#ani.save('heart_rate_animation.mp4', writer='ffmpeg', fps=2)

# Optional live display
plt.show()
