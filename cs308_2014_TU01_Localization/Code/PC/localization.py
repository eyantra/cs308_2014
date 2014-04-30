import numpy as np
from sklearn.lda import LDA
from sklearn.qda import QDA
from sklearn.naive_bayes import GaussianNB
from math import sqrt
from math import pow
import serial 
import time

# =================================================================DATA=================================================================
# Training data
train_rssi = [[-56,-60,-38],[-55,-58,-37],[-58,-57,-37],[-53,-60,-38],[-58,-59,-38],[-56,-58,-39],[-60,-58,-49],[-59,-60,-50],[-55,-61,-50],[-59,-62,-42],[-58,-59,-43],[-57,-57,-42],[-55,-67,-41],[-56,-62,-40],[-56,-62,-40],[-53,-53,-39],[-52,-54,-39],[-51,-53,-39],[-49,-59,-38],[-51,-57,-38],[-51,-57,-38],[-51,-55,-39],[-52,-55,-39],[-51,-55,-39],[-57,-61,-38],[-55,-62,-39],[-54,-64,-38],[-54,-59,-42],[-51,-60,-42],[-55,-61,-42],[-57,-59,-43],[-58,-59,-43],[-59,-57,-43],[-51,-63,-43],[-53,-59,-43],[-53,-61,-43],[-60,-58,-43],[-61,-60,-43],[-57,-59,-44],[-50,-53,-42],[-50,-56,-42],[-49,-55,-42],[-51,-65,-40],[-47,-63,-40],[-51,-62,-40],[-43,-62,-40],[-46,-63,-40],[-42,-67,-40],[-58,-63,-40],[-58,-58,-40],[-55,-63,-41],[-54,-55,-45],[-52,-57,-45],[-53,-57,-45],[-56,-57,-43],[-58,-60,-43],[-58,-59,-43],[-58,-61,-42],[-57,-62,-42],[-55,-59,-42],[-55,-53,-45],[-52,-53,-46],[-52,-54,-45],[-56,-55,-40],[-53,-55,-41],[-55,-54,-41],[-50,-59,-40],[-52,-57,-40],[-52,-58,-40],[-53,-62,-42],[-54,-63,-42],[-50,-66,-42],[-55,-56,-40],[-52,-57,-40],[-56,-58,-41],[-63,-49,-56],[-58,-53,-56],[-64,-51,-55],[-51,-44,-43],[-53,-43,-43],[-54,-43,-43],[-55,-52,-41],[-56,-53,-41],[-56,-52,-42],[-57,-63,-42],[-59,-61,-42],[-55,-63,-42],[-54,-61,-43],[-55,-66,-43],[-54,-66,-42],[-49,-56,-42],[-46,-57,-42],[-50,-61,-42],[-47,-56,-43],[-51,-55,-43],[-50,-56,-43],[-64,-59,-43],[-59,-60,-43],[-57,-59,-43],[-57,-53,-47],[-58,-54,-47],[-59,-53,-47],[-57,-59,-43],[-56,-58,-44],[-55,-55,-44],[-55,-58,-43],[-52,-58,-44],[-55,-60,-43],[-49,-57,-44],[-49,-56,-43],[-46,-57,-43],[-55,-57,-43],[-53,-60,-44],[-57,-58,-43],[-59,-62,-42],[-60,-60,-44],[-58,-63,-42],[-54,-52,-44],[-53,-52,-44],[-54,-52,-43],[-58,-59,-42],[-55,-61,-43],[-54,-60,-43],[-61,-53,-45],[-62,-51,-45],[-61,-52,-46],[-59,-54,-45],[-55,-59,-45],[-55,-56,-44],[-53,-52,-46],[-55,-53,-46],[-54,-53,-48],[-49,-52,-45],[-49,-52,-46],[-49,-52,-47],[-48,-57,-45],[-48,-57,-43],[-48,-54,-43],[-47,-51,-43],[-48,-54,-43],[-44,-52,-43],[-49,-54,-42],[-52,-54,-42],[-49,-56,-42],[-63,-55,-43],[-65,-53,-43],[-61,-52,-42],[-51,-66,-42],[-54,-65,-41],[-50,-59,-43],[-64,-58,-43],[-66,-60,-45],[-62,-59,-45],[-55,-52,-46],[-54,-52,-45],[-57,-52,-45],[-53,-58,-53],[-54,-58,-51],[-52,-60,-51],[-53,-60,-46],[-55,-58,-46],[-55,-60,-43],[-50,-55,-42],[-48,-55,-42],[-50,-48,-43],[-47,-52,-42],[-51,-54,-43],[-50,-53,-41],[-58,-52,-52],[-55,-54,-55],[-54,-50,-51],[-61,-54,-43],[-61,-52,-43],[-58,-55,-43],[-59,-58,-45],[-60,-54,-44],[-59,-57,-43],[-58,-55,-45],[-55,-55,-47],[-53,-54,-44],[-53,-51,-42],[-58,-50,-42],[-59,-49,-42],[-55,-57,-42],[-53,-57,-42],[-59,-59,-42],[-56,-52,-43],[-56,-53,-43],[-55,-55,-44],[-51,-56,-42],[-53,-58,-43],[-52,-54,-42]]
X = np.array(train_rssi)
y = np.array([0,0,0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7,8,8,8,9,9,9,10,10,10,11,11,11,12,12,12,13,13,13,14,14,14,15,15,15,16,16,16,17,17,17,18,18,18,19,19,19,20,20,20,21,21,21,22,22,22,23,23,23,24,24,24,25,25,25,26,26,26,27,27,27,28,28,28,29,29,29,30,30,30,31,31,31,32,32,32,33,33,33,34,34,34,35,35,35,36,36,36,37,37,37,38,38,38,39,39,39,40,40,40,41,41,41,42,42,42,43,43,43,44,44,44,45,45,45,46,46,46,47,47,47,48,48,48,49,49,49,50,50,50,51,51,51,52,52,52,53,53,53,54,54,54,55,55,55,56,56,56,57,57,57,58,58,58,59,59,59,60,60,60,61,61,61,62,62,62,63,63,63])

