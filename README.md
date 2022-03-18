# VICI_SYNTH



## Core Specifications and Functionality
<ol>
  <li>The synthesizer plays the appropriate musical tone using a sawtooth wave when a key is pressed</li>
  <li>There is no perceptible delay between pressing a key and playing the tone</li>
  <li>There is volume control with 16 increments operated by turning a knob</li>
  <li>OLED display shows the note played and volume level</li>
  <li>Every 100ms the OLED display refreshes and LED LD3 on the MCU module toggles</li>
  <li>Synthesiser is configurable, during compilation or operation, to act as a sender module or receiver module. </li>
  <li>If the synthesiser is configured as a sender, it sends a message on the CAN bus whenever a key is pressed or released</li>
  <li>If the synthesiser is configured as a receiver, it plays a note or stops playing a note when it receives an appropriate message on the CAN bus</li>
  <li></li>
  <li></li>
  <li></li>
  <li></li>
</ol>


1. The synthesiser shall play the appropriate musical tone as a sawtooth wave when a key is pressed
2. There shall be no perceptible delay between pressing a key and the tone starting
3. There shall be a volume control with at least 8 increments, which shall be operated by turning
a knob
4. The OLED display shall show the name of the note being played and the current volume level
5. Every 100ms the OLED display shall refresh and LED LD3 on the MCU module shall toggle
6. The synthesiser shall be configurable, during compilation or operation, to act as a sender module
or receiver module.
7. If the synthesiser is configured as a sender, it shall send a message on the CAN bus whenever a
key is pressed or released
8. If the synthesiser is configured as a receiver, it shall play a note or stop playing a note when it
receives an appropriate message on the CAN bus
9. CAN bus messages for playing a note shall be an 8-byte sequence {0x50,x,y,0,0,0,0,0}, where
x is the octave number 0–8 and y is the note number as the number of semitones above the note
C
10. CAN bus messages for ending a note shall be an 8-byte sequence {0x52,x,y,0,0,0,0,0}, where
x is the octave number 0–8 and y is the note number as the number of semitones above the
note C. The values of x and y in a note end command shall be ignored by the receiver unless
polyphony is implemented as an advanced feature
