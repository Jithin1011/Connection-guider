  #define _comPin 12 // Common pin for Turn on Leds  NB: COM PIN MUST BE SELECTED AFTER   :  STARTPIN + MAXPIN
  #define _debugMode 0 // Chamge to 1 for activate (no need)
  #define _maxPin 10 // change to total number og LEDs(30)
  #define _startPin  2 // change to start pin (2)  if start pin is 2 then pins 2,3,4,5,6,........31 will be selected. 
  #define _pushButtonPin 13 // change to push button pin number.

  /*  NO NEED TO CHANGE ANYTHING BELOW */
  
  int _LEDPinArray[_maxPin];
  int _onCount = 0;
  int _onCountFlgArray[_maxPin];
  int _pinStsArray[_maxPin];
  int _pushBtnState = 0;
  int _startUpFlg = 0;


void setup() {
  Serial.begin(9600);

  /* INITIAL STATE  */
  int _temppin = _startPin;
  for(int i=0;i<_maxPin;i++){
    _LEDPinArray[i] = _temppin;
    _temppin++; 
  }

  pinMode(_pushButtonPin,INPUT);

  __initialState();

  if(_debugMode == 1){

    Serial.print("\n******** INITIAL STATE **********");
    for(int i=0;i<_maxPin;i++){
      Serial.print("\nLED_PIN_ARRAY ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(_LEDPinArray[i]);

    }  
    for(int i=0;i<_maxPin;i++){
      Serial.print("\nON_COUNT_FLAG_ARRAY: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(_onCountFlgArray[i]); 
    }

    Serial.print("\nCOUNT : ");
    Serial.print(_onCount);
  }



}

void loop() {

   if(_startUpFlg == 0){
      /*   START UP LED BLINK  */
  delay(2000);
  for(int i=0;i<_maxPin;i++){
    pinMode(_LEDPinArray[i],OUTPUT);
  }
  for(int i=0;i<_maxPin;i++){
    digitalWrite(_LEDPinArray[i],HIGH);
  }
  delay(5000);
  for(int i=0;i<_maxPin;i++){
    digitalWrite(_LEDPinArray[i],LOW);
  }
  delay(2000);


  /*  STARTING UP */
  pinMode(_comPin,OUTPUT);
  digitalWrite(_comPin,HIGH);   // COM PIN AT 3.3VDC
  __loopInitialState();
    
   }

   

   for(int i=0;i<_maxPin;i++){

    _pinStsArray[i] = digitalRead(_LEDPinArray[i]);
    delay(100);
    _pinStsArray[i] = digitalRead(_LEDPinArray[i]);

    if(_pinStsArray[i] == 1 && _onCountFlgArray[i] == 0){

      pinMode(_LEDPinArray[i],OUTPUT);
      digitalWrite(_LEDPinArray[i],HIGH);

      _onCount++;
      _onCountFlgArray[i] = 1; 
    }

    if(_debugMode == 1){

    Serial.print("\n******** CURRENT STATE **********");
    for(int i=0;i<_maxPin;i++){
      Serial.print("\nPIN STS ARRAY: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(_pinStsArray[i]);

     }  
    for(int i=0;i<_maxPin;i++){
      Serial.print("\nON_COUNT_FLAG_ARRAY: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(_onCountFlgArray[i]);
     }

    Serial.print("\nCOUNT : ");
    Serial.print(_onCount);
    Serial.print("\nSTARTUP_FLAG_STATE: ");
    Serial.print(_startUpFlg);
    }  
   }

   _startUpFlg = 1;

   if(_onCount == _maxPin){

    if(_debugMode == 1){
      Serial.print("\nMAX COUNT REACHED. RESETTING OPERATION STARTED ........");
      Serial.print("\nCount : ");
      Serial.print(_onCount);
    }

      delay(20000);

      /* RESETTING LED */
      for(int i=0;i<_maxPin;i++){
        digitalWrite(_LEDPinArray[i],LOW);
      }

      delay(2000);

      LAB1:

      _pushBtnState = digitalRead(_pushButtonPin);
      delay(1000);
      _pushBtnState = digitalRead(_pushButtonPin);

      if(_debugMode == 1){
        Serial.print("\nPUSH_BUTTON_STATE: ");
        Serial.print(_pushBtnState);
      }

      if(_pushBtnState == 1){
        if(_debugMode == 1){
        Serial.print("\nLED TOOGLING STARTED");
      }
        /* LED TOOGLE */
        for(int i=0;i<_maxPin;i++){
           digitalWrite(_LEDPinArray[i],HIGH);
           delay(200);
           digitalWrite(_LEDPinArray[i],LOW);
           delay(200);
        }
        
      }
      else{
        goto LAB1;
      }

      

      /* CHANGE TO INITIAL STATE */
      __initialState();

      if(_debugMode == 1){
    Serial.print("\n******** RESET STATE **********");
    for(int i=0;i<_maxPin;i++){
      Serial.print("\nPIN STS ARRAY: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(_pinStsArray[i]);
     }  
    for(int i=0;i<_maxPin;i++){
      Serial.print("\nON_COUNT_FLAG_ARRAY: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(_onCountFlgArray[i]);
     }

     for(int i=0;i<_maxPin;i++){
      Serial.print("\nLED_PIN_ARRAY: ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.print(_LEDPinArray[i]);
    } 

    Serial.print("\nCOUNT : ");
    Serial.print(_onCount);
    Serial.print("\nPUSH_BUTTON_STATE: ");
    Serial.print(_pushBtnState);
    Serial.print("\nSTARTUP_FLAG_STATE: ");
    Serial.print(_startUpFlg);
    Serial.print("\n******** END OF PROGRAM LOOP **********");
    } 
   }
}


/* Initial State */
void __initialState(){
  for(int i=0;i<_maxPin;i++){
    _onCountFlgArray[i] = 0;
    _pinStsArray[i] = 0;
  }
  _onCount = 0; 
  _pushBtnState = 0;
  _startUpFlg = 0;
}


/* LOOP Initial State */
void __loopInitialState(){
  for(int i=0;i<_maxPin;i++){
    pinMode(_LEDPinArray[i],INPUT);
  }
}
