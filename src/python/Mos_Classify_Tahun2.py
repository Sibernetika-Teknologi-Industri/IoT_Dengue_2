#SARAS-Mos Classify
#V2.0.0
#Update : 
#- Using Dzaky and Daniel classification method (mel-spectrogram)
#- adapting some parameter to fit the node-red server needs 

import keras 
from sklearn.preprocessing import LabelEncoder
import librosa
import numpy as np
import argparse

# Running parameter function 
ap = argparse.ArgumentParser()
ap.add_argument('-i','--audio',required=True)
ap.add_argument('-j','--model',required=True)
args = vars(ap.parse_args())

def windows(data, window_size):
  start = 0
  while start < len(data):
    yield start, start + window_size
    start += (window_size // 2)

def extract_features_prediction(file_name):
    window_size = hop_length * (frames - 1)
    log_specgrams = []
    data,sample_rate = librosa.load(file_name)
    sound_clip, _ = librosa.load(file_name, sr=sample_rate) 
    for (start, end) in windows(sound_clip, window_size):
        if (len(sound_clip[start:end]) == window_size):
          signal = sound_clip[start:end]
          melspec = librosa.feature.melspectrogram(signal, n_mels=bands, sr=sample_rate, n_fft=n_fft, hop_length=hop_length)
          logspec = librosa.power_to_db(melspec, ref=np.max)
          logspec = logspec / 80 + 1
          logspec = logspec.T.flatten()[:, np.newaxis].T
          log_specgrams.append(logspec)
    features = np.asarray(log_specgrams).reshape(len(log_specgrams), bands, frames, 1)
    
    return features

def print_prediction(file_name):
    prediction_feature = extract_features_prediction(file_name) 

    predicted_vector = np.argmax(model.predict(prediction_feature), axis=-1)
    predicted_class = le.inverse_transform(predicted_vector)  

    predicted_proba_vector = model.predict(prediction_feature)
    predicted_proba = predicted_proba_vector[0]
    for i in range(len(predicted_proba_vector)):
        if (np.max(predicted_proba_vector[i]) == np.max(predicted_proba_vector)):
            predicted_proba = predicted_proba_vector[i]
            # print("The predicted class is:", predicted_class[i], '\n')
            
    for i in range(len(predicted_proba)): 
        category = le.inverse_transform(np.array([i]))
        # print(category[0], "\t\t : ", format(predicted_proba[i], '.16f'))
    
    aegyptii = predicted_proba[0] + predicted_proba[1]
    print("aegypti="+str(round((aegyptii*100),2))+"%&")
    # print(predicted_proba_vector)

model = keras.models.load_model(args["model"])
y = ['aedes_aegypti_jantan','aedes_aegypti_betina','culex_quinquefasciatus']
le = LabelEncoder()
le.fit(y)

bands = 60
frames = 60
hop_length = 512
n_fft = 1024

filename = args["audio"]
print_prediction(filename)