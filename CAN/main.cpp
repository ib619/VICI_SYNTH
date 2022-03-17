#include <Arduino.h>
#include <U8g2lib.h>
#include <STM32FreeRTOS.h>
#include <ES_CAN.h>

//Constants
  const uint32_t interval = 100; //Display update interval

// CAN Init bool
bool can_init = false;

//Pin definitions
  //Row select and enable
  const int RA0_PIN = D3;
  const int RA1_PIN = D6;
  const int RA2_PIN = D12;
  const int REN_PIN = A5;

  //Matrix input and output
  const int C0_PIN = A2;
  const int C1_PIN = D9;
  const int C2_PIN = A6;
  const int C3_PIN = D1;
  const int OUT_PIN = D11;

  //Audio analogue out
  const int OUTL_PIN = A4;
  const int OUTR_PIN = A3;

  //Joystick analogue in
  const int JOYY_PIN = A0;
  const int JOYX_PIN = A1;

  //Output multiplexer bits
  const int DEN_BIT = 3;
  const int DRST_BIT = 4;
  const int HKOW_BIT = 5;
  const int HKOE_BIT = 6;

//Display driver object
U8G2_SSD1305_128X32_NONAME_F_HW_I2C u8g2(U8G2_R0);

// Mutex
static uint8_t keyArray[7];
SemaphoreHandle_t keyArrayMutex;

// CAN queue handler
QueueHandle_t msgInQ;


// waveform generation
volatile int16_t currentIndex;
const int32_t stepSizes [] = {51076922, 54112683, 57330004, 60740598, 64352275, 68178701, 72231588, 76528508, 81077269, 85899346, 91006452, 96418111, 0};
const int32_t stepSizeLUT [] = {141515, 131515, 111515, 71515, 151415, 151315, 151115, 150715, 151514, 151513, 151511, 151507};
const String Notes [] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B", "--"};
const double frequencies [] = {261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392, 415.3, 440, 466.16, 493.88, 0};

// sinewave lookup tables
const int16_t sine_lut_sizes [] = {42, 39, 37, 35, 33, 31, 29, 28, 26, 25, 23, 22, 0};

const int16_t sine_lut_start_index [] = {0, 43, 83, 121, 157, 191, 223, 253, 282, 309, 335, 359, 0};

