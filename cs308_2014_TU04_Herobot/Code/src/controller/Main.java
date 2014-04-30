package controller;

import gui.LeapGUI;
import network.Commander;

/*
 * Entry point of the application
 * Creates GUI instance and initialises communication interfaces
 */
public class Main {	
	public static void main(String[] args) {						
		
		// Create classes that serve as an interface for sending commands to
		// the bot
		Commander commander = new Commander();
		Commander secCommander = new Commander();
		
		LeapGUI gui = new LeapGUI();
		
		// This was intended to be used with TCP
		// Now this is useless
		gui.setCommander(commander);
		
		// Connects with serial port
		// Change the address in the connectSerial method in the LeapGUI class
		// to the address of the port where the Xbee module is connected
		gui.connectSerial(secCommander);
		
		gui.setVisible(true);			
	}
}
