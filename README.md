# Team VICI Synthesizer



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

## Non-functional Specifications 
  <li>The system is implemented using interrupts and threads to achieve concurrent execution of tasks</li>
  <li>All data and other resources that are accessed by multiple tasks are protected against errors caused by simultaneous access using mutexes and atomic access</li>
  <li>The code is structured such that it is easy to understand and it is maintainable</li>
</ol>

## System Tasks
The table below lists all tasks performed by the system, their method of implementation (thread or interrupt), their theoretical minimum initiation interval and measured maximum execution time, in the order of their priority.
<table align="center">
      <tr>
        <th align=center>Priority</th>
        <th align=center>Task </th>
        <th align=center>Type</th>
        <th align=center>Initiation Interval</th>
        <th align=center>Execution Time</th>
      </tr>
      <tr>
        <td align=center>-</td>
        <td align=center>sampleISR</td>
        <td align=center>Interrupt</td>
        <th align=center>45.5 &mu;s</th>
        <th align=center>22 &mu;s</th>
      </tr>
      <tr>   
        <td align=center>-</td>
        <td align=center>CAN_RX_ISR</td>
        <td align=center>Interrupt</td>
        <th align=center>0.7 ms</th>
        <th align=center>3 &mu;s</th>
      </tr>
      <tr>
        <td align=center>-</td>
        <td align=center>CAN_TX_ISR</td>
        <td align=center>Interrupt</td>
        <th align=center>0.7 ms</th>
        <th align=center>3 &mu;s</th>
      </tr>
      <tr>
        <td align=center>1</td>
        <td align=center>scanKeysTask</td>
        <td align=center>Thread</td>
        <th align=center>20 ms</th>
        <th align=center>405 &mu;s</th>
      </tr>
      <tr>
        <td align=center>2</td>
        <td align=center>decodeTask</td>
        <td align=center>Thread</td>
        <th align=center>25.2 ms</th>
        <th align=center>10 &mu;s</th>
      </tr>
      <tr>
        <td align=center>3</td>
        <td align=center>CAN_TX_KEY_Task</td>
        <td align=center>Thread</td>
        <th align=center>60 ms</th>
        <th align=center>5 &mu;s</th>
      </tr>
      <tr>
        <td align=center>4</td>
        <td align=center>CAN_TX_Task</td>
        <td align=center>Thread</td>
        <th align=center>60 ms</th>
        <th align=center>0.9 ms</th>
      </tr>
      <tr>
        <td align=center>5</td>
        <td align=center>displayUpdateTask</td>
        <td align=center>Thread</td>
        <th align=center>100 ms</th>
        <th align=center>17.6 ms</th>
      </tr>
    </table>

## Critical Instant Analysis

## Shared Data Structures

## Inter-Task Blocking Dependencies

## Advanced Features
### 1. Polyphony Up To 4 Keys
### 2. Sine, Triangle, Square, CML Waveforms
### 3. Delay with Adjustable Period
### 4. Tremolo with Joystick Control
### 5. Octave Selection
### 6. Keyboard Auto-Detect


```mermaid
  graph TD;
      A-->B;
      A-->C;
      B-->D;
      C-->D;
```
