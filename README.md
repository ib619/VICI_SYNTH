# Team VICI Synthesizer

- [Team VICI Synthesizer](#team-vici-synthesizer)
  * [Core Functional Specifications](#core-functional-specifications)
  * [Non-functional Specifications](#non-functional-specifications)
  * [System Tasks](#system-tasks)
  * [Critical Instant Analysis](#critical-instant-analysis)
  * [Shared Data Structures](#shared-data-structures)
  * [Inter-Task Blocking Dependencies](#inter-task-blocking-dependencies)
  * [Advanced Features](#advanced-features)
    + [1. Polyphony Up To 4 Keys](#1-polyphony-up-to-4-keys)
    + [2. Sine, Triangle, Square, CML Waveforms](#2-sine--triangle--square--cml-waveforms)
    + [3. Delay with Adjustable Period](#3-delay-with-adjustable-period)
    + [4. Tremolo with Joystick Control](#4-tremolo-with-joystick-control)
    + [5. Octave Selection](#5-octave-selection)
    + [6. Keyboard Auto-Detect](#6-keyboard-auto-detect)
  * [User Interface](#user-interface)

<small><i><a href='http://ecotrust-canada.github.io/markdown-toc/'></a></i></small>


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
        <td align=center><code>sampleISR</code></td>
        <td align=center>Interrupt</td>
        <th align=center>45.5 &mu;s</th>
        <th align=center>22 &mu;s</th>
      </tr>
      <tr>   
        <td align=center>-</td>
        <td align=center><code>CAN_RX_ISR</code></td>
        <td align=center>Interrupt</td>
        <th align=center>0.7 ms</th>
        <th align=center>3 &mu;s</th>
      </tr>
      <tr>
        <td align=center>-</td>
        <td align=center><code>CAN_TX_ISR</code></td>
        <td align=center>Interrupt</td>
        <th align=center>0.7 ms</th>
        <th align=center>3 &mu;s</th>
      </tr>
      <tr>
        <td align=center>1</td>
        <td align=center><code>scanKeysTask</code></td>
        <td align=center>Thread</td>
        <th align=center>20 ms</th>
        <th align=center>405 &mu;s</th>
      </tr>
      <tr>
        <td align=center>2</td>
        <td align=center><code>decodeTask</code></td>
        <td align=center>Thread</td>
        <th align=center>25.2 ms</th>
        <th align=center>360 &mu;s</th>
      </tr>
      <tr>
        <td align=center>3</td>
        <td align=center><code>CAN_TX_KEY_Task</code></td>
        <td align=center>Thread</td>
        <th align=center>60 ms</th>
        <th align=center>180 &mu;s</th>
      </tr>
      <tr>
        <td align=center>4</td>
        <td align=center><code>CAN_TX_Task</td>
        <td align=center>Thread</td>
        <th align=center>60 ms</th>
        <th align=center>7.2 ms</th>
      </tr>
      <tr>
        <td align=center>5</td>
        <td align=center><code>displayUpdateTask</code></td>
        <td align=center>Thread</td>
        <th align=center>100 ms</th>
        <th align=center>17.6 ms</th>
      </tr>
    </table>

## Critical Instant Analysis
Using the times in the table above and the latency formula: 
<p align="center">
<img src="/images/latency.png" alt="latency" width="250"/>
</p>
The latency of the lowest priority task <code>displayUpdateTask</code> is <strong>85.05 ms</strong>, which is less than the initiation interval of <strong>100 ms</strong> for the task. We can then say that the system has passed the critical instant analysis and that the given schedule works. Consequently, the total CPU utilization is <strong>85.05 %</strong>.

## Shared Data Structures and Blocking Dependencies
The mechanism for sharing data between different tasks and interrupts follows the publisher/subscriber pattern. This means that for each data structure shared, only one task is responsible for updating the data while the other tasks are read only.

There are 4 major structural blocks of the code: 
<ol>
<li>Keyboard, Knob and Joystick scanning</li>
<li>CAN transmission and reception</li>
<li>Sample generation and playback</li>
<li>Display update</li>
</ol>

All the data is generated inside the key scanning task. If the keyboard is configured as a sender, the key press information is stored in a volatile array and passed to the CAN transmission via freeRTOS queue. On the other hand if the keyboard is configured as a receiver, the data will only be stored but not transmitted.

Received key press information is decoded into separate volatile array.
Sample generation interrupt combines the information into a third array, which is only accessed in the display updating task, achieving protected read/write access.

The aformentioned structure allowed us to minimise the use of blocking dependencies for data protection, thus minimising the risk of deadlock.

## Advanced Features
The pictures below demonstrate the implementation of several advanced features. All of the waveforms present here come from oscilloscope measurements on our synthesizer modules.

### 1. Polyphony Up To 4 Keys
### 2. Selectable Waveforms: Sine, Triangle, Square, Sawtooth, CML Waveforms
Waveform are stored in constant look up tables(LUTs). For each waveform the LUT contains the a complete cycle of each note in octave 4, sampled at 22kHz. This method of storing the waveforms allows to store any waveform imaginable, 5 of which are listed in the table below.

The one disadvantage of this approach is that frequencies are discrete, therefore implementation of any frequency modulating effects such as vibrato is very difficult. On the other hand, array lookup always takes the same time regardless of size, hence this can support many waveforms and still be computationally eficcient.

The desired waveform is displayed in the UI and can be selected by rotating knob 2. The table below showcases oscilloscope measurements for each of the possible waveforms.
<table align="center">
  <tr>
      <th align=center>Type</th>
      <th align=center>Waveform</th>
      <th align=center>Comments</th>
  </tr>
 <tr>
      <th align=center>Sine</th>
      <th align=center><img src="/images/sine.png" alt="sine" width="400"/></th>
      <th align=center>Note: C3<br>Measured Frequency: 129.29 Hz</th>
  </tr>
  <tr>
      <th align=center>Triangle</th>
      <th align=center><img src="/images/triangle.png" alt="triangle" width="400"/></th>
      <th align=center>Note: C3<br>Measured Frequency: 129.60 Hz<br>Parasitic capacitance affects the waveform</th>
  </tr>
 <tr>
      <th align=center>Square</th>
      <th align=center><img src="/images/square.png" alt="square" width="400"/></th>
      <th align=center>Note: C3<br>Measured Frequency: 131.71 Hz<br>Frequency response of the filter heavily attenuates higher frequencies</th>
  </tr>
 <tr>
      <th align=center>Sawtooth</th>
      <th align=center><img src="/images/saw.png" alt="saw" width="400"/></th>
      <th align=center>Note: C3<br>Measured Frequency: 129.35 Hz<br>Parasitic capacitance affects the wave significantly</th>
  </tr>
 <tr>
      <th align=center>CML</th>
      <th align=center><img src="/images/CML.png" alt="CML" width="400"/></th>
      <th align=center>Note: C3<br>Measured Frequency: 129.27 Hz</th>
 </tr>
</table>

### 3. Delay with Adjustable Period
The delay effect can be enabled or disabled by pressing knob 1. The delay period is displayed in the UI and it can be adjusted by rotating knob 1. The waveform below shows the delay effect for 2 consecutive key presses.
<p align="center">
<img src="/images/delay.png" alt="delay" width="500"/>
</p>

### 4. Tremolo with Joystick Control
Tremolo is a form of Amplitude Modulation where the gain of an audio signal is changed at a very slow rate, often at a frequency below the range of hearing (approximately 20 Hz) which creates a change in volume. This effect is commonly used to alter the sound of organs or electric guitar and hence it has many practical applications.

This has been implemented using a Low frequency oscillator (LFO) which outputs the samples of a traingular waveform with amplitudes ranging from 0-1. Given that the sampling frequency (fs) is fixed at 22kHz, the desired frequency (f) of the waveform created by the LFO achieved by altering the step size (S) where S = 1/(2xf) x fs. The phase accumaltor for this is multiplied with the phase accumalator for the notes pressed to achieve the tremolo effect.

The joystick position is mapped using an X and Y value which range roughly from 100-900. The addition of these values is used as the number of steps(N) of the waveform which changes the step size (S), S = 1/N, and hence frequency. This value is scaled to ensure that the movement of the joystick has a significantly recognisable Tremolo effect.
<p align="center">
<img src="/images/tremolo.png" alt="tremolo" width="500"/>
</p>

### 5. Octave Selection
Considering that the samples are coming from the LUT where they are stored only for 4th octave, the method of switching octaves is as follows:
<ul>
<li>If the 4th octave is chosen, the samples are just played back as they were recorded.</li>
<li>If the octave chosen is below 4th, each sample would be repeated a number of times to achieve the desired frequency. For example, for the 3rd octave, each sample is played twice</li>
<li>If the octave chosen is above 4th, some of the samples will be skipped, producing higher pitch. For example, for the 5th octave, every second sample will be skipped.</li>

This method has one disadvantage, at the highest notes there might be not enough samples to accurately represent the waveform. This issue can be solved by increacing the sampling frequency of the device.
</ul>

## User Interface
### Controls
The system controls are listed below. Knobs are numbered from left to right.

Knob 1 : Press to activate delay and rotate to change delay time <br>
Knob 2 : Rotate to change waveform <br>
Knob 3 : Press to switch to helper menu and rotate to change volume <br>
Knob 4 : Rotate to change octaves and press to cycle helper menu pages <br>
Joystick: Press to activate Tremolo and move to adjust Tremolo <br>

### Main UI Screen
This is the UI Screen that greets the user on start-up.

<p align="center">
<img src="/images/UI.jpg" alt="UI" width="400"/>
</p>

### Helper Menu Pages
Access by pressing Knob 3 and change page by pressing Knob 4.
<p align="center">
<img src="/images/help_knob1.jpg" alt="help_knob1" width="400"/> <img src="/images/help_knob2.jpg" alt="help_knob2" width="400"/> <br>
<img src="/images/help_knob3.jpg" alt="help_knob3" width="400"/> <img src="/images/help_knob4.jpg" alt="help_knob4" width="400"/> <br>
<img src="/images/help_joy.jpg" alt="help_joy" width="400"/>
</p>

## Real Time System Architecture Decisions 


Asynchronus vs Synchronus. more flexible and robust system.
queue is better since
A queue is a FIFO buffer for passing information between tasks
The buffer allows a mismatch in processing rates between writing task and reading task
A queue can be used to give a task more time and lower worst-case utilisation
Queue allows critical instant analysis to consider average, not peak initiation interval
Can also lower task priority even if initiations are not bursty

## CAN Communication Decisions
The two CAN messaging formats were explored:

### 1. 8-byte sequence: {note1, note2, note3, note4, octave, 0, 0, 0}
This message format allow passing multiple note information in one message, which makes decoding easy and less time consuming. The downside of this format is that it does not scale well with multiple keyboards, requiring each keyboard to use its own can ID.

### 2. 8-byte sequence: {P/R, note, octave, 0, 0, 0, 0, 0}
This message format has an advantage of supporting multiple keyboards without overcomplicating the message id recognition. However it adds more complexity to the decode logic and increaces the overall amount of messages going through the system. This method requires a higher communication bandwidth.

The system at its current state uses the second messaging format. The keyboard module configures as a sender/receiver in the setup via the handshake function.

### Handshake
The system would be configured as a receiver if it has either both or neither side attached on the startup. In case there is only one side attached, it will be configured as the sender. This way, the keyboard which is powered first will automatically become the receiver, and the ones attached afterwards will become the senders.

Also, under the restriction of 3 keyboards for team, this handshake allows to connect and power all 3 keyboards simultaneously. In this case, the middle one will become the receiver and the sides become the senders.

## AnalogWrite Timing and other optimisations

analogWrite and analogRead (used in the sampleISR and scanKeysTask respectively) represent 45% and 23% of the **maximium** exection time for their tasks. Each analogueWrite and analogRead takes around 10 μs and 94 μs respectively. This means that the maximium speeds these function are run is more than sufficient for a synth to produce music/audio frequencies in the audible spectrum.

However, for systems with more strict time requirements it may be a problem, however there is a way around this. For example the AnalogWrite function has a delay due to the branching to decide if DAC or PWM is to be used, however on some pins both operations are completed. AnalogWrite first looks up the DAC address for a given pin by calling get_dac_channel(), secondly if uninitialised, HAL_DAC_Init() and HAL_DAC_ConfigChannel() are called.

The first operation can be hardcoded and the second operation can be completed on startup. Finally, HAL_DAC_SetValue() and HAL_DAC_Start() can be used in the ISR function instead to write new values and update the output. This should reduce the maximium execution time of the analogWrite operation and hence the ISR.
