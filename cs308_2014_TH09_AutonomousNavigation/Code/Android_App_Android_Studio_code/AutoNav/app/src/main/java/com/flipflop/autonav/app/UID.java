package com.flipflop.autonav.app;

/**
 * Struct to handle the UID of a location.
 *
 * Created by Pallav on 13/4/14.
 */
public class UID {

    public int row1;
    public int column1;
    public int row2;
    public int column2;

    public String side;
    public int distance;

    //Parse a UID string (as from the input)
    public UID(String s){
        String array[] = s.split(":");
        if(array.length == 6){
            row1 = Integer.parseInt(array[0]);
            column1 = Integer.parseInt(array[1]);
            row2 = Integer.parseInt(array[2]);
            column2 = Integer.parseInt(array[3]);

            side = array[4];
            distance = Integer.parseInt(array[5]);
        } else {
            System.out.println("Invalid length: "+array.length);
            throw new java.lang.IllegalArgumentException();
        }
    }

}
