int total_switch = 6;
int total_sensors = 3;
int switch_state[6];
int output_pins[] = {8,9,10,11,12,13};
int switch_pins[] = {2,3,4,5,6,7};
int sensor_input_pins[] = {A0,A1,A2};
int deviceID = 2;
int iters = 0;

int is_reading = 0;char command[20];int command_length = 0;

void sendSwitchCond(int no,int cond){
  char msg[6];
  msg[0] = 255;
  msg[1] = deviceID;
  msg[2] = 1;
  msg[3] = no+1;
  msg[4] = cond;
  Serial1.write(msg,5);
}

void sendSensorValue(int sensorNo, int val){
  char msg[6];
  msg[0] = 255;
  msg[1] = deviceID;
  msg[2] = 2;
  msg[3] = sensorNo + 1;
  msg[4] = val/4;
  Serial1.write(msg,5);
}


void executeCommand(){  
  if(command[1] == 1){
    if(command[2] <= total_switch) 
      digitalWrite(output_pins[command[2] - 1],command[3]);
  }
  else if(command[1] == 10){
      char msg[6];
      msg[0] = 255;
      msg[1] = deviceID;
      msg[2] = 10;
      msg[3] = total_switch;
      msg[4] = total_sensors;
      Serial1.write(msg,5);
  }  
}

void readCommand(){
  char temp;
  for(int i = 0; i < Serial1.available();i++){
    temp = Serial1.read();
    //Serial.write(temp);
    if(temp == -1){
      is_reading = 1;
      command_length = 0;
    }
    else if(is_reading){ 
      command[command_length] = temp;
      command_length++;
      if(command_length >= 4){        
        is_reading = 0;
        executeCommand();  
      }
    }
    
    if(is_reading && command_length == 1){
      if(command[0] != deviceID && command[0] != 0)
        is_reading = 0; 
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  iters = 0;
  for(int i = 0; i < total_switch; i++){
    pinMode(switch_pins[i],INPUT);
    pinMode(output_pins[i],OUTPUT);
    switch_state[i] = -1;    
  }
  
  Serial1.begin(9600);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int temp;
  for(int i = 0; i < total_switch;i++){
    temp = digitalRead(switch_pins[i]);
    if(temp != switch_state[i]){
      digitalWrite(output_pins[i],temp);
      switch_state[i] = temp;
      sendSwitchCond(i,temp);  
    }
  }
  readCommand();
  
  if(iters%20 == 0){
    for(int i = 0; i < total_sensors;i++){
      temp = analogRead(sensor_input_pins[i]);
      sendSensorValue(i, temp);
    }
  }
  iters++;
  delay(100);
}
