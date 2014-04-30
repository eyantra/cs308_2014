import javax.swing.*;
import java.awt.Color;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;

public class app implements  ActionListener{

    // Definition of global values and items that are part of the GUI.
    int labelsX = 100; //length of water level bar
    int labelsY = 20;  //width of water level bar
    
    //start coordinates of levels
    int startTitleX = 10; 
    int startScoreX = 80;
    int startY = 30; 
    
    //water level bar
    JProgressBar progressBar;

    // panel declaration for trough labels and levels
    JPanel titlePanel, scorePanel;
    
    // declaration of level bars for bots and troughs
    JProgressBar[] bots;
    JProgressBar[] troughs;
    
    int noOfBots; //no of bots
    int noOfTroughs; // no of troughs

    //function resposible for setting up GUI
    //this function supports any number of watering bots and troughs.
    //expects no of bots, no of troughs, labels to bot, labels to troughs
    public JPanel createContentPane (int noOfBotsValue, int noOfTroughsValue, String[] labelsBot, String[] labelsTrough){
        troughs = new JProgressBar[noOfTroughsValue];
        bots = new JProgressBar[noOfBotsValue];

        noOfBots = noOfBotsValue;
        noOfTroughs = noOfTroughsValue;

        // We create a bottom JPanel to place everything on.
        JPanel totalGUI = new JPanel();
        totalGUI.setLayout(null);

        // Creation of a Panel to contain the troughs and bots title labels
        titlePanel = new JPanel();
        titlePanel.setLayout(null);
        titlePanel.setLocation(startTitleX, startY);
        titlePanel.setSize(100, 500);
        totalGUI.add(titlePanel);

        // placement of labels to troughs
       for(int i=0;i<noOfTroughs;i++){
          JLabel redLabel = new JLabel(labelsTrough[i]);
          redLabel.setLocation(startTitleX, startY+ i*(labelsY+10));
          redLabel.setSize(labelsX, labelsY);
          redLabel.setHorizontalAlignment(0);
          redLabel.setForeground(Color.red);
          titlePanel.add(redLabel);
       }
       
       // placement of labels to watering bots
       for(int i=0;i<noOfBots;i++){
          JLabel blueLabel = new JLabel(labelsBot[i]);
          blueLabel.setLocation(startTitleX, startY + (noOfTroughs+i)*(labelsY+10));
          blueLabel.setSize(labelsX, labelsY);
          blueLabel.setHorizontalAlignment(0);
          blueLabel.setForeground(Color.blue);
          titlePanel.add(blueLabel);
       }

        // Creation of a Panel to contain the water levels.
        scorePanel = new JPanel();
        scorePanel.setLayout(null);
        scorePanel.setLocation(startScoreX, startY);
        scorePanel.setSize(260, 500);
        totalGUI.add(scorePanel);

        //below loop initialize water levels of each trough
        for(int i=0;i<noOfTroughs;i++){
          progressBar = new JProgressBar(0, 100);
          progressBar.setLocation(startScoreX, startY+ (i)*(labelsY+10));
          progressBar.setSize(labelsX, labelsY);
          progressBar.setValue(20);
          progressBar.setStringPainted(true);
          scorePanel.add(progressBar);
          troughs[i] = progressBar;
       }

        //below loop initialize water levels of each watering bot
       for(int i=0;i<noOfBots;i++){
          progressBar = new JProgressBar(0, 100);
          progressBar.setLocation(startScoreX, startY+ (noOfTroughs+i)*(labelsY+10));
          progressBar.setSize(labelsX, labelsY);
          progressBar.setValue(20);
          progressBar.setStringPainted(true);
          scorePanel.add(progressBar);
          bots[i] = progressBar;
       }
        
        totalGUI.setOpaque(true);
        return totalGUI;
    }

    //this function takes values of water levels for each trough and reflects them in the GUI
    public void updateTroughs(int[] values){
      for(int i=0;i<noOfTroughs;i++){
        troughs[i].setValue(values[i]*33);
      }

    }

    // This is the new ActionPerformed Method.
    // It catches any events with an ActionListener attached.
    // Using an if statement, we can determine which button was pressed
    // and change the appropriate values in our GUI.
    public void actionPerformed(ActionEvent e) {
        /*
        add code for actionevents
        */
    }

    //create instance of parent class and create frame for setting up GUI
    //expects no of bots, no of troughs, labels to bot, labels to troughs
    private static app createAndShowGUI(int noOfBotsValue, int noOfTroughsValue, String[] labelsBot, String[] labelsTrough) {

        //Create and initialize JFrame
        JFrame.setDefaultLookAndFeelDecorated(true);
        JFrame frame = new JFrame("Greenhouse Status");

        //Create and set up the content pane.
        app demo = new app();
        frame.setContentPane(demo.createContentPane(noOfBotsValue, noOfTroughsValue, labelsBot, labelsTrough));

        //set attribute values of Jframe
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(300, (noOfBotsValue+noOfTroughsValue)*50+50);
        frame.setVisible(true);

        return demo;
    }

}