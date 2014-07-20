volatile int pauseState = LOW;
volatile int strumState = LOW;
int outPins[4] = {7, 8, 9, 10};
volatile int buttonInputs[4] = {13, 4, 5, 6};
int inputStates[4];
// Don't want to use a 2D array
int nextNotes[4];
int currNotes[4] = {0, 0, 0, 0};
int oldNotes[4] = {1,1,1,1};
int pause = 2;
int strum = 3;
int difficulty = 0;
int clockPin = 12;
boolean correct = true;
boolean firstRun = true;
boolean uniqueNotes = true;
void setup()
{
  // Setup coloured buttons as inputs and output pins
  for (int currPin = 0; currPin < 4; currPin++)
  {
    pinMode(buttonInputs[currPin], INPUT);
    pinMode(outPins[currPin], OUTPUT);
  }
  // Clock pin output
  pinMode(clockPin, OUTPUT);
  // Pause interrupt
  attachInterrupt(0, PAUSE_HANDLER, HIGH);
  // Strum
  attachInterrupt(1, STRUM_HANDLER, CHANGE);
  // Just in case, serial
  Serial.begin(9600);
}
void loop()
{
  if (correct){
    difficulty++;
    do{
      randomNotes();
      // Make sure notes aren't same as before
      for (int currNote = 0; !uniqueNotes && currNote < 4; currNote++)
      {
        if (oldNotes[currNote] != nextNotes[currNote])
          uniqueNotes = true;
      }
    }while(!uniqueNotes);
    // Bug checking/serial monitor output
    Serial.print("Level: ");
    Serial.println(difficulty);
    Serial.println("NEXT NOTES");
    for (int i = 0; i < 4; i++)
      Serial.print(nextNotes[i]);
    Serial.println(" ");
    Serial.println("CURRENT NOTES");
    for (int i = 0; i < 4; i++)
      Serial.print(currNotes[i]);
    Serial.println(" ");
    // Adjustment to make sure we get correct array indexes
    int adjust = 0;    
    while (pauseState == HIGH)
    {
      // Wait
    }
    digitalWrite(clockPin, HIGH);
    delay(5);
    digitalWrite(clockPin, LOW);
    // Output pins (should be from output range, not 0-2)
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(outPins[i], nextNotes[i]);
    }
    // Check button pushes
    //for (int i = 0; i < 6; i ++)
    //{
      //delay(500);
     // getInputs();
    //}
    delay(400);
    delay(400);
    delay(400);
    delay(400);
    delay(400);
    delay(400);
    // Bug checking/serial monitor output
    Serial.println ("INPUTS READ: ");
    for (int i = 0; i < 4; i++)
      Serial.print(inputStates[i]);
    Serial.println(" ");
    correct = compareArrays();
    firstRun = false;
    // Shift array
    updateCurrent();
    // Reset
    strumState = LOW;
    Serial.println("Strum SET LOW");
    // Deep copy of next notes and wipe of next notes
    for (int reset = 0; reset < 4; reset++)
    {
      oldNotes[reset] = nextNotes[reset];
      nextNotes[reset] = 0;
    }
    uniqueNotes = false;
  }
  else
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(outPins[i], 1);
    }
    delay(1000);
    digitalWrite(clockPin, HIGH);
    delay(5);
    digitalWrite(clockPin, LOW);
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(outPins[i], 0);
    }
    delay(1000);
    digitalWrite(clockPin, HIGH);
    delay(5);
    digitalWrite(clockPin, LOW);
  }
}

void updateCurrent()
{
  // Copy next notes to current notes
  for (int i = 0; i < 4; i++)
  {
    currNotes[i] = nextNotes[i];
  }
}

void getInputs()
{
  for (int inCount = 0; inCount < 4; inCount++)
  { 
    inputStates[inCount] = digitalRead(buttonInputs[inCount]);
  }
}
boolean compareArrays ()
{
  if (firstRun)
    return true;
 // if (strumState == LOW)
  //  return false;
  for (int note = 0; note < 4; note++)
  {
    if (inputStates[note] != currNotes[note])
    {
      return false;
    }
  }
  return true;
}
void randomNotes()
{
  for (int i = 0; i < difficulty && i < 6; i++)
  {
    randomSeed(analogRead(0));
    nextNotes[random(4)] = 1;
  }
}
void PAUSE_HANDLER()
{
  //pauseState = !pauseState;
}
void STRUM_HANDLER ()
{
  if (strumState == LOW){
    //Serial.println("STRUM SET HIGH");
    getInputs();
    strumState = HIGH;
  }
}
