package in.ac.iitb.aquaponis;

import static in.ac.iitb.aquaponis.CommonUtilities.DISPLAY_MESSAGE_ACTION;
import static in.ac.iitb.aquaponis.CommonUtilities.EXTRA_MESSAGE;
import static in.ac.iitb.aquaponis.CommonUtilities.SENDER_ID;
import static in.ac.iitb.aquaponis.CommonUtilities.SERVER_URL;
import static in.ac.iitb.aquaponis.CommonUtilities.SERVER_URL_feed;
import static in.ac.iitb.aquaponis.CommonUtilities.TAG;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.widget.Toast;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

import java.io.IOException;
import java.io.OutputStream;
import java.lang.reflect.Method;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.UUID;
 

 

import java.util.Map.Entry;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;

import com.google.android.gcm.GCMRegistrar;
 
public class MainActivity extends Activity {
    // label to display gcm messages
    TextView lblMessage;
    
    // Bluetooth 
    
    Button btnOn, btnOff,btnFeed,btnStop;
    
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private OutputStream outStream = null;
     
    // SPP UUID service 
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
   
    // MAC-address of Bluetooth module (you must edit this line)
    private static String address = "00:19:A4:02:C6:B7";
     
    
    // Bluetooth
    
    // Asyntask
    AsyncTask<Void, Void, Void> mRegisterTask;
    //private final String mytag = "MYTAG";
    private static final String TAG = "ABCD";
    // Alert dialog manager
    AlertDialogManager alert = new AlertDialogManager();
     
    // Connection detector
    ConnectionDetector cd;
     
    public static String name;
    public static String email;
    public static String myregId;
 
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
         
        cd = new ConnectionDetector(getApplicationContext());
 
        // Check if Internet present
        if (!cd.isConnectingToInternet()) {
            // Internet Connection is not present
            alert.showAlertDialog(MainActivity.this,
                    "Internet Connection Error",
                    "Please connect to working Internet connection", false);
            // stop executing code by return
            return;
        }
        
        //Check bluetooth connection
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        checkBTState();
         
        // Getting name, email from intent
        Intent i = getIntent();
         
        name = i.getStringExtra("name");
        email = i.getStringExtra("email");      
         
        // Make sure the device has the proper dependencies.
        //GCMRegistrar.checkDevice(this);
 
        // Make sure the manifest was properly set - comment out this line
        // while developing the app, then uncomment it when it's ready.
        //GCMRegistrar.checkManifest(this);
 
        //lblMessage = (TextView) findViewById(R.id.lblMessage);
         
        registerReceiver(mHandleMessageReceiver, new IntentFilter(
                DISPLAY_MESSAGE_ACTION));
         
        // Get GCM registration id
        final String regId = GCMRegistrar.getRegistrationId(this);
       // myregId = regId;
 
        // Check if regid already presents
        if (regId.equals("")) {
            // Registration is not present, register now with GCM           
            GCMRegistrar.register(this, SENDER_ID);
           
        } else {
            // Device is already registered on GCM
            if (GCMRegistrar.isRegisteredOnServer(this)) {
                // Skips registration.              
                Toast.makeText(getApplicationContext(), "Already registered with GCM", Toast.LENGTH_LONG).show();
            } else {
                // Try to register again, but not in the UI thread.
                // It's also necessary to cancel the thread onDestroy(),
                // hence the use of AsyncTask instead of a raw thread.
                final Context context = this;
                mRegisterTask = new AsyncTask<Void, Void, Void>() {
 
                    @Override
                    protected Void doInBackground(Void... params) {
                        // Register on our server
                        // On server creates a new user
                        ServerUtilities.register(context, name, email, regId);
                        return null;
                    }
 
                    @Override
                    protected void onPostExecute(Void result) {
                        mRegisterTask = null;
                    }
 
                };
                mRegisterTask.execute(null, null, null);
            }
        }
        
