volatile int pauseState = LOW;
volatile int strumState = LOW;
int outPins[4] = {7, 8, 9, 10};
volatile int speakerPin = 13;
volatile int buttonInputs[4] = {11, 4, 5, 6};
volatile int inputStates[4];
// Don't want to use a 2D array
int nextNotes[4];
int currNotes[4] = {0, 0, 0, 0};
int oldNotes[4] = {1,1,1,1};
int pause = 2;
int strum = 3;
int difficulty = 1;
int diffProgress = 0;
int clockPin = 12;
boolean correct = true;
boolean firstRun = true;
boolean uniqueNotes = true;
volatile int midFreq= 1500;
volatile int setFreq[4] = {-750, -250, 250, 750};

void setup()
{
  // Timer1 to 0, pre scale to 4 (/256)
  TCNT1 = 0;
  TCCR1B = (_BV(CS02));
  // Setup coloured buttons as inputs and output pins
  for (int currPin = 0; currPin < 4; currPin++)
  {
    pinMode(buttonInputs[currPin], INPUT);
    pinMode(outPins[currPin], OUTPUT);
  }
  // Speaker pin
  pinMode(speakerPin, OUTPUT);
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
  // Make sure last set of inputs was right
  if (correct){
    // Check if difficulty should increase
    if (diffProgress == 6)
    {
      difficulty++;
      diffProgress = 0;
      Serial.println ("Level up");
    }
    // Difficulty does not increase, but progress through level
    else
    {
      diffProgress++;  
    }
    do{
      // Generate random notes to output next
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
    while (pauseState == HIGH)
    {
      // Wait
    }
    // Clock HIGH and back to LOW
    digitalWrite(clockPin, HIGH);
    myDelay(6);
    digitalWrite(clockPin, LOW);
    // Write next set of lights
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(outPins[i], nextNotes[i]);
    }
    // Give time for button pushes and strum
    int delayCount = 8;
    do{
      myDelay(400);
      --delayCount;
    }while(delayCount - difficulty > 0);
    // Bug checking/serial monitor output
    Serial.println ("INPUTS READ: ");
    for (int i = 0; i < 4; i++)
      Serial.print(inputStates[i]);
    Serial.println(" ");
    // Determine if the inputs were correct
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
  // Inputs were wrong, flash lights
  else
  {
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(outPins[i], 1);
    }
    myDelay(1000);
    digitalWrite(clockPin, HIGH);
    myDelay(5);
    digitalWrite(clockPin, LOW);
    for (int i = 0; i < 4; i++)
    {
      digitalWrite(outPins[i], 0);
    }
    myDelay(1000);
    digitalWrite(clockPin, HIGH);
    myDelay(5);
    digitalWrite(clockPin, LOW);
  }
}

// Copy next notes to current notes
void updateCurrent()
{
  // Copy next notes to current notes
  for (int i = 0; i < 4; i++)
  {
    currNotes[i] = nextNotes[i];
  }
}

// Custom delay function because we weren't supposed to use built-in one
void myDelay(int millSec)
{
  // Reset timer
  TCNT1=0;
  // Two ms pass when TCNT1 = 125
  int totLoops = millSec / 2;
  while (totLoops != 0)
  {
    // If counter reached/exceeded 125, 2ms have passed
    if (TCNT1 >= 125){
      // Reset counter
      --totLoops;
      TCNT1 = 0;
    }
  }
}

// Read in inputs and save them
void getInputs()
{
  for (int inCount = 0; inCount < 4; inCount++)
  { 
    inputStates[inCount] = digitalRead(buttonInputs[inCount]);
  }
}

// Play a different note depending on buttons pressed
void playSound()
{
  int outFreq = midFreq;
  // Calculate frequency to use
  for (int currButton = 0; currButton < 4; currButton++)
  {
    if (inputStates[currButton] == 1)
      outFreq += setFreq[currButton];
  }
  tone(speakerPin, outFreq, 400);
}

// Check if inputs were correct
boolean compareArrays ()
{
  // No notes first time
  if (firstRun)
    return true;
  // Quit early
  if (strumState == LOW)
    return false;
  // Check that inputs are correct
  for (int note = 0; note < 4; note++)
  {
    if (inputStates[note] != currNotes[note])
    {
      return false;
    }
  }
  return true;
}

// Generate a new set of random notes to be outputted next
void randomNotes()
{
  // Randomly generate next set of notes
  for (int i = 0; i < difficulty && i < 6; i++)
  {
    // Seed random number generator with an unconnected pin
    randomSeed(analogRead(0));
    nextNotes[random(4)] = 1;
  }
}

// Pause
void PAUSE_HANDLER()
{
  pauseState = !pauseState;
}

// Strum
void STRUM_HANDLER ()
{
  // Only first strum per set of notes is counted
  if (strumState == LOW){
    //Serial.println("STRUM SET HIGH");
    // Read button states
    getInputs();
    // Play sound corresponding to button states
    playSound();
    strumState = HIGH;
  }
}