# Motion related GLOBAL VARIABLES
class motion_state_enum():
    forward = 1
    left = 2
    back = 3
    right = 4
    stop = 5
    
MOTION_ACTIONS = ["w","a","s","d","q"]
MOTION_STATE = {"w":motion_state_enum.forward,"a":motion_state_enum.left,"s":motion_state_enum.back,"d":motion_state_enum.right,"q":motion_state_enum.stop}

# Other GLOBAL variables
observation_count = 6           # This is the number of times the routers are pinged for RSSI values during testing.
sampling_count = 3              # This is the number of readings taken at a place during sampling.
# ===============================================================END DATA===============================================================

# ========================================================= UTILITIES FUNCTIONS ========================================================
# Function to convert label (0-63) to corresponding (x,y) pair.
def labelToAxes(label):
    return [label/8,label%8]

# Function to calculate distance.
def distance_function(cord1,cord2):
    return sqrt(pow((cord1[0]-cord2[0]),2)+pow((cord1[1]-cord2[1]),2)+pow((cord1[2]-cord2[2]),2))

# Function to get the 3 RSSI values via standard I/O.
def get_std_input():
    rssi1=float(raw_input("Enter rssi1 - "))
    rssi2=float(raw_input("Enter rssi2 - "))
    rssi3=float(raw_input("Enter rssi3 - "))
    return [rssi1,rssi2,rssi3]

# Function to print initial message.
def init_print():
    print ""
    print "Welcome to the localization program!!"
    print "=================================================================================="
    print "Controls for the localization program"
    print "w - Move forward"
    print "a - Move left"
    print "s - Move back"
    print "d - Move right"
    print "q - Stop"
    print "l - Get current coordinates. The bot needs to be stationary (stopped) for this control to work"
    print "z - Quit the system"
    print "=================================================================================="

# ======================================================= END UTILITIES FUNCTIONS ======================================================

# Get the predicted label and coordinates.
def predict_label(rssi1,rssi2,rssi3):
    # Get the predicted label according to the Gaussian NB model and print in various formats.
    pred_label = nbmodel.predict([[rssi1,rssi2,rssi3]])
    print "Predicted candidate by Gaussian NB - Label:", pred_label[0], ", Co-ordinates:", labelToAxes(pred_label[0])

    # Get the probable label according to the shortest distance.
    min_dist = 1000
    curr_dist = 0
    probable_candidates = []
    for i in range(0,192,3):
        trssi = [0,0,0]
        # Calculating a mean reading for each label. This mean reading will be used to find the distance to the current reading.
        for j in range(0,sampling_count,1):
                trssi[j] = train_rssi[i][j]+train_rssi[i+1][j]+train_rssi[i+2][j]
                trssi[j] = trssi[j]/3.0
        curr_dist = distance_function([rssi1,rssi2,rssi3],trssi)
        if curr_dist <= min_dist:
            probable_candidates.append([i/3,curr_dist])
            min_dist = curr_dist
    print "Probable candidate by distance function - Label:", probable_candidates[-1][0], ", Co-ordinates:", labelToAxes(probable_candidates[-1][0])

