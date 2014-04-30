__author__ = 'RAGHU'

from PySide.QtCore import *
from PySide.QtGui import *
from os import listdir
from os.path import isfile, join
from multiprocessing import Process

import sys
import os
import QtDesign
import play
import beatDetection
import communicate

class MainDialog(QDialog, QtDesign.Ui_Form):

    def __init__(self, parent = None):
        super(MainDialog, self).__init__(parent)
        self.setupUi(self)
        self.refreshList()
        self.connect(self.preview, SIGNAL("clicked()"), self.previewMusic)
        self.connect(self.refresh, SIGNAL("clicked()"), self.refreshList)
        self.connect(self.graphs, SIGNAL("clicked()"), self.showGraphs)
        self.connect(self.execute, SIGNAL("clicked()"), self.executeProcess)
        self.connect(self.musicFiles, SIGNAL("itemClicked (QListWidgetItem *)"),self.showItemClicked)
        self.connect(self.musicFiles, SIGNAL("currentItemChanged (QListWidgetItem *,QListWidgetItem *)"),self.showItemChanged)
        self.musicFiles.setCurrentItem(self.musicFiles.item(0))
        self.console.clear()

    def previewMusic(self):
        print "Inside Music"
        self.console.append("Playing Music file:" + self.path.text())
        play.playWAV(self.path.text())

    def refreshList(self):
        print "Refreshing List"
        self.console.append("Refreshing Music File list")
        self.musicFiles.clear()
        files = [ f for f in listdir("Music") if isfile(join("Music",f)) ]
        self.musicFiles.addItems(files)

    def showGraphs(self):
        self.console.append("About to show Graphs")
        beatDetection.drawGraphs(self.path.text())

    def executeProcess(self):
        self.console.append("Started Beat detection")
        print "Inside Main Process"
        musicArray, energyArray, finalBeatArray, beatArray, beatPosition = beatDetection.detectBeats(self.path.text())
        print beatPosition
        numBeatsInPeriod = beatDetection.detectPeriod(musicArray, energyArray, finalBeatArray, beatArray)
        print numBeatsInPeriod

        self.console.append("Started Dancing")

        if self.path.text() == join("Music","med1.wav"):
            communicate.sendChar('a', beatPosition[1] - beatPosition[0])
            communicate.sendChar('s', beatPosition[2] - beatPosition[1])
            communicate.sendChar('m', beatPosition[3] - beatPosition[2])
            communicate.sendChar('n', beatPosition[4] - beatPosition[3])
            communicate.sendChar('s', beatPosition[5] - beatPosition[4])
            communicate.sendChar('c', beatPosition[6] - beatPosition[5])
            communicate.sendChar('s', beatPosition[7] - beatPosition[6])
            communicate.sendChar('m', beatPosition[8] - beatPosition[7])
            communicate.sendChar('n', beatPosition[9] - beatPosition[8])
            communicate.sendChar('s', beatPosition[10] - beatPosition[9])

            communicate.sendChar('a', beatPosition[11] - beatPosition[10])
            communicate.sendChar('s', beatPosition[12] - beatPosition[11])
            communicate.sendChar('m', beatPosition[13] - beatPosition[12])
            communicate.sendChar('n', beatPosition[14] - beatPosition[13])
            communicate.sendChar('s', beatPosition[15] - beatPosition[14])
            communicate.sendChar('c', beatPosition[16] - beatPosition[15])
            communicate.sendChar('s', beatPosition[17] - beatPosition[16])
            communicate.sendChar('m', beatPosition[18] - beatPosition[17])
            communicate.sendChar('n', beatPosition[19] - beatPosition[18])
            communicate.sendChar('s', 10)
        elif self.path.text() == join("Music","med_200bpm.wav"):
            communicate.sendChar('i', beatPosition[1] - beatPosition[0])
            communicate.sendChar('j', beatPosition[2] - beatPosition[1])
            communicate.sendChar('k', beatPosition[3] - beatPosition[2])
            communicate.sendChar('l', beatPosition[4] - beatPosition[3])
            communicate.sendChar('m', beatPosition[5] - beatPosition[4])
            communicate.sendChar('n', beatPosition[6] - beatPosition[5])
            communicate.sendChar('i', beatPosition[7] - beatPosition[6])
            communicate.sendChar('j', beatPosition[8] - beatPosition[7])
            communicate.sendChar('k', beatPosition[9] - beatPosition[8])
            communicate.sendChar('l', beatPosition[10] - beatPosition[9])

            communicate.sendChar('m', beatPosition[11] - beatPosition[10])
            communicate.sendChar('n', beatPosition[12] - beatPosition[11])
            communicate.sendChar('i', beatPosition[13] - beatPosition[12])
            communicate.sendChar('j', beatPosition[14] - beatPosition[13])
            communicate.sendChar('k', beatPosition[15] - beatPosition[14])
            communicate.sendChar('l', beatPosition[16] - beatPosition[15])
            communicate.sendChar('m', beatPosition[17] - beatPosition[16])
            communicate.sendChar('n', beatPosition[18] - beatPosition[17])
            communicate.sendChar('i', beatPosition[19] - beatPosition[18])
            communicate.sendChar('j', beatPosition[20] - beatPosition[19])

            communicate.sendChar('i', beatPosition[21] - beatPosition[20])
            communicate.sendChar('j', beatPosition[22] - beatPosition[21])
            communicate.sendChar('k', beatPosition[23] - beatPosition[22])
            communicate.sendChar('l', beatPosition[24] - beatPosition[23])
            communicate.sendChar('m', beatPosition[25] - beatPosition[24])
            communicate.sendChar('n', beatPosition[26] - beatPosition[25])
            communicate.sendChar('i', beatPosition[27] - beatPosition[26])
            communicate.sendChar('j', beatPosition[28] - beatPosition[27])
            communicate.sendChar('k', beatPosition[29] - beatPosition[28])
            communicate.sendChar('l', beatPosition[30] - beatPosition[29])

            communicate.sendChar('m', beatPosition[31] - beatPosition[30])
            communicate.sendChar('n', beatPosition[32] - beatPosition[31])
            communicate.sendChar('i', beatPosition[33] - beatPosition[32])
            communicate.sendChar('j', beatPosition[34] - beatPosition[33])
            communicate.sendChar('k', beatPosition[35] - beatPosition[34])
            communicate.sendChar('l', beatPosition[36] - beatPosition[35])
            communicate.sendChar('m', beatPosition[37] - beatPosition[36])
            communicate.sendChar('n', beatPosition[38] - beatPosition[37])
            communicate.sendChar('i', beatPosition[39] - beatPosition[38])
            communicate.sendChar('j', beatPosition[40] - beatPosition[39])
        else:
            i = 1
            while i < beatPosition.__len__():
                communicate.sendChar('a', beatPosition[i] - beatPosition[i-1])
                i = i + 1
            print "Done !"

    def showItemClicked(self, item):
        print "Inside Selected"
        self.selected(str(item.text()))

    def showItemChanged(self, citem, pitem):
        print "Inside Item Changed"
        if citem:
            self.selected(str(citem.text()))

    def selected(self, fileName):
        self.console.append("Item Selected: " + fileName)
        print "Inside Selected"
        self.path.setEnabled(True)
        self.preview.setEnabled(True)
        self.graphs.setEnabled(True)
        self.console.setEnabled(True)
        self.execute.setEnabled(True)
        self.path.setText(os.path.abspath(join("Music",fileName)))


if __name__ == '__main__':
    app = QApplication(sys.argv)
    form = MainDialog()
    form.show()
    app.exec_()