const int32_t sine_lut [] = {
  0, 319726491, 632326042, 930830579, 1208586214, 1459401569, 1677685785, 1858573144, 1998031535, 2092952327, 2141219664, 2141757633, 2094554241, 2000661687, 1862172906, 1682174915, 1464680001, 1214536288, 937319664, 639209491, 326850867, 7206495, -312598515, -625435473, -924331012, -1202622529, -1454106702, -1673177762, -1854952453, -1995378883, -2091326843, -2140657582, -2142271482, -2096132568, -2003269309, -1865751697, -1686645101, -1469941939, -1220472685, -943798193, -646085742, -333971562, -14412909,
  0, 338587403, 668704928, 982094572, 1270916787, 1527946587, 1746754286, 1921866333, 2048902242, 2124684166, 2147316395, 2116232776, 2032210876, 1897352532, 1715031270, 1489807920, 1227316518, 934123378, 617562825, 285553728, -53598593, -391410126, -719430402, -1029453883, -1313725225, -1565133286, -1777389006, -1945182738, -2064317063, -2131811796, -2145978535, -2106462892, -2014253365, -1871656608, -1682239725, -1450741045, -1182951585, -885570190, -566035957, -232342141,
  0, 358534288, 707004107, 1035627508, 1335179655, 1597251774, 1814487201, 1980787887, 2091485581, 2143472872, 2135290416, 2067167904, 1941017613, 1760380728, 1530327933, 1257317074, 949011881, 614066833, 261884223, -97649777, -454442636, -798478768, -1120100684, -1410280086, -1660871305, -1864839959, -2016460412, -2111476504, -2147221024, -2122690583, -2038573778, -1897231864, -1702632472, -1460238230, -1176853423, -860432988, -519859210, -164692386,
  0, 379632637, 747307112, 1091441937, 1401197107, 1666815551, 1879930474, 2033828907, 2123663155, 2146603504, 2101927348, 1991041955, 1817440138, 1586590231, 1305763845, 983806813, 630860557, 258042637, -122903446, -499978157, -861303906, -1195499187, -1492037081, -1741576855, -1936258179, -2069948729, -2138437346, -2139566686, -2073301177, -1941728136, -1748992020, -1501163882, -1206050135, -872946655, -512345968, -135606742,
  0, 401946457, 789686009, 1149513895, 1468711903, 1735997893, 1941924569, 2079213398, 2143011868, 2131065006, 2043795077, 1884286659, 1658177622, 1373459849, 1040196768, 670167652, 276451279, -127036339, -526033823, -906438500, -1254804864, -1558819811, -1807737852, -1992760911, -2107349298, -2147452857, -2111654114, -2001218388, -1820049058, -1574549604, -1273397269, -927236364, -548302040, -149987833,
  0, 425544038, 834210839, 1209792522, 1537393376, 1804020628, 1999099740, 2114893800, 2146810372, 2093583633, 1957324576, 1743437286, 1460404613, 1119451742, 734100992, 319635518, -107506821, -530385399, -932228691, -1297099441, -1610526737, -1860079933, -2035861656, -2130900336, -2141426703, -2067023277, -1910640926, -1678481834, -1379753517, -1026303655, -632150201, -212925431,
  0, 450479597, 880913360, 1272147472, 1606772458, 1869897882, 2049814960, 2138517585, 2132058590, 2030725394, 1839027210, 1565494389, 1222298834, 824712361, 390427118, -61231692, -510165760, -936398027, -1320961639, -1646743948, -1899248010, -2067237678, -2143237602, -2123865871, -2009984505, -1806661097, -1522943308, -1171456260, -767840731, -330057155,
  0, 476828985, 929852164, 1336452242, 1676329609, 1932515803, 2092220664, 2147470888, 2095508092, 1938926533, 1685543584, 1348009452, 943175612, 491253495, 14805425, -462381810, -916484518, -1324831508, -1667035955, -1926013216, -2088833789, -2147368814, -2098695916, -1945245102, -1694677442, -1359502589, -956454230, -505654655, -29610146,
  0, 504665114, 981063574, 1402511965, 1745404660, 1990535995, 2124175990, 2138839373, 2033704838, 1814661044, 1493976791, 1089613833, 624220827, 123864768, -383429046, -869246725, -1306377275, -1670336703, -1940739381, -2102439862, -2146381187, -2070102172, -1877875257, -1580467211, -1194536069, -741698110, -247317110,
  0, 534057466, 1034558137, 1470053716, 1813180413, 2042378316, 2143246079, 2109445808, 1943101299, 1654664589, 1262259217, 790541457, 269151070, -269151070, -790541457, -1262259217, -1654664589, -1943101299, -2109445808, -2143246079, -2042378316, -1813180413, -1470053716, -1034558137, -534057466, 0,
  0, 565083370, 1090337805, 1538741643, 1878689906, 2086221921, 2146710149, 2055891181, 1820166236, 1456149985, 989499491, 453105815, -115224249, -675432929, -1188034873, -1616900210, -1931801098, -2110542287, -2140525510, -2019637450, -1756398696, -1369363181, -885810443, -339822869,
  0, 597815320, 1148368767, 1608134881, 1940765637, 2119964028, 2131563051, 1974645716, 1661617537, 1217225785, 676603027, 82489626, -518145210, -1077816706, -1552278549, -1904020903, -2105235844, -2140015794, -2005611131, -1712647571, -1284286125, -754392040, -164857495
};

// Knob detection structure
class Knob {
  private:
    const int8_t increment [16] = {0, 1, -1, 2, -1, 0, 2, 1, 1, 2, 0, -1, 2, -1, 1, 0};
    int16_t *state;
    int16_t lowerLimit;
    int16_t upperLimit;
    int8_t selector;
    int8_t currentValue;
    int8_t previousValue;
    int8_t prevRotDir;
  public:
    int16_t count = 0;

    Knob (int16_t st[16], int16_t l, int16_t u, int8_t s){
      state = st;
      lowerLimit = l;
      upperLimit = u;
      selector = s;
    }

    void decode(const int8_t value){
      previousValue = currentValue;
      currentValue = value&selector;
      int16_t my_state = previousValue*100 + currentValue;

      for (int8_t i = 0; i < 17; i++){
        if(my_state == *(state+i)){
          if(increment[i] != 2){
            count += increment[i];
          } else {
            count += prevRotDir*2;
          }
          if(increment[i] == -1 || increment[i] == 1){prevRotDir == increment[i];}
          break;
        }
      }

      // Limiting values
      if(count > upperLimit){count = upperLimit;}
      if(count < lowerLimit){count = lowerLimit;}
    }
};

int16_t knob2_state[] = {0, 4, 8, 12, 400, 404, 408, 412, 800, 804, 808, 812, 1200, 1204, 1208, 1212};
Knob knob2_obj(knob2_state, 0, 32, 12);

