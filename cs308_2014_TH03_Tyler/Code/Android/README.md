Android Client
==============

There is no seperate android client. Use Android phone with version >= 4.0. Download the latest Chrome browser from the Play store.

Open `http://<server-ip>:<port>/client/b0` in the chrome browser. Ensure the server is running, the server code can be found in `PC/remote/`. 

Opening the URL initiates a websocket connection to the server and listens for video request. On video request, it starts audio/video communication using WebRTC.