        final String regId1 = GCMRegistrar.getRegistrationId(this);
        myregId = regId1;
        Log.d(TAG, "regId is " + myregId);
        
    }       
 
    /**
     * Receiving push messages
     * */
    private final BroadcastReceiver mHandleMessageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String newMessage = intent.getExtras().getString(EXTRA_MESSAGE);
           
            // Waking up mobile if it is sleeping
           
            checkBTState();
            String s1 = "low";
            String s2 = "medium";
            String s3 = "high";
            
            // check if dosage to be given is low , letter 'f' used for low dose
            if (s1.equals(newMessage))
            sendData("f"); 
            // check if dosage to be given is medium , letter 'g' used for medium dose
            if (s2.equals(newMessage))
                sendData("g"); 
            // check if dosage to be given is high , letter 'h' used for high dose
            if (s3.equals(newMessage))
                sendData("h"); 
           
            WakeLocker.acquire(getApplicationContext());
            
             
            /**
             * Take appropriate action on this message
             * depending upon your app requirement
             * For now we are displaying dosage amount  on the screen
             * */
            
            
            // Showing received message
            //Log.d(TAG, "LBL"+lblMessage.toString());
           // Log.d(TAG, newMessage);
          
           // lblMessage.append(newMessage + "ab \n");   
           // Log.d(TAG, "LBL"+lblMessage.toString());
            
            Toast.makeText(getApplicationContext(), "New Message: " + newMessage, Toast.LENGTH_LONG).show();
             
            WakeLocker.release();
            
           // Releasing wake lock
           
        }
    };
   
    @Override
    public void onResume() {
      super.onResume();
   
      Log.d(TAG, "...onResume - try connect...");
     
      // Set up a pointer to the remote node using it's address.
      BluetoothDevice device = btAdapter.getRemoteDevice(address);
     
      // Two things are needed to make a connection:
      //   A MAC address, which we got above.
      //   A Service ID or UUID.  In this case we are using the
      //     UUID for SPP.
     
  	try {
  		btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
  	} catch (IOException e1) {
  		errorExit("Fatal Error", "In onResume() and socket create failed: " + e1.getMessage() + ".");
  	}
      
      
     
      // Discovery is resource intensive.  Make sure it isn't going on
      // when you attempt to connect and pass your message.
      btAdapter.cancelDiscovery();
     
      // Establish the connection.  This will block until it connects.
      Log.d(TAG, "...Connecting...");
      try {
        btSocket.connect();
        Log.d(TAG, "...Connection ok...");
      } catch (IOException e) {
        try {
          btSocket.close();
        } catch (IOException e2) {
          errorExit("Fatal Error", "In onResume() and unable to close socket during connection failure" + e2.getMessage() + ".");
        }
      }
       
      // Create a data stream so we can talk to server.
      Log.d(TAG, "...Create Socket...");
   
      try {
        outStream = btSocket.getOutputStream();
      } catch (IOException e) {
        errorExit("Fatal Error", "In onResume() and output stream creation failed:" + e.getMessage() + ".");
      }
    }
   
  /*  @Override
    public void onPause() {
      super.onPause();
   
      Log.d(TAG, "...In onPause()...");
   
      if (outStream != null) {
        try {
          outStream.flush();
        	int i=1;
        } catch (IOException e) {
          errorExit("Fatal Error", "In onPause() and failed to flush output stream: " + e.getMessage() + ".");
        }
      }
   
      try     {
        btSocket.close();
      } catch (IOException e2) {
        errorExit("Fatal Error", "In onPause() and failed to close socket." + e2.getMessage() + ".");
      }
    }*/
     
    @Override
    protected void onDestroy() {
    	Log.d(TAG, "On Destroy Called");
        if (mRegisterTask != null) {
            mRegisterTask.cancel(true);
        }
        try {
            unregisterReceiver(mHandleMessageReceiver);
            GCMRegistrar.onDestroy(this);
        } catch (Exception e) {
            Log.e("UnRegister Receiver Error", "> " + e.getMessage());
        }
        super.onDestroy();
    }
    
    private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException {
        if(Build.VERSION.SDK_INT >= 10){
            try {
                final Method  m = device.getClass().getMethod("createInsecureRfcommSocketToServiceRecord", new Class[] { UUID.class });
                return (BluetoothSocket) m.invoke(device, MY_UUID);
            } catch (Exception e) {
                Log.e(TAG, "Could not create Insecure RFComm Connection",e);
            }
        }
        return  device.createRfcommSocketToServiceRecord(MY_UUID);
    }
    
    private void checkBTState() {
        // Check for Bluetooth support and then check to make sure it is turned on
        // Emulator doesn't support Bluetooth and will return null
        if(btAdapter==null) { 
          errorExit("Fatal Error", "Bluetooth not support");
        } else {
          if (btAdapter.isEnabled()) {
            Log.d(TAG, "...Bluetooth ON...");
          } else {
            //Prompt user to turn on Bluetooth
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBtIntent, 1);
          }
        }
      }
     
      private void errorExit(String title, String message){
        Toast.makeText(getBaseContext(), title + " - " + message, Toast.LENGTH_LONG).show();
        finish();
      }
     
      private void sendData(String message) {
        byte[] msgBuffer = message.getBytes();
     
        Log.d(TAG, "...Send data: " + message + "...");
     
        try {
          outStream.write(msgBuffer);
        } catch (IOException e) {
          String msg = "In onResume() and an exception occurred during write: " + e.getMessage();
          if (address.equals("00:00:00:00:00:00")) 
            msg = msg + ".\n\nUpdate your server address from 00:00:00:00:00:00 to the correct address on line 35 in the java code";
          	msg = msg +  ".\n\nCheck that the SPP UUID: " + MY_UUID.toString() + " exists on server.\n\n";
           
          	errorExit("Fatal Error", msg);       
        }
      }
     
      /*
      * @param endpoint POST address.
      * @param params request parameters.
      *
      * @throws IOException propagated from POST.
      */
     private static void post(String endpoint, Map<String, String> params)
             throws IOException {    
          
         URL url;
         try {
             url = new URL(endpoint);
         } catch (MalformedURLException e) {
             throw new IllegalArgumentException("invalid url: " + endpoint);
         }
         StringBuilder bodyBuilder = new StringBuilder();
         Iterator<Entry<String, String>> iterator = params.entrySet().iterator();
         // constructs the POST body using the parameters
         while (iterator.hasNext()) {
             Entry<String, String> param = iterator.next();
             bodyBuilder.append(param.getKey()).append('=')
                     .append(param.getValue());
             if (iterator.hasNext()) {
                 bodyBuilder.append('&');
             }
         }
         String body = bodyBuilder.toString();
         Log.v(TAG, "Posting '" + body + "' to " + url);
         byte[] bytes = body.getBytes();
         HttpURLConnection conn = null;
         try {
             Log.e("URL", "> " + url);
             conn = (HttpURLConnection) url.openConnection();
             conn.setDoOutput(true);
             conn.setUseCaches(false);
             conn.setFixedLengthStreamingMode(bytes.length);
             conn.setRequestMethod("POST");
             conn.setRequestProperty("Content-Type",
                     "application/x-www-form-urlencoded;charset=UTF-8");
             // post the request
             OutputStream out = conn.getOutputStream();
             out.write(bytes);
             out.close();
             // handle the response
             int status = conn.getResponseCode();
             if (status != 200) {
               throw new IOException("Post failed with error code " + status);
             }
         } finally {
             if (conn != null) {
                 conn.disconnect();
             }
         }
       }
    }

 