int16_t knob3_state[] = {0, 1, 2, 3, 100, 101, 102, 103, 200, 201, 202, 203, 300, 301, 302, 303};
Knob knob3_obj(knob3_state, 0, 32, 3);
Knob knob0_obj(knob2_state, 0, 32, 12);
Knob knob1_obj(knob3_state, 0, 32, 3);



//Function to set outputs using key matrix
void setOutMuxBit(const uint8_t bitIdx, const bool value) {
      digitalWrite(REN_PIN,LOW);
      digitalWrite(RA0_PIN, bitIdx & 0x01);
      digitalWrite(RA1_PIN, bitIdx & 0x02);
      digitalWrite(RA2_PIN, bitIdx & 0x04);
      digitalWrite(OUT_PIN,value);
      digitalWrite(REN_PIN,HIGH);
      delayMicroseconds(2);
      digitalWrite(REN_PIN,LOW);
}

//Function to read collumns
uint8_t readCols(){
      uint8_t c0 = digitalRead(C0_PIN);
      uint8_t c1 = digitalRead(C1_PIN) << 1;
      uint8_t c2 = digitalRead(C2_PIN) << 2;
      uint8_t c3 = digitalRead(C3_PIN) << 3;

      return (c0 + c1 + c2 + c3);
}

void setRow(uint8_t rowIdx){
  //disable decoder
  digitalWrite(REN_PIN, LOW);

  
  //select the row
  switch (rowIdx){
    case 0:
      digitalWrite(RA0_PIN, LOW);
      digitalWrite(RA1_PIN, LOW);
      digitalWrite(RA2_PIN, LOW); 
      break;

    case 1:
      digitalWrite(RA0_PIN, HIGH);
      digitalWrite(RA1_PIN, LOW);
      digitalWrite(RA2_PIN, LOW); 
      break;

    case 2:
      digitalWrite(RA0_PIN, LOW);
      digitalWrite(RA1_PIN, HIGH);
      digitalWrite(RA2_PIN, LOW); 
      break;

    case 3:
      digitalWrite(RA0_PIN, HIGH);
      digitalWrite(RA1_PIN, HIGH);
      digitalWrite(RA2_PIN, LOW); 
      break;

    case 4:
      digitalWrite(RA0_PIN, LOW);
      digitalWrite(RA1_PIN, LOW);
      digitalWrite(RA2_PIN, HIGH); 
      break;

    default:
      Serial.println("Row address exceeds 3");
  }
  
  //enable decoder
  digitalWrite(REN_PIN, HIGH);

}

int8_t decodeKey(const int8_t bit0, const int8_t bit1, const int8_t bit2){
  int32_t value = bit0*10000 + bit1*100 + bit2;

  for(int8_t i = 0; i < 12; i++){
    if (stepSizeLUT[i] == value){
      return i;
    }
  }

  return 12;
}

volatile int8_t pressed_keys[4] = {12};
volatile int8_t previous_pressed_keys[4] = {12};
volatile int8_t played_notes[4] = {12};
const int16_t key_select [] = {256, 512, 1024, 2048, 16, 32, 64, 128, 1, 2, 4, 8};
const int16_t key_power [] = {8, 9, 10, 11, 4, 5, 6, 7, 0, 1, 2, 3};

uint8_t TX_Message[8] = {0};
void decodeMultipleKeys(const int8_t w0, const int8_t w1, const int8_t w2){
  int8_t index = 0;
  int16_t value = (w0 << 8) + (w1 << 4) + w2;

  // CAN message container
  

  // previous press
  for(int8_t i = 0; i < 4; i++){
    previous_pressed_keys[i] = pressed_keys[i];
  }

  // Reset pressed keys
  for(int8_t i = 0; i < 4; i++){
    pressed_keys[i] = 12;
  }

  // Scan for new keys
  for (int8_t i = 0; i < 12; i++){
    if(index > 3){break;}
    if((value&key_select[i])>>key_power[i] == 0){
      pressed_keys[index] = i;
      index++;
    }
  }

  // Format message
  TX_Message[0] = 4;
  TX_Message[1] = pressed_keys[0];
  TX_Message[2] = 4;
  TX_Message[3] = pressed_keys[1];
  TX_Message[4] = 4;
  TX_Message[5] = pressed_keys[2];
  TX_Message[6] = 4;
  TX_Message[7] = pressed_keys[3];

  // Detect changes
  bool no_changes_detected = true;
  for (int8_t i = 0; i < 4; i++){
    no_changes_detected = no_changes_detected && (pressed_keys[i] == previous_pressed_keys[i]);
  }

  // Send message if change has been detected
  if(!no_changes_detected && can_init){
    CAN_TX(0x123, TX_Message);
  }
  
}

