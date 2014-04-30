package com.flipflop.autonav.app;

/**
 * Struct to handle the QR code info on the junctions.
 *
 * Created by Pallav on 13/4/14.
 */
public class QRInfo {

    public int row;
    public int column;

    public String forward;
    public String forwardDir;

    public String right;
    public String rightDir;

    //Parse the string from the QR Code
    public QRInfo(String s){
        String array[] = s.split(":");
        if(array.length == 6){
            row = Integer.parseInt(array[0]);
            column = Integer.parseInt(array[1]);
            forward = array[2];
            forwardDir = array[3];
            right = array[4];
            rightDir = array[5];
        } else throw new java.lang.IllegalArgumentException();
    }

}
