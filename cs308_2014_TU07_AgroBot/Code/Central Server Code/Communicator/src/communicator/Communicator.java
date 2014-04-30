// This class is the server class which contains functions to initialize
// the server with the input and output serial ports along with the streams which
// are used to communicate between troughs and watering bot.

package communicator;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStream;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import java.io.DataInputStream;
import java.util.Enumeration;

public class Communicator implements SerialPortEventListener {

    public SerialPort serialPortIn; // input serial port
    public SerialPort serialPortInOut; // output serial port
    /**
     * The port we're normally going to use.
     */
    private static final String PORT_NAMES[] = {
        "/dev/tty.usbserial-A9007UX1", // Mac OS X
        "/dev/ttyUSB0", // Linux
        "COM3", // Windows
    };
    
    // Output and input streams of the server which will be used to communicate with troughs and bot
    public static DataInputStream input;
    public static OutputStream output;
    /**
     * Milliseconds to block while waiting for port open
     */
    public static final int TIME_OUT = 2000;
    /**
     * Default bits per second for COM port.
     */
    public static final int DATA_RATE = 9600;

    public void initialize() {
        // inPortId is the input port where Server receives message from the trough
        // outPortId is the output port through which Server will notify the bot about the troughs that need watering
        CommPortIdentifier inPortId = null;
        CommPortIdentifier outPortId = null;
        Enumeration portEnum = CommPortIdentifier.getPortIdentifiers();

        //First, Find instances of serial port to set the outport and inport
        // portEnum has all the available ports
        //After iterating "COM5" is set as the inPort and "COM3" as outPort
        // These ports can be changed as per the requirements.
        while (portEnum.hasMoreElements()) {
            CommPortIdentifier currPortId = (CommPortIdentifier) portEnum.nextElement();
            // COM5 is set as inPortId
            if (currPortId.getName().equals("COM5")) {
                inPortId = currPortId;
            }

            //COM3 is set as outPortId
            if (currPortId.getName().equals("COM3")) {
                outPortId = currPortId;
            }
        }
        // Error message if either ports are not available
        if (inPortId == null || outPortId == null) {
            System.out.println("Could not find COM port.");
            return;
        }

        try {
            // open serial ports, and use class name for the appName.
            serialPortIn = (SerialPort) inPortId.open(this.getClass().getName(),
                    TIME_OUT);
            serialPortInOut = (SerialPort) outPortId.open(this.getClass().getName(),
                    TIME_OUT);

            // set port parameters
            serialPortIn.setSerialPortParams(DATA_RATE,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);

            serialPortInOut.setSerialPortParams(DATA_RATE,
                    SerialPort.DATABITS_8,
                    SerialPort.STOPBITS_1,
                    SerialPort.PARITY_NONE);

            // open the streams
            input = new DataInputStream(serialPortIn.getInputStream());
            output = serialPortInOut.getOutputStream();

            // add event listeners
            serialPortIn.addEventListener(this);
            serialPortIn.notifyOnDataAvailable(true);

            serialPortInOut.addEventListener(this);
            serialPortInOut.notifyOnDataAvailable(true);
        } catch (Exception e) {
            System.err.println(e.toString());
        }
    }
    
    // functions called when the server shuts down
    public synchronized void close() {
        if (serialPortIn != null) {
            serialPortIn.removeEventListener();
            serialPortIn.close();
        }
        if (serialPortInOut != null) {
            serialPortInOut.removeEventListener();
            serialPortInOut.close();
        }
    }
    
    // This function is called whenever the server receives any data.
    public synchronized void serialEvent(SerialPortEvent oEvent) {
        if (oEvent.getEventType() == SerialPortEvent.DATA_AVAILABLE) {
            try {
                String inputLine = input.readLine();
            } catch (Exception e) {
                System.err.println(e.toString());
            }
        }

    }
    
    // This is the function called when the Server sends the message to the bot.
    // The string that is to be send is converted into a byte array and then sent 
    // For communication between XBee usually a single byte is sent
    public static synchronized void writeData(String data) {
        try {
            output.write(data.getBytes());
        } catch (Exception e) {
            System.out.println("could not write to port");
        }
    }

    
    public static void main(String[] args) throws Exception {
        Communicator main = new Communicator();
        main.initialize();
        Thread t = new Thread() {
            public void run() {
                //the following line will keep this app alive for 1000 seconds,
                //waiting for events to occur and responding to them (printing incoming messages to console).
                try {
                    Thread.sleep(1500);
                } catch (InterruptedException ie) {
                }
            }
        };
        t.start();
    }
}
