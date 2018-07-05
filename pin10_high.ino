#include <timer-api.h>

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
int prescaler = 230000;
int counterResetValue = 0;
int one = 0;
int two = 0;
int three = 0;
int four = 0;

boolean newData = false;

int incomingByte = 0;    // for incoming serial data

void setup() {
    Serial.begin(230400);
    Serial1.begin(230400);// opens serial port, sets data rate to 9600 bps
    PIOC->PIO_OER= 0x00200000;
    // put your setup code here, to run once: 84 000 000 / 2 / freq

}

void loop() {
    recvWithStartEndMarkers();
    if (newData == true) {
      
        strcpy(tempChars, receivedChars);
            // this temporary copy is necessary to protect the original data
            //   because strtok() used in parseData() replaces the commas with \0
        parseData();
        timer_init_ISR(TIMER_DEFAULT, TIMER_PRESCALER_1_2, prescaler-1);
        showParsedData();
        newData = false;
    }
}


//============

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;

    while (Serial1.available() > 0 && newData == false) {
        rc = Serial1.read();
        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
    
}

//============

void parseData() {      // split the data into its parts

    char * strtokIndx; // this is used by strtok() as an index

    strtokIndx = strtok(tempChars,",");      // get the first part - the string
    prescaler = atoi(strtokIndx); // copy it to messageFromPC

    strtokIndx = strtok(NULL, ",");
    counterResetValue = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    one = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    two = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    three = atoi(strtokIndx);
    
    strtokIndx = strtok(NULL, ",");
    four = atoi(strtokIndx);
 
    //strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
    //integerFromPC = atoi(strtokIndx);     // convert this part to an integer

    //strtokIndx = strtok(NULL, ",");
    //floatFromPC = atof(strtokIndx);     // convert this part to a float

}

//============

void showParsedData() {
    Serial1.print("101 ");
    Serial1.print(one);
    Serial1.print(four);
    Serial1.print(counterResetValue);
}

void timer_handle_interrupts(int timer) {

  static int i = 1;
    if(i % 2 == 1) {
      PIOC->PIO_SODR = 0x00200000;
    }
    if(i % 2 == 0) {
      PIOC->PIO_CODR =  0x00200000;
    }
    if(i>(one+1) & i<four) {
      PIOC->PIO_CODR =  0x00200000;
    }
    //if(i>three & i<four) {
     // PIOC->PIO_SODR =  0x00200000;
    //}
       
    i++;
    if(i>(counterResetValue-1)) {
      i = 0;
    }
    

    
}
