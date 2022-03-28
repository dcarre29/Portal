String codigomando;
String mandoant;
String mandoa =  String(0) + String(0) + String(0) + String(1);
String mandob = String(0) + String(0) + String(1) + String(0);
byte pin_obstaculo = A0;
byte pin_abrir = 6;
byte pin_pm = 5;
byte pin_fc = 3;
byte pin_llave = 2;
byte pin_laser = 4;
bool laser;
bool fc = false;
bool automatico = false;
bool a = false;
bool b = false;
bool fcant = true;
bool finalcarrera;
bool llave;
bool llaveant = false ;
unsigned long tiempo = 20000; //tiempo de espera para volver a cerrar el portal
unsigned long tportal = 0;
unsigned long tportalant = 0;
int estado = 0;

float voltaje = 0.0;
float sensibilidad = 0.078; //sensibilidad del sensor efecto HALL -----no se toca -----
float intensidad = 0; //Intensidad que circula por el motor

void setup() {
  // put your setup code here, to run once:
  pinMode( pin_fc , INPUT); // fc
  pinMode( pin_laser, INPUT ); //laser
  pinMode(13, INPUT ); // D0
  pinMode(12, INPUT); //D1
  pinMode(11, INPUT); //D2
  pinMode(10, INPUT); //D3
  pinMode(9, INPUT); //vt
  pinMode(pin_llave, INPUT);
  pinMode(pin_obstaculo, INPUT); //obstaculo
  pinMode(pin_abrir , OUTPUT); //abrir con low
  pinMode(pin_pm , OUTPUT); // energia motor
  digitalWrite(pin_abrir, HIGH);
  digitalWrite(pin_pm, HIGH);
  Serial.begin(9600);
}


void medir() {
  voltaje = analogRead(pin_obstaculo) * 5.0 / 1023;
  intensidad = (voltaje - 2.5) / sensibilidad;

}


void loop() {
  // put your main code here, to run repeatedly:

  codigomando = String(digitalRead(13), BIN) + String (digitalRead(12), BIN) + String (digitalRead(11), BIN) + String (digitalRead(10), BIN);
  llave = digitalRead(pin_llave);
  if ( mandoant != codigomando || llaveant != llave) { //deteccion de flanco de boton a o b
    if (mandoa == codigomando) {
      a = true;
    } else if (mandob == codigomando || llave == true) {
      b = true;
    } else {
      a = false;
      b = false;
    }
    mandoant = codigomando;
    llaveant = llave;
  }
  finalcarrera = digitalRead(pin_fc);
  if (fcant == false && finalcarrera == HIGH) { //deteccion de flanco del final de carrera
    fc = true;
    Serial.println("Flanco positivo");
  } else {
    fc = false;
  }
  fcant = finalcarrera;

  medir();
  laser = digitalRead(pin_laser);

  if (automatico == false) {
    tportalant = millis();
  }


  switch (estado) {
    case 0:
      Serial.println("estado = 0");
      if (b == true) {
        estado = 1;

      }
      if (a == true) {
        estado = 1;
        automatico = true;
      }
      break;

    case 1:
      Serial.println("estado = 1");
      if ( intensidad > 6000 || fc == true || b == true ) {
        estado = 2;
      }
      break;
    case 2:
      Serial.println("estado = 2");

      if (b == true)  {
        estado = 3;

      }

      if (automatico == true && (millis() - tportalant) >= tiempo) {
        estado = 3;
        tportalant = millis();
        automatico = false;
      }

      break;
    case 3:
      Serial.println("estado = 3");
      if (intensidad > 6000 || fc == true || b == true || laser == HIGH) {
        estado = 0;
      }
      break;

  }
  a = false;
  b = false;
  fc = false;

  if (estado == 0 || estado == 2) {
    digitalWrite(pin_pm, HIGH);
    delay(500);
    digitalWrite(pin_abrir, HIGH);
  }
  if (estado == 1) {
    //abrir
    
    digitalWrite(pin_abrir, LOW);
    delay(1000);
    digitalWrite(pin_pm, LOW);
  }
  if (estado == 3) {
    //cerrando
    
    digitalWrite(pin_abrir, HIGH);
    delay(1000);
    digitalWrite(pin_pm, LOW);
  }
  Serial.print("codigomando ");
  Serial.print(codigomando);
  Serial.print(" ");
  Serial.print("laser ");
  Serial.print(laser);
  Serial.print(" ");
  Serial.print("key ");
  Serial.print(llave);
  Serial.print(" ");
  Serial.print("fc ");
  Serial.print(finalcarrera);
  Serial.print(" ");
  Serial.print("intensidad ");
  Serial.print(intensidad);
  Serial.println(" ");


}
