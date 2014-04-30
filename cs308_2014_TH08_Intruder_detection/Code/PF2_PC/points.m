% For taking base image of the arena (w/o bot and intruder)
close all;clc;clear;
vid = videoinput('winvideo',1, 'RGB24_640x480');
vid.ReturnedColorspace = 'rgb';
start(vid);
pause(1);
baseimage = getsnapshot(vid);
stop(vid);
delete(vid);
imtool(baseimage);
imwrite(baseimage,'baseimage.jpg');