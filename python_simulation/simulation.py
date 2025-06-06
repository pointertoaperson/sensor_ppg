import numpy as np
from  matplotlib import pyplot as plt

import adc_20ms_log

T = 20e-3

Fs = 50 #50Hz sampling

samp_N = 256 #number of samples

freq_res = Fs/256 #frequency resolution


#function for peak detection
def peak_detect(inp_arr):
    peak_Idx = []
    for i in range(1,len(inp_arr)-1):
        if(inp_arr[i-1] < inp_arr[i] > inp_arr[i+1]):
            peak_Idx.append(i)
    return peak_Idx

#function for HR estimate calculation
def calculate_HR(peaks):
    delat = 0
    mean_buff = 0
    for i in range(len(peaks)-1):
        delta = peaks[i+1] - peaks[i]
        mean_buff += delta
    mean_buff = mean_buff/(len(peaks)-1)
    return mean_buff
        
 #function for confidence calculation
def confidence(filtered_arr):
    
    #find the ratio of max amplitude / noise floor
    return ((filtered_arr[max_Idx]/np.mean(filtered_arr[max_Idx+1:])) )
        

population = adc_20ms_log.ADC_Value
#taking 256 samples from the data file

sample =  np.resize(population,samp_N)
#remove DC component
sample = sample - np.mean(sample) 
#time axis
time = np.linspace(0,samp_N*20,samp_N)

plt.figure(1)
plt.plot(sample, label='sample', color='blue')

freq_transform = np.fft.fft(sample)
mag = np.abs(freq_transform)
mag = np.resize(mag,samp_N//2)

#save copy of FFT magnitutdes for Confidence
mag_copy = mag

#zero the bins below from 3 and bins above from 26
mag[:3] = 0
mag[26:] = 0

#check for the highest amplitude bins in these and get the index of it
max_Idx = np.argmax(mag)

#create a zero array same as freq_transform
freq_filtered = np.zeros_like(freq_transform)
#keep the max index and make all zero
freq_filtered[max_Idx] = freq_transform[max_Idx] 
#perfom IFFT to the filtered frequency
ifft_transform = np.fft.ifft(freq_filtered)

#plot the filtered signal
plt.plot(np.real(ifft_transform), label='FFT+IFFT', color='red')

#using only real part of the ifft
ifft_real = np.real(ifft_transform)

peaks = peak_detect(ifft_real)

#calculate Heart Rate
HR =  60/ (calculate_HR(peaks) * T )
conf = confidence(mag_copy) 


# mark the peaks
plt.plot( peaks,[ifft_real[i] for i in peaks],'go',label='peak')

#print the heart rate estimate
plt.text(0,50, f"HR estimate:{HR} BPM  confidence: {conf}", fontsize=12,color='red')
plt.xlabel('Time (ms)')
plt.ylabel('Amplitude')

plt.legend()

plt.show()