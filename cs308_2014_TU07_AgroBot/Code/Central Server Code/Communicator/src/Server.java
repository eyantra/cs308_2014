// This is the test/main class of the server side. To run the server, run this class.
// It creates an instance of the Server (or Communicator) and uses its ports and streams
// to communicate with the troughs and watering bot.
// It contains the functions to read data from troughs and send it to the bot.
// The code is currently for 2 trough system with each trough having a 3-level water-level sensor
// and can be easily extended to more troughs with more water levels.

import communicator.Communicator;
import gnu.io.CommPortIdentifier;
import gnu.io.SerialPort;
import gnu.io.SerialPortEvent;
import gnu.io.SerialPortEventListener;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.Scanner;
import java.util.concurrent.TimeUnit;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFrame;
import javax.swing.SwingUtilities;

public class Server {

    // Input and Output streams of the server
    public static DataInputStream input;
    public static OutputStream output;

    // troughstatus array which contains the status of the 2 troughs.
    // If a trough is currently being watered then its status is busy and its request should not be considered
    // for the entire watering duration.
    public static int[] trstatus = new int[2];

    // function to create and display the GUI pane
    private static app createAndShowGUI(int noOfBotsValue, int noOfTroughsValue, String[] labelsBot, String[] labelsTrough) {

        JFrame.setDefaultLookAndFeelDecorated(true);
        JFrame frame = new JFrame("Greenhouse Status");

        //Create and set up the content pane.
        app demo = new app();
        frame.setContentPane(demo.createContentPane(noOfBotsValue, noOfTroughsValue, labelsBot, labelsTrough));

        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(300, (noOfBotsValue + noOfTroughsValue) * 50 + 50);
        frame.setVisible(true);

        return demo;
    }

    // This is the function called when the Server sends the message to the bot.
    // The string that is to be send is converted into a byte array and then sent 
    // For communication between XBee usually a single byte is sent.
    // Error is thrown if the port is not available 
    public static synchronized void writeData(String data) {
        try {
            output.write(data.getBytes());
        } catch (Exception e) {
            System.out.println("could not write to port");
        }
    }
    
    // This function creates a single byte which contains info about a single trough 
    // and the water level in that trough. This byte is sent to the bot by the server.
    // In the given byte, initial 4 bits contains the trough id whereas the last 4 bits 
    // contains the water level in that trough
    // Bot reads this byte and extracts the trough id and water level in that trough.
    public static String getByte(int trough, int level) {
        int[] bitarray = new int[8];
        for (int i = 0; i < 8; i++) {
            bitarray[i] = 0;
        }
        
        // initial 4 bits are set to the trough id
        int k = 3;
        while (trough > 0) {
            bitarray[k--] = trough % 2;
            trough /= 2;
        }
    
        // last 4 bits are set to the water level
        k = 7;
        while (level > 0) {
            bitarray[k--] = level % 2;
            level /= 2;
        }
        
        // resultant byte is then returned back after adding 33 to it (to make it printable - easier for debugging).
        int chareq = 0;
        int prod = 1;
        for (int i = 7; i >= 0; i--) {
            chareq += bitarray[i] * prod;
            prod *= 2;
        }
        String ret = Integer.toString(chareq + 33);
        return ret;
    }

