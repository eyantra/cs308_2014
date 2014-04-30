__author__ = 'RAGHU'

import wave
import contextlib
import numpy as np
import matplotlib.pyplot as plt

def scale(OldList, NewMin, NewMax):
    NewRange = float(NewMax - NewMin)
    OldMin = min(OldList)
    OldMax = max(OldList)
    OldRange = float(OldMax - OldMin)
    ScaleFactor = NewRange / OldRange
    NewList = []
    for OldValue in OldList:
        NewValue = ((OldValue - OldMin) * ScaleFactor) + NewMin
        NewList.append(NewValue)
    return NewList

def detectBeats(wavfilePath):
    # Reading Music file and store it in variable musicArray
    wr = wave.open(wavfilePath, 'r')
    numFrames = wr.getnframes()
    musicArray = np.fromstring(wr.readframes(numFrames), dtype=np.int16).tolist()

    sampleSize = 1024
    numSamples = int(musicArray.__len__()/sampleSize)
    print "Music Array Size: ", musicArray.__len__()
    print "Number of Samples: ", numSamples

#    plt.plot(musicArray)
#    plt.ylabel('Amplitude')
#    plt.show()

    # Calculating Energy array
    energyArray = []
    for i in range(0, numSamples):
        sampleEnergy = 0
        for j in range(0,sampleSize):
            amplitude = musicArray[(i*sampleSize) + j]
            sampleEnergy = sampleEnergy + (amplitude * amplitude)
        energyArray.append(sampleEnergy)

#    plt.plot(energyArray)
#    plt.ylabel('Energy')
#    plt.show()

    energyArray = scale(energyArray, 0.0, 1.0)

    avgEnrg = 0
    var = 0
    beats = []
    for i in range(0, 43):
            avgEnrg = avgEnrg + energyArray[i]

    for i in range(0, 43):
            var = var + ((energyArray[i] - (avgEnrg/43)) * (energyArray[i] - (avgEnrg/43)))

    var = var/43
    cValue = (-0.0025714*var) + 1.5142857

    for i in range(0, 43):
            if(energyArray[i] > ((avgEnrg/43)*cValue)):
                    beats.append(1)
            else:
                    beats.append(0)

    for i in range(43, energyArray.__len__()):
            avgEnrg = avgEnrg - energyArray[i-43]
            avgEnrg = avgEnrg + energyArray[i]
            var = 0
            for j in range(i-43, i):
                    var = var + ((energyArray[j] - (avgEnrg/43)) * (energyArray[j] - (avgEnrg/43)))
            var = var/43
            cValue = (-0.0025714*var) + 1.5142857
            if(energyArray[i] > ((avgEnrg/43)*cValue)):
                    beats.append(1)
            else:
                    beats.append(0)

#    plt.plot(beats)
#    plt.ylabel('Beat')
#    plt.show()

    beatArray = []
    minBeatRepeat = 6
    i = 0
    while i < beats.__len__():
            if(beats[i] == 1):
                    beatArray.append(i)
                    i = i + minBeatRepeat
            else:
                    i = i + 1

    finalBeatArray = [0] * beats.__len__()
    for beatPos in beatArray:
        finalBeatArray[beatPos] = 1


    beatPosition = []
    frameRate = 0
    with contextlib.closing(wave.open(wavfilePath,'r')) as f:
        frameRate = f.getframerate()

    for beatPos in beatArray:
        temp = round(beatPos*1024/((float)(frameRate/1000)))
        beatPosition.append(temp)

    return musicArray, energyArray, finalBeatArray, beatArray, beatPosition


def detectPeriod(musicArray, energyArray, finalBeatArray, beatArray):
    maxBeatsPerPeriod = 10
    periodThreshold = 50
    numBeats  = 0
    for i in finalBeatArray:
        if i == 1:
            numBeats = numBeats + 1

    stdev = [0] * maxBeatsPerPeriod
    for i in range(0, maxBeatsPerPeriod):
        sqSum = 0
        s = 0
        for j in range(0, numBeats - maxBeatsPerPeriod):
            sqSum = sqSum + ((beatArray[j+i+1] - beatArray[j])*(beatArray[j+i+1] - beatArray[j]))
            s = s + (beatArray[j+i+1] - beatArray[j]);
        s = s/(numBeats-maxBeatsPerPeriod)
        sqSum = sqSum/(numBeats-maxBeatsPerPeriod)
        stdev[i] = sqSum - (s*s);

    print stdev
    numBeatsInPeriod = 0
    '''
    for k in range(0, maxBeatsPerPeriod):
        if(stdev[k] < periodThreshold):
            numBeatsInPeriod = k + 1
            break
    '''
    numBeatsInPeriod = stdev.index(min(stdev)) + 1
    return numBeatsInPeriod




def drawGraphs(wavfilePath):
    musicArray, energyArray, finalBeatArray, beatArray, beatPosition = detectBeats(wavfilePath)
    plt.subplot(3,1,1)
    plt.plot(musicArray)
    plt.xlabel("Time")
    plt.ylabel("Amplitude")
    plt.subplot(3,1,2)
    plt.plot(energyArray)
    plt.xlabel("Time")
    plt.ylabel("Energy")
    plt.subplot(3,1,3)
    plt.plot(finalBeatArray)
    plt.xlabel("Time")
    plt.ylabel("Beat")
    plt.show()