Tyler: Motion tracking of customers and virtual assistance
==============

About
-----

**ERTS_project 2014-CS308**

Group Number: TH03

Team Name: Athena

Team Members:

* Anil Shanbhag (100050082) [Team Leader]
* Pranav Jindal (100050006)
* Sameep Bagadia (100050003)
* Rahee Borade (100050028)

Installation Instructions
-------------------------

Setup Screencast can be found here `https://www.youtube.com/watch?v=eHxsoIeHbXA`

The Tyler system can be broadly split into 3 parts based on where it will be run. 

1. Bot: Bot acts as a sales person. It further contains two parts, the Android phone and the FB5 bot
2. Host: The host is present in the store. It does camera feed processing and communication with the remote center.
3. Remote: Remote provides an interface for people to interact with bots and hence with customers.

**BOT**

The code for the bot can be found in `FB5_ARM/`. The project can be opened in Kiel uVision. Generate the .bin file and put it on the FB5 bot.

There is no seperate code for the Android Phone. Just download Chrome browser on Android and open `http://<server-ip>:<port>/client/b0`.

**HOST**

* Install OpenCV for python using the following tutorial `http://www.raben.com/book/export/html/3`
* Setup virtualenv, follow this tutorial `http://docs.python-guide.org/en/latest/dev/virtualenvs/`
* Setup node.js and npm

```
sudo add-apt-repository ppa:chris-lea/node.js
sudo apt-get update
sudo apt-get install python-software-properties python g++ make nodejs
```

* Install necessary python and npm packages 

```
cd PC/
pip install -r requirements.txt
npm install
```

* Start tracker

```
python PC/host/tracker/tracker.py
```

* In another terminal, after setting up the remote, start the client

```
node PC/host/client/host.js
```

**REMOTE**

* Follow the instructions to install nodejs and npm above
* Start the server

```
npm install
node PC/remote/httpserver.js
```

