/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package greenhousemanager;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;

/**
 *
 * @author pulkit
 */
public class SerialComm {
    private static SerialPort serialPort;
    private static MainPage parentFrame;
    private static Protocol protocol;
    private final static char delimiter = ',';
    private final static char endCharTask = '|';
    private final static char endCharThreshold = '~';
    private final static char endCharSensorSetStart = ':';
    private final static char endCharSensorSetStop = '!';
    
    /**
     */
    
    public SerialComm(String str, MainPage frame) throws Exception
    {
        serialPort = new SerialPort(str);
        parentFrame = frame;
        initPort();
        protocol = new Protocol(this, frame);
    }
    
    public Protocol getProtocol()
    {
        return protocol;
    }
    
    public void initPort() throws Exception
    {
        serialPort.openPort();//Open port
        serialPort.setParams(9600, 8, 1, 0);//Set params
        //int mask = SerialPort.MASK_RXCHAR + SerialPort.MASK_CTS + SerialPort.MASK_DSR;//Prepare mask
        int mask = SerialPort.MASK_RXCHAR;
        serialPort.setEventsMask(mask);//Set mask
        serialPort.addEventListener(new SerialPortReader());//Add SerialPortEventListener
    }
    
    static class SerialPortReader implements SerialPortEventListener {

        public void serialEvent(SerialPortEvent event) {
            if(event.isRXCHAR())
            {
                System.out.println(event.getEventValue());
                int len = event.getEventValue();
                try {
                    byte buffer[] = serialPort.readBytes(len);
                    String s = new String(buffer);
                    parentFrame.appendTerminal(s, "red");
                    System.out.println("set values:"+s);
                    if(s.charAt(s.length()-1) == endCharTask)
                    {
                        protocol.receiveMessage(1);
                    } else if(s.charAt(s.length()-1) == endCharThreshold)
                    {
                        protocol.receiveMessage(0);
                    }
                }
                catch (SerialPortException ex) {
                    System.out.println(ex);
                }   
            }
        }
    }
    
    public void closePort() throws Exception
    {
        serialPort.closePort();    
    }
    
    static String toBinary( byte[] bytes )
    {
        StringBuilder sb = new StringBuilder(bytes.length * Byte.SIZE);
        for( int i = 0; i < Byte.SIZE * bytes.length; i++ )
            sb.append((bytes[i / Byte.SIZE] << i % Byte.SIZE & 0x80) == 0 ? '0' : '1');
        return sb.toString();
    }
    
    public static String binaryToHex(String bin) {
        return String.format("%2X", Long.parseLong(bin,2)) ;
     }
    
    public boolean writeBytes(String str) throws Exception
    {
//        byte[] b = str.getBytes();
//        serialPort.writeBytes(b);
        boolean flag = serialPort.writeString(str);
        parentFrame.appendTerminal(str, "blue");
        return flag;
    }
    
    public static void displayErrorGUI(Exception e, JFrame j) {
        e.printStackTrace();
        Writer writer = new StringWriter();
        PrintWriter printWriter = new PrintWriter(writer);
        e.printStackTrace(printWriter);
        JOptionPane.showMessageDialog(j, "Serial Communication Error:\n" + writer.toString(), "Error while doing Serial IO", JOptionPane.ERROR_MESSAGE);
    }
    
    
    public class Protocol
    {
        //type 1 is task
        //type 0 is threshold
        //type 2 is sensor set start
        //type 3 is sensor set stop
        private final SerialComm serialCommObj;
        private final MainPage parentFrame;
        private final String replaceNull = "-1";
        
        public Protocol(SerialComm sc, MainPage mp){
            serialCommObj = sc;
            parentFrame = mp;
        }

        public boolean sendMessage(String data[], int type)
        {
            String str = "";
            for(String s : data)
            {
                if(s == null || s.length() == 0 || s.equals(""))
                    str= str + replaceNull + delimiter;
                else
                    str= str + s + delimiter;
            }
            str = str.substring(0, str.length() - 1);
            if(type == 1)
                str = str + endCharTask;
            else if(type == 2)
                str = str + endCharSensorSetStart;
            else if(type == 3)
                str = str + endCharSensorSetStop;
            else if(type == 0)
                str = str + endCharThreshold;
            boolean flag = false;
            try {
                flag = serialCommObj.writeBytes(str);
            } catch (Exception ex) {
                SerialComm.displayErrorGUI(ex, parentFrame);
            }
            return flag;
        }

        public void receiveMessage(int type)
        {
            if(type == 0)
            {
                JOptionPane.showMessageDialog(parentFrame, "Threshold Set Successful !", "Success", JOptionPane.INFORMATION_MESSAGE);
            } else if(type == 1)
            {
                JOptionPane.showMessageDialog(parentFrame, "Task Creation Successful !", "Success", JOptionPane.INFORMATION_MESSAGE);
            }
        }

    }

}
    

