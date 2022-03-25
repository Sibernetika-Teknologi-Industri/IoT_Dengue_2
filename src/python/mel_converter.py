import librosa
import librosa.display
import matplotlib.pyplot as plt
import base64
import argparse
import numpy as np

# constant parameter
bands = 60
frames = 60
hop_length = 512
n_fft = 1024

# Running function parameters
ap = argparse.ArgumentParser()
ap.add_argument('-s','--sound',required=True) #lokasi folder sound file, hingga tanda "/" akhir
ap.add_argument('-n','--node',required=True) #nomor node
args = vars(ap.parse_args())

def create_mel_spectrogram(file_name, node):
  window_size = hop_length * (frames - 1)
  log_specgrams = []
  file_loc = file_name + "suara" + node + ".m4a"
  data,sample_rate = librosa.load(file_loc)
  sound_clip, _ = librosa.load(file_loc, sr=sample_rate)
  # Plotting the short-time Fourier Transformation
  plt.figure(figsize=(20, 5))
  # Using librosa.display.specshow() to create our spectrogram
  melspec = librosa.feature.melspectrogram(sound_clip, n_mels=bands, sr=sample_rate, n_fft=n_fft, hop_length=hop_length)
  logspec = librosa.power_to_db(melspec, ref=np.max)
  librosa.display.specshow(logspec,sr=sample_rate,x_axis='time',y_axis='hz',cmap='magma', hop_length=hop_length)
  plt.colorbar(label='Amplitude')
  plt.title('Spectrogram (amp)', fontdict=dict(size=18))
  plt.xlabel('Time', fontdict=dict(size=15))
  plt.ylabel('Frequency', fontdict=dict(size=15))
  plt.savefig(file_name+"Spec"+node+".png")

create_mel_spectrogram(args["sound"], args["node"])
file_ = args["sound"]+"Spec"+args["node"]+".png"

with open(file_, "rb") as image_file:
    encoded_string = base64.b64encode(image_file.read())

print(encoded_string)