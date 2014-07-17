volatile int pauseState = LOW;
volatile int strumState = LOW;
int outPins[4] = {7, 8, 9, 10};
int buttonInputs[4] = {1, 4, 5, 6};
int inputStates[4];
// Don't want to use a 2D array
int nextNotes[4];
int currNotes[4] = {0, 0, 0, 0};
int pause = 2;
int strum = 3;
int difficulty = 0;
int clockPin = 0;
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
  // Clock pin output
  pinMode(clockPin, OUTPUT);
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
    // Adjustment to make sure we get correct array indexes
    int adjust = 0;
    digitalWrite(clockPin, HIGH);
    delay(5);
    digitalWrite(clockPin, LOW);
    // Output pins (should be from output range, not 0-2)
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(outPins[i], nextNotes[i]);
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
  for (int i = 0; !firstRun && i < difficulty && i < 5; i++)
  {
    randomSeed(millis());
    nextNotes[random(4)] = 1;
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