    // This function is used to send data to bot byte by byte
    // The server sends the byte to bot and waits for the ack that bot has served that request.
    // After the ack as the bot gets idle, server sends the next byte to the bot.
    // If any of the troughs in completely filled, then server doesnt send that byte
    // This function currently handles only 2 troughs. Multiple troughs can be handled by processing
    // them serially in round-robin fashion. 
    // The protocol can be changed as per the requirements and number of troughs and 
    // also the number discrete of water levels 
    public static void SendToBot(int tr1, int tr2) {
        System.out.println("Trough 1 Water level: " + tr1 * 33 + " Trough 2 water level: " + tr2 * 33);
        try {
            int checkbyte = 0;    // used to check if the server has received any data 
            String recStr = "";   // contains the received string
            byte[] recByte = new byte[1]; // byte array in which read data is stored.
            
            // process the 1st trough only if its level is not full
            // The current trough is set to be busy, so that its request is not 
            // considered while the watering is being done.
            // Server sends the byte to the bot using the writeData(byte) function
            // and waits for the ACK ("K") from the bot. 
            // After the ack is received server processes the request of the next trough.
            // Note that another way to achieve this functionality is to bombard the bot with the current 
            // byte and wait for the bot to respond (send ACK). 
            if (tr1 < 3) {
                trstatus[0] = 1;    // The current trough is being processed so 
                String sendingByte = getByte(1, tr1); // pack the trough id and water level in a single byte
                writeData(sendingByte); // send the byte
                
                // wait for the ACK to be received
                // to bombard the bot with the current trough's request, put the writeData command in the while loop
                while (checkbyte < 1 || !recStr.equals("K")) {
                    checkbyte = input.read(recByte, 0, 1);
                    recStr = new String(recByte);
                }
                trstatus[0] = 0; // set the current trough to be idle (request has been processed)
            }
            
            // process the request of the 2nd trough
            recStr = "";
            checkbyte = 0;
        
            if (tr2 < 3) {
                trstatus[1] = 1;
                String sendingByte = getByte(2, tr2);
                writeData(sendingByte);
                while (checkbyte < 1 || !recStr.equals("K")) {
                    checkbyte = input.read(recByte, 0, 1);
                    recStr = new String(recByte);
                }
                trstatus[1] = 0; // 2nd trough set idle
            }
        } catch (IOException ex) {
            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public static void main(String[] ag) {
        SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                // create and display the GUI Pane
                app test = createAndShowGUI(0, 2, new String[]{"Bot1"}, new String[]{"trough1", "trough2"});
                test.updateTroughs(new int[]{0, 0}); // initial waterlevel is assumed to be empty for both troughs
                try {
                    Communicator obj = new Communicator(); // get the instance of the communicator
                    obj.initialize(); // initialise all the streams and ports
                    
                    // set the Testclass streams to be Communicator's streams
                    input = Communicator.input; 
                    output = Communicator.output;
                    
                    // helper variables in the functions
                    int start = 0;
                    int checkbyte = 0;

                    byte[] recByte = new byte[1];
                    
                    // infinite loop in which the server listens to the requests from the trough
                    while (start < 1) {
                        checkbyte = 0;
                        try {
                            String recStr = "";
                            
                            //reading the message from troughs
                            // Until a non empty request is received, keep on listening
                            while (checkbyte < 1 || recStr.equals("")) {
                                checkbyte = input.read(recByte, 0, 1);
                                recStr = new String(recByte);
                            }
                            int deceq = (int) recStr.charAt(0); // decimal equivalent of the byte received
                            deceq -= 33; // subtract 33 which was added to make it printable
                            
                            // get the bits of the byte in a array
                            // the byte is 00fedcba - where def is the status in trough2 and abc in trough1
                            // if waterlevel is 0 - a=b=c=0
                            // else if waterlevel is 1 - a=b=0 c=1
                            // else if waterlevel is 2 - a=0 b=c=1
                            // else if waterlevel is 3(full) - a=b=c=1
                            
                            int[] bitarray = new int[8];
                            for (int i = 7; i >= 0; i--) {
                                bitarray[i] = (deceq & 1);
                                deceq = deceq >> 1;
                            }
                            
                            int tr1 = bitarray[5] + bitarray[6] + bitarray[7];
                            int tr2 = bitarray[2] + bitarray[3] + bitarray[4];
                            
                            //update the app with the latest waterlevel in the troughs
                            test.updateTroughs(new int[]{tr1, tr2});

                            // if the trough is being served at the moment, set its waterlevel to be 3 
                            // so that server doesnt serve the current request
                            if (trstatus[0] != 0) {
                                tr1 = 3;
                            }
                            if (trstatus[1] != 0) {
                                tr2 = 3;
                            }
                            
                            // the data is then sent to the bot using SendToBot function
                            SendToBot(tr1, tr2);

                        } catch (IOException ex) {
                            Logger.getLogger(Server.class.getName()).log(Level.SEVERE, null, ex);
                        }
                    }
                    obj.close();

                } catch (Exception e) {
                }
            }
        });

    }
}
