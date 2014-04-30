__author__ = 'RAGHU'

# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'QtDesign.ui'
#
# Created: Mon Apr 07 20:51:35 2014
#      by: pyside-uic 0.2.15 running on PySide 1.2.1
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(553, 294)
        self.musicFiles = QtGui.QListWidget(Form)
        self.musicFiles.setGeometry(QtCore.QRect(10, 40, 161, 241))
        self.musicFiles.setObjectName("musicFiles")
        self.path = QtGui.QLineEdit(Form)
        self.path.setEnabled(False)
        self.path.setGeometry(QtCore.QRect(190, 10, 351, 20))
        self.path.setObjectName("path")
        self.preview = QtGui.QPushButton(Form)
        self.preview.setEnabled(False)
        self.preview.setGeometry(QtCore.QRect(190, 40, 101, 23))
        self.preview.setObjectName("preview")
        self.graphs = QtGui.QPushButton(Form)
        self.graphs.setEnabled(False)
        self.graphs.setGeometry(QtCore.QRect(320, 40, 101, 23))
        self.graphs.setObjectName("graphs")
        self.console = QtGui.QTextEdit(Form)
        self.console.setEnabled(False)
        self.console.setGeometry(QtCore.QRect(190, 90, 351, 191))
        self.console.setAcceptDrops(True)
        self.console.setObjectName("console")
        self.execute = QtGui.QPushButton(Form)
        self.execute.setEnabled(False)
        self.execute.setGeometry(QtCore.QRect(450, 40, 91, 23))
        self.execute.setObjectName("execute")
        self.refresh = QtGui.QPushButton(Form)
        self.refresh.setGeometry(QtCore.QRect(10, 10, 161, 23))
        self.refresh.setObjectName("refresh")

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        Form.setWindowTitle(QtGui.QApplication.translate("Form", "Form", None, QtGui.QApplication.UnicodeUTF8))
        self.path.setPlaceholderText(QtGui.QApplication.translate("Form", "Path to the music file", None, QtGui.QApplication.UnicodeUTF8))
        self.preview.setText(QtGui.QApplication.translate("Form", "Preview", None, QtGui.QApplication.UnicodeUTF8))
        self.graphs.setText(QtGui.QApplication.translate("Form", "Graphs", None, QtGui.QApplication.UnicodeUTF8))
        self.console.setHtml(QtGui.QApplication.translate("Form", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:\'MS Shell Dlg 2\'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">Console</span></p></body></html>", None, QtGui.QApplication.UnicodeUTF8))
        self.execute.setText(QtGui.QApplication.translate("Form", "Execute", None, QtGui.QApplication.UnicodeUTF8))
        self.refresh.setText(QtGui.QApplication.translate("Form", "Refresh File List", None, QtGui.QApplication.UnicodeUTF8))

