# VICI_SYNTH



## Core Functional Specifications
<ol>
  <li>The synthesizer plays the appropriate musical tone using a sawtooth wave when a key is pressed</li>
  <li>There is no perceptible delay between pressing a key and playing the tone</li>
  <li>There is volume control with 16 increments operated by turning a knob</li>
  <li>OLED display shows the note played and volume level</li>
  <li>Every 100ms the OLED display refreshes and LED LD3 on the MCU module toggles</li>
  <li>Synthesiser is configurable, during compilation or operation, to act as a sender module or receiver module. </li>
  <li>If the synthesiser is configured as a sender, it sends a message on the CAN bus whenever a key is pressed or released</li>
  <li>If the synthesiser is configured as a receiver, it plays a note or stops playing a note when it receives an appropriate message on the CAN bus</li>
  <li>CAN bus messages for playing a note are an 8-byte sequence {0x50,x,y,0,0,0,0,0}, where x is the octave number 0–8 and y is the note number as the number of semitones above the note C</li>
  <li>CAN bus messages for ending a note are an 8-byte sequence {0x52,x,y,0,0,0,0,0}, where x is the octave number 0–8 and y is the note number as the number of semitones above the note C. The values of x and y in a note end command are not ignored by the receiver, because polyphony is implemented as an advanced feature</li>
  <li></li>
  <li></li>
</ol>



9. CAN bus messages for playing a note shall be an 8-byte sequence {0x50,x,y,0,0,0,0,0}, where
x is the octave number 0–8 and y is the note number as the number of semitones above the note
C
10. CAN bus messages for ending a note shall be an 8-byte sequence {0x52,x,y,0,0,0,0,0}, where
x is the octave number 0–8 and y is the note number as the number of semitones above the
note C. The values of x and y in a note end command shall be ignored by the receiver unless
polyphony is implemented as an advanced feature



```mermaid
  graph TD;
      A-->B;
      A-->C;
      B-->D;
      C-->D;
```
