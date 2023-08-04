const byte LED = 2;
const byte BUTTON1 = 3;
const byte BUTTON2 = 4;
bool ledState = false;
bool button1State = false;
bool button2State = false;
bool lastButton1State = false;
bool lastButton2State = false;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
}

void loop() {
  button1State = digitalRead(BUTTON1);
  button2State = digitalRead(BUTTON2);

  // Botão1 de ligar o Led
  if (button1State == LOW && lastButton1State == HIGH) {
    ledState = true;
  }

  // BBotão2 de desligar LED
  if (button2State == LOW && lastButton2State == HIGH) {
    ledState = false;
  }

  digitalWrite(LED, ledState);

  lastButton1State = button1State;
  lastButton2State = button2State;
  
}