# Function to get the least variance range from the given array
def get_best_range(arr,target_len):
    min_variance = 1000
    curr_variance = 0
    pos = 0
    for i in range(0,observation_count-target_len+1,1):
        curr_variance = np.var(arr[i:i+target_len])
        if curr_variance < min_variance:
            min_variance = curr_variance
            pos = i
    return [min_variance,pos]

# Function to get the best possible average given the RSSI array
def get_best_avg(rssi_arr):
    rssi_arr.sort()
    # Calculate the best possible range of length 3
    min_variance_3, pos_3 = get_best_range(rssi_arr,3)
    return np.mean(rssi_arr[pos_3:pos_3+3])

# Function to connect to the given router and return the RSSI value. e - ERTS_1, f - ERTS_2, g - ERTS_3
def connect_get_RSSI(x):
    ser.write('c')
    ser.write(x)
    line = ""    
    while ("GW=" not in line):
        c = ser.read()
        line = ""
        while(c != '\n'):
            line = line + c
            c = ser.read()
    ser.write('c')
    time.sleep(0.5)
    c = ser.read()
    if(c == '$'):
            ser.write('H')
            time.sleep(0.5)
    ser.write('b')
    line = ""
    while ("RSSI=" not in line):
        c = ser.read()
        line = ""
        while(c != '\n'):
            line = line + c
            c = ser.read()
        if("ERR:" in line):
            ser.write('b')
    a = line.index("RSSI=")+6
    b = line.index(" dBm")-1
    rssi_val = int(line[a:b])
    return rssi_val

# Function to collect RSSI observations and calculate the avg RSSI to be used for localization.
def get_RSSI():
    rssi1_arr = []
    rssi2_arr = []
    rssi3_arr = []
    # Get the RSSI values for different routers count number of times.
    for i in range(0,observation_count,1):
        rssi1_arr.append(connect_get_RSSI('e'))
        rssi2_arr.append(connect_get_RSSI('f'))
        rssi3_arr.append(connect_get_RSSI('g'))
        print rssi1_arr, rssi2_arr, rssi3_arr
    rssi_final = []
    for i in range(0,observation_count,1):
        print rssi1_arr[i], rssi2_arr[i], rssi3_arr[i]
    
    # Compute the best average for the 3 RSSI values from the observation values.
    rssi_final.append(get_best_avg(rssi1_arr))
    rssi_final.append(get_best_avg(rssi2_arr))
    rssi_final.append(get_best_avg(rssi3_arr))
    return rssi_final

# Function to get the (x,y) coordinates after collecting the RSSI values.
def get_coordinates():
    #rssi = get_std_input()
    rssi = get_RSSI()
    print rssi
    predict_label(rssi[0],rssi[1],rssi[2])

# Initializes the system and runs the system
def init():
    init_print()
    loop_flag = True
    motion_state = motion_state_enum.stop
    # Entering the loop to perform user actions
    while loop_flag:
        curr_action = raw_input("Enter action: ")
        if curr_action in MOTION_ACTIONS:
            motion_state = MOTION_STATE[curr_action]
            ser.write(curr_action)
        elif curr_action == "l":
            if motion_state == motion_state_enum.stop:
                get_coordinates()
            else:
                print "Stop the bot!"
        elif curr_action == "z":
            print "Exiting the system!"
            return
        else:
            print "Please refer to the controls given above and enter a correct control!!"

# ===================================================================== EXECUTION =========================================================
# Fitting a Gaussian Naive Bayes model to the above training data.
nbmodel = GaussianNB()
nbmodel.fit(X, y)

# Initializes the serial communication on port 4.
ser = serial.Serial(4)
ser.write('c')
c = ser.read()
print c
# Check if we are in command mode already
if(c != '$'):                   #not in command mode
    print "Wifly going into command mode.."

# Start the system
init()





