__author__ = 'RAGHU'

#! /bin/env python

import sys
import pymedia.audio.sound as sound
import time, wave
import subprocess

def convert2Wav(fname, fout):
    subprocess.call(['ffmpeg', '-i', fname, fout])

def playWAV(fname):
    f= wave.open( fname, 'rb' )
    sampleRate= f.getframerate()
    channels= f.getnchannels()
    format= sound.AFMT_S16_LE
    snd1= sound.Output( sampleRate, channels, format )
    s= ' '
    while len( s ):
        s= f.readframes( 1000 )
        snd1.play( s )
    # Since sound module is not synchronous we want everything to be played before we exit
    while snd1.isPlaying():
        time.sleep(0.05)