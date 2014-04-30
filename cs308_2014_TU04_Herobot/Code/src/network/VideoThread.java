package network;

import java.awt.image.BufferedImage;
import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.SocketException;
import java.net.URL;

import javax.imageio.ImageIO;

import gui.VideoPanel;

/*
 * Thread that receives packets sent by video streaming software
 * running on an Android phone. The Android phone runs the IP Webcam
 * software available on the Google Play Store
 * (https://play.google.com/store/apps/details?id=com.pas.webcam)
 */
public class VideoThread extends Thread {
	
	// The panel on which the received image is to be drawn
	VideoPanel videoPanel;
	
	// IP address of the Android phone
	String ip;
	
	// URL object that points to the location where frames captured
	// by the phone can be got by sending HTTP requests
	URL videoURL;
	
	// Holds the image retrieved from the response of the HTTP request
	// to 'videoURL' above
	BufferedImage frame = null;
	
	// Some variables useful for pausing and resuming the video streaming
	private final Object pauseLock = new Object();
	boolean stopWorking = false;
	boolean pauseWorking = false;

	public VideoThread(VideoPanel videoPanel) throws SocketException {
		this.videoPanel = videoPanel;		
	}
	
	// This method does all the work
	@Override
	public void run()
	{
		System.out.println("VideoThread: I'm up");
		try {
			
			// create the URL object, the frames are available at /shot.jpg
			videoURL = new URL("http://" + ip + ":8080/shot.jpg");
			
			while (!stopWorking) {
				frame = ImageIO.read(videoURL);
				if (frame != null)
					videoPanel.updateFrame(frame);
				else
					System.out.println("Null frame received");
				synchronized (pauseLock) {
					while (pauseWorking) {
						try {
							pauseLock.wait();
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
				}
			}
		} catch (IOException e) {
			System.out.println("Error: " + e.getMessage());
			e.printStackTrace();
		}
		System.out.println("VideoThread: I'm done");
	}
	
	public void setVideoIP(String ip) {
		this.ip = ip;
	}
	
	public void pauseStreaming() {
		this.pauseWorking = true;
	}
	
	public void resumeStreaming() {
		this.pauseWorking = false;
		synchronized (pauseLock) {
			pauseLock.notify();
		}
	}
	
	public void stopStreaming() {
		this.stopWorking = true;
	}
}
