guitar_hero_arduino
===================
Final project for SYDE 192L.  

The code generates a random sequence that is then outputted to LEDs on the Arduino board.  The lights show which buttons need to be pressed and when the 'strum' button is pressed, the state of the buttons is used by the program.

If the input is correct, the player advances.  If it is wrong, the game ends and the lights flash.

There is also a paues button if the user needs to pause for whatever reason.

The strum and pause buttons use interrupts to interact with the program, this allows me to write simpler code because the interrupts will always take priorty.

Other Group members:

Aaron Wiersma

Mousa Khan
