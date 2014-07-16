volatile int pauseState = LOW;
volatile int strumState = LOW;
int outPins[4] = {7, 8, 9, 10};
int buttonInputs[4] = {3, 4, 5, 6};
int inputStates[4];
//int redOut = 4;
//int redIn = 5;
//int blue0 = 6;
//int blue1 = 7;
//int yellow0 = 8;
//int yellow1 = 9;
//int green0 = 10;
//int green1 = 11;
// Don't want to use a 2D array
int nextNotes[4];
int currNotes[4] = {0, 0, 0, 0};
int convNotes[3];
int pause = 2;
int strum = 3;
int difficulty = 0;
boolean correct = true;
boolean firstRun = true;
void setup()
{
  // Setup coloured buttons as inputs and output pins
  for (int currPin = 0; currPin < 4; currPin++)
  {
    pinMode(buttonInputs[currPin], INPUT);
    pinMode(outPins[currPin], OUTPUT);
  }
  // Pause interrupt
  attachInterrupt(0, PAUSE_HANDLER, CHANGE);
  // Strum
  attachInterrupt(1, STRUM_HANDLER, HIGH);
  // Just in case, serial
  Serial.begin(9600);
}
void loop()
{
  if (correct){
    difficulty++;
    randomNotes();
    // Bug checking/serial monitor output
    for (int i = 0; i < 4; i++)
      Serial.print(currNotes[i]);
    Serial.println(" ");
    for (int i = 0; i < 4; i++)
      Serial.print(nextNotes[i]);
    Serial.println(" ");
    convert();
    // Adjustment to make sure we get correct array indexes
    int adjust = 0;
    // Output pins (should be from output range, not 0-2)
    for (int i = 0; i < 3; i++)
    {
      digitalWrite(i, convNotes[i-adjust]);
    }
    delay(1000);
    // Check button pushes
    getInputs();
    correct = compareArrays();
    firstRun = false;
    // Shift array
    updateCurrent();
    // Reset
    strumState = LOW;
    for (int reset = 0; reset < 4; reset++)
      nextNotes[reset] = 0;
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
  if (strumState == LOW)
    return false;
  for (int note = 0; note < 4; note++)
  {
    if (inputStates[note] != currNotes[note])
      return false;
  }
  return true;
}
void randomNotes()
{
  for (int i = 0; i < difficulty && i < 5; i++)
  {
    randomSeed(millis());
    nextNotes[random(4)] = 1;
  }
}
void convert()
{
  // TEMP TEST
  for (int i=0; i < 3; i++)
  {
    convNotes[i] = nextNotes[i];
  }
}
void PAUSE_HANDLER()
{
  pauseState = !pauseState;
  while (pauseState == LOW)
  {
    // Wait
  }
}
void STRUM_HANDLER ()
{
  strumState = HIGH;
}
