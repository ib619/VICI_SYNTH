# VICI_SYNTH



## Core Specifications and Functionality
<ol>
  <li>The synthesizer plays the appropriate musical tone using a sawtooth wave when a key is pressed</li>
  <li>There is no prceptible delay between pressing a key and playing the tone</li>
  <li>There is a volume control with 16 increments</li>
  <li>OLED display shows the note in play and volume level</li>
  <li>Synthesizer sends a message to the serial port upon key press or release</li>
  <li>Synthesizer stops / plays a note when receiving an appropriate message on the serial port</li>
  <li>Serial port message for playing a note is of the form Pxy, where x represents the octave number and y is the note number in hexadecimal format</li>
  <li>Serial port message for releasing a note is of the form Rxy, where x represents the octave number and y is the note number in hexadecimal format</li>
</ol>
