package network;

import java.io.IOException;
import java.net.UnknownHostException;
import jssc.*;

/*
 * Uses the TCPClient class to send instructions to the Firebird bot
 */
public class Commander {
	
	// When communicating with bot over TCP, this is the client
	// that sends messages to the TCP server on the phone
	// mounted on the bot
	TCPClient client;
	
	// Alternatively, if TCP is not being used, this is the serial port
	// to use to communicate messages over to the bot via Xbee
	SerialPort serialPort;
	
	public Commander() {}
	
	// This method is called when a TCP connection needs to be 
	// established
	public int connect(String serverIP, Integer serverPort)
	{
		int result = 0;
		try {
			client = new TCPClient(serverIP, serverPort);			
		} catch (UnknownHostException e) {
			System.out.println("Uknown host " + serverIP);
			result = -1;
		} catch (IOException e) {
			System.out.println("IO Error: " + e.getMessage());
			e.printStackTrace();
			result = -1;
		}
		return result;
	}
	
	// This method is called when a serial connection is to be 
	// established to the port where the Xbee module is connected
	public int connectXbee(String port) {
		serialPort = new SerialPort(port);
		try {
			serialPort.openPort();
			serialPort.setParams(SerialPort.BAUDRATE_9600, SerialPort.DATABITS_8, 
					SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
		} catch (SerialPortException e) {			
			e.printStackTrace();
			return -1;
		}
		return 0;
	}
	
	// disconnect TCP connection
	public int disconnect()
	{
		if (client != null)
		{
    		client.close();
    		client = null;
    	}
		
		return 0;
	}
	
	// disconnect serial connection
	public int disconnectXbee() {
		try {
			serialPort.closePort();
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
		return 0;
	}
	
	// send an integer over TCP to a TCP server (phone mounted on bot)
	public void send(Integer n)
	{
		if (client != null)
		{
			client.send(n);
		}
	}
	
	// send an integer (only the last 8 bits) to the bot using the serial
	// connection
	public boolean sendXbee(Integer n) {
		boolean res = true;
		try {
			res = serialPort.writeInt(n & 0xFF);
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
		return res;
	}

}