void CAN_RX_ISR (void){
  uint8_t RX_Message_ISR[8];
  uint32_t ID;
  CAN_RX(ID, RX_Message_ISR);
  xQueueSendFromISR(msgInQ, RX_Message_ISR, NULL);
  
  }

uint8_t RX_Message[8] = {12};
void decodeTask(void * pvParameters){
  
  while(1){
    xQueueReceive(msgInQ, RX_Message, portMAX_DELAY);
    played_notes[0] = RX_Message[1];
    played_notes[1] = RX_Message[3];
    played_notes[2] = RX_Message[5];
    played_notes[3] = RX_Message[7];
  }
}


void scanKeysTask(void * pvParameters){

  const TickType_t xFrequency = 20/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1){
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    // keypress detection code

    xSemaphoreTake(keyArrayMutex, portMAX_DELAY);
    for (int i = 0; i < 5; i++){
      // select the row to read
      setRow(i);
      delayMicroseconds(3);

      //record the value into the array
      keyArray[i] = readCols();
      
    }

    currentIndex = decodeKey(keyArray[0], keyArray[1], keyArray[2]);
    decodeMultipleKeys(keyArray[0], keyArray[1], keyArray[2]);
    knob2_obj.decode(keyArray[3]);
    knob3_obj.decode(keyArray[3]);
    knob1_obj.decode(keyArray[4]);
    knob0_obj.decode(keyArray[4]);
    xSemaphoreGive(keyArrayMutex);
  }
}

void displayUpdateTask(void * pvParameters){
  const TickType_t xFrequency = 100/portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while(1){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(22,10);
    u8g2.print(Notes[played_notes[0]]);
    u8g2.setCursor(42,10);
    u8g2.print(Notes[played_notes[1]]);
    u8g2.setCursor(62,10);
    u8g2.print(Notes[played_notes[2]]);
    u8g2.setCursor(82,10);
    u8g2.print(Notes[played_notes[3]]);
    u8g2.setCursor(22,20);
    u8g2.print(played_notes[0]);
    u8g2.setCursor(42,20);
    u8g2.print(played_notes[1]);
    u8g2.setCursor(62,20);
    u8g2.print(played_notes[2]);
    u8g2.setCursor(82,20);
    u8g2.print(played_notes[3]);
    u8g2.setCursor(22,30);
    u8g2.print(TX_Message[1]);
    u8g2.setCursor(42,30);
    u8g2.print(TX_Message[3]);
    u8g2.setCursor(62,30);
    u8g2.print(TX_Message[5]);
    u8g2.setCursor(82,30);
    u8g2.print(TX_Message[7]);
    u8g2.sendBuffer();
  }
}

void sampleISR(){
  static int32_t phaseAcc = 0;
  static int16_t counter = 0;
  static int16_t lenCount = 0;

  static int32_t phaseAcc1 = 0;
  static int16_t counter1 = 0;
  static int16_t lenCount1 = 0;

  static int32_t phaseAcc2 = 0;
  static int16_t counter2 = 0;
  static int16_t lenCount2 = 0;

  static int32_t phaseAcc3 = 0;
  static int16_t counter3 = 0;
  static int16_t lenCount3 = 0;

  static int32_t phaseAccTotal = 0;

  if(played_notes[0] != 12){phaseAcc = sine_lut[sine_lut_start_index[played_notes[0]]+lenCount];} else {phaseAcc = 0; counter = 0; lenCount = 0;}
  if(counter == 1){
    counter = 0;
    if(lenCount >= sine_lut_sizes[played_notes[0]]){
    lenCount = 0;
    } else {
    lenCount+=1;
  }
  } else {
    counter++;
  }

  if(played_notes[1] != 12){phaseAcc1 = sine_lut[sine_lut_start_index[played_notes[1]]+lenCount1];} else {phaseAcc1 = 0; counter1 = 0; lenCount1 = 0;}
  if(counter1 == 1){
    counter1 = 0;
    if(lenCount1 >= sine_lut_sizes[played_notes[1]]){
    lenCount1 = 0;
    } else {
    lenCount1+=1;
  }
  } else {
    counter1++;
  }

  if(played_notes[2] != 12){phaseAcc2 = sine_lut[sine_lut_start_index[played_notes[2]]+lenCount2];} else {phaseAcc2 = 0; counter2 = 0; lenCount2 = 0;}
  if(counter2 == 1){
    counter2 = 0;
    if(lenCount2 >= sine_lut_sizes[played_notes[2]]){
    lenCount2 = 0;
    } else {
    lenCount2+=1;
  }
  } else {
    counter2++;
  }

  if(played_notes[3] != 12){phaseAcc3 = sine_lut[sine_lut_start_index[played_notes[3]]+lenCount3];} else {phaseAcc3 = 0; counter3 = 0; lenCount3 = 0;}
  if(counter3 == 1){
    counter3 = 0;
    if(lenCount3 >= sine_lut_sizes[played_notes[3]]){
    lenCount3 = 0;
    } else {
    lenCount3+=1;
  }
  } else {
    counter3++;
  }
  
  if(played_notes[1] == 12){
    phaseAccTotal = phaseAcc;
  } else if(played_notes[2] == 12) {
    phaseAccTotal = phaseAcc/2 + phaseAcc1/2;
  } else if(played_notes[3] == 12) {
    phaseAccTotal = phaseAcc/3 + phaseAcc1/3 + phaseAcc2/3;
  } else {
    phaseAccTotal = phaseAcc/4 + phaseAcc1/4 + phaseAcc2/4 + phaseAcc3/4;
  }
  
  //phaseAcc += currentStepSize/4;
  int32_t Vout = phaseAccTotal >> 24;
  Vout = Vout >> (8 - knob2_obj.count/4);
  analogWrite(OUTR_PIN, Vout + 128);
}

