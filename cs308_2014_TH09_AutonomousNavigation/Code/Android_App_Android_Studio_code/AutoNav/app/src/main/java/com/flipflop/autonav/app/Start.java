package com.flipflop.autonav.app;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Set;
import java.util.UUID;


public class Start extends ActionBarActivity {

    Button button;
    public Activity mainActivity = this;

    //To be used for bluetooth connection
    static InputStream is = null;
    static OutputStream os = null;
    static BluetoothSocket bs = null;

    //UID of the final destination
    static UID dest = null;

    //Function is called when the app activity is launched
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_start);

        //Adding listener on the button
        addListenerOnButton();

        //Initialising Bluetooth
        try {
            initBluetooth();
        } catch (IOException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        }

        //Connecting to FireBird5 via Bluetooth
        try {
            connectBluetooth();
        } catch (Exception e) {
            e.printStackTrace();
        }

        //Debugging
        /*try {
            sendBluetoothMessage("8");
        } catch (IOException e) {
            e.printStackTrace();
        }*/

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.start, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    //Adding listener to the button
    public void addListenerOnButton(){
        button = (Button) findViewById(R.id.button);

        button.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View arg0) {

                //Get destination
                EditText et = (EditText) findViewById(R.id.editText);
                String uid = et.getText().toString();

                //System.out.println(uid);
                /*if(uid.length() != 16){
                    et.
                }*/

                //Disable the edit box after getting input
                dest = new UID(uid);
                et.setEnabled(false);

                //Starting Bot traversal by sending 'S'
                try {
                    sendBluetoothMessage("S");
                } catch (IOException e) {
                    e.printStackTrace();
                }

                //Staring camera scan for QR code
                Intent intent = new Intent("com.google.zxing.client.android.SCAN");
                intent.putExtra("SCAN_WIDTH", 1000);
                intent.putExtra("SCAN_HEIGHT", 700);
                intent.putExtra("SCAN_MODE", "QR_CODE_MODE");
                startActivityForResult(intent, 0);

            }

        });
    }

    //Callback function for a Scan Success
    public void onActivityResult(int requestCode, int resultCode, Intent intent) {

        if (requestCode == 0) {
            if (resultCode == RESULT_OK) {

                //Contents of the QR code
                String contents = intent.getStringExtra("SCAN_RESULT");
                //String format = intent.getStringExtra("SCAN_RESULT_FORMAT");

                //Text view to display what is scanned
                TextView t = (TextView) findViewById(R.id.textView2);
                t.setText(contents);

                //Parse the string from QR code into a info struct
                QRInfo qr = new QRInfo(contents);

                //Code for deciding on the QR info, where the bot should go next
                int rowDiff = (dest.row1 - qr.row);
                int colDiff = (dest.column1 - qr.column);

                if(rowDiff == 0 && colDiff == 0) {
                    try {
                        int distanceBit = dest.distance / 10;
                        sendBluetoothMessage("D"/*" + distanceBit*/);
                    } catch (IOException e) {
                    }
                }

                if(rowDiff != 0){
                    if(qr.forward.equals("R")){
                        if(rowDiff * (qr.forwardDir.equals("+")? 1: -1) > 0){
                            try {
                                sendBluetoothMessage("F");
                            } catch (IOException e) {}
                        } else {
                            try {
                                sendBluetoothMessage("U");
                            } catch (IOException e) {}
                        }
                    } else if(qr.right.equals("R")){
                        if(rowDiff * (qr.rightDir.equals("+")? 1: -1) > 0){
                            try {
                                sendBluetoothMessage("R");
                            } catch (IOException e) {}
                        } else {
                            try {
                                sendBluetoothMessage("L");
                            } catch (IOException e) {}
                        }
                    }
                } else {
                    if(colDiff != 0) {
                        if (qr.forward.equals("C")) {
                            if (colDiff * (qr.forwardDir.equals("+") ? 1 : -1) > 0) {
                                try {
                                    sendBluetoothMessage("F");
                                } catch (IOException e) {
                                }
                            } else {
                                try {
                                    sendBluetoothMessage("U");
                                } catch (IOException e) {
                                }
                            }
                        } else if (qr.right.equals("C")) {
                            if (colDiff * (qr.rightDir.equals("+") ? 1 : -1) > 0) {
                                try {
                                    sendBluetoothMessage("R");
                                } catch (IOException e) {
                                }
                            } else {
                                try {
                                    sendBluetoothMessage("L");
                                } catch (IOException e) {
                                }
                            }
                        }
                    } else {
                        String change = (dest.row1 - dest.row2 != 0)?"R":"C";
                        rowDiff = dest.row2 - qr.row;
                        colDiff = dest.column2 - qr.column;

                        if(change.equals("R")){
                            if(qr.forward.equals("R")){
                                if(rowDiff * (qr.forwardDir.equals("+")? 1: -1) > 0){
                                    try {
                                        sendBluetoothMessage("F");
                                    } catch (IOException e) {}
                                } else {
                                    try {
                                        sendBluetoothMessage("U");
                                    } catch (IOException e) {}
                                }
                            } else if(qr.right.equals("R")){
                                if(rowDiff * (qr.rightDir.equals("+")? 1: -1) > 0){
                                    try {
                                        sendBluetoothMessage("R");
                                    } catch (IOException e) {}
                                } else {
                                    try {
                                        sendBluetoothMessage("L");
                                    } catch (IOException e) {}
                                }
                            }
                        } else {
                            if (qr.forward.equals("C")) {
                                if (colDiff * (qr.forwardDir.equals("+") ? 1 : -1) > 0) {
                                    try {
                                        sendBluetoothMessage("F");
                                    } catch (IOException e) {
                                    }
                                } else {
                                    try {
                                        sendBluetoothMessage("U");
                                    } catch (IOException e) {
                                    }
                                }
                            } else if (qr.right.equals("C")) {
                                if (colDiff * (qr.rightDir.equals("+") ? 1 : -1) > 0) {
                                    try {
                                        sendBluetoothMessage("R");
                                    } catch (IOException e) {
                                    }
                                } else {
                                    try {
                                        sendBluetoothMessage("L");
                                    } catch (IOException e) {
                                    }
                                }
                            }
                        }
                    }
                }

                //Restarting scanning
                Intent intent2 = new Intent("com.google.zxing.client.android.SCAN");
                intent2.putExtra("SCAN_WIDTH", 1000);
                intent2.putExtra("SCAN_HEIGHT", 700);
                intent2.putExtra("SCAN_MODE", "QR_CODE_MODE");
                startActivityForResult(intent2, 0);

            } else if (resultCode == RESULT_CANCELED) {

                //QR code scan failure
                TextView t = (TextView) findViewById(R.id.textView2);

                t.setText("NO QR CODE FOUND");

            }
        }

    }

    //Initialise the Bluetooth, setting up variables
    public void initBluetooth() throws IOException,
            NoSuchMethodException, InvocationTargetException, IllegalAccessException {

        //Get the bluetooth adapter of the phone if it exists
        BluetoothAdapter mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter == null) {
            // Device does not support Bluetooth
            System.exit(1);
        }

        //If the adapter is not disabled, enable it
        if (!mBluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, 1/*REQUEST_ENABLE_BT*/);
        }

        //Get paired devices
        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();

        // If there are paired devices
        if (pairedDevices.size() > 0) {
            // Loop through paired devices
            for (BluetoothDevice device : pairedDevices) {
                // Find the bot's blutooth id and create a socket for it
                System.out.println(device.getAddress());
                if(device.getAddress().equals("00:19:A4:02:C6:7E")){
                    System.out.println(device.getAddress());
                    UUID uuid1 = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

                    Method m = device.getClass().getMethod("createRfcommSocket",
                                                            new Class[] { int.class });
                    bs = (BluetoothSocket) m.invoke(device, 1);

                    mBluetoothAdapter.cancelDiscovery();
                }
            }
        }
    }

    //Connect the socket
    public void connectBluetooth() throws Exception {
        System.out.println("CONNECTING...");
        try {
            // Connect the device through the socket. This will block
            // until it succeeds or throws an exception
            bs.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and get out
            try {
                bs.close();
            } catch (IOException closeException) { }
            return;
        }
        System.out.println("CONNECTED!");

        is = bs.getInputStream();
        os = bs.getOutputStream();
    }

    //Send one character message to the bot via bluetooth
    public void sendBluetoothMessage(String s) throws IOException {

        byte b[] = s.getBytes();
        System.out.println("Writing to bot: " +  b[0]);
        os.write(b[0]);

        TextView t = (TextView) findViewById(R.id.textView2);
        t.append(" "+s);
    }
}