void setup() {
  // put your setup code here, to run once:

  //Set pin directions
  pinMode(RA0_PIN, OUTPUT);
  pinMode(RA1_PIN, OUTPUT);
  pinMode(RA2_PIN, OUTPUT);
  pinMode(REN_PIN, OUTPUT);
  pinMode(OUT_PIN, OUTPUT);
  pinMode(OUTL_PIN, OUTPUT);
  pinMode(OUTR_PIN, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(C0_PIN, INPUT);
  pinMode(C1_PIN, INPUT);
  pinMode(C2_PIN, INPUT);
  pinMode(C3_PIN, INPUT);
  pinMode(JOYX_PIN, INPUT);
  pinMode(JOYY_PIN, INPUT);

  //Initialise display
  setOutMuxBit(DRST_BIT, LOW);  //Assert display logic reset
  delayMicroseconds(2);
  setOutMuxBit(DRST_BIT, HIGH);  //Release display logic reset
  u8g2.begin();
  setOutMuxBit(DEN_BIT, HIGH);  //Enable display power supply

  //Initialise UART
  Serial.begin(9600);
  Serial.println("Hello World");

  //Setup timer
  TIM_TypeDef *Instance = TIM1;
  HardwareTimer *sampleTimer = new HardwareTimer(Instance);
  sampleTimer->setOverflow(22000, HERTZ_FORMAT);
  sampleTimer->attachInterrupt(sampleISR);
  sampleTimer->resume();

  // Keyboard Scanning Thread intialisaation
  TaskHandle_t scanKeysHandle = NULL;
  xTaskCreate(
    scanKeysTask, // Function that implements the task
    "scanKeys", // Task name
    64, // Stack size in words
    NULL, // Parameter passed into the task
    3, // Task priority
    &scanKeysHandle // Pointer to store the task handle
  );

  // Decode Thread initialisation
  TaskHandle_t decodeHandle = NULL;
  xTaskCreate(
    decodeTask, // Function that implements the task
    "decode", // Task name
    64, // Stack size in words
    NULL, // Parameter passed into the task
    2, // Task priority
    &decodeHandle // Pointer to store the task handle
  );

  // Display Updating Thread intialisaation
  TaskHandle_t displayUpdateHandle = NULL;
  xTaskCreate(
    displayUpdateTask, // Function that implements the task
    "displayUpdate", // Task name
    64, // Stack size in words
    NULL, // Parameter passed into the task
    1, // Task priority
    &displayUpdateHandle // Pointer to store the task handle
  );

  // Creating Key Array mutex
  keyArrayMutex = xSemaphoreCreateMutex();

  // Start the CAN communication
  CAN_Init(can_init);
  setCANFilter(0x123,0x7ff);
  CAN_RegisterRX_ISR(CAN_RX_ISR);
  CAN_Start();

  // Initialise the receive queue
  msgInQ = xQueueCreate(36, 8);

  // Start the tasks
  vTaskStartScheduler();
}

void loop() {
 // Empty loop
}

