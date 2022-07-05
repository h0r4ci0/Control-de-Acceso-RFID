/*
   Desarrollador: Horacio

   Github: https://github.com/h0r4ci0

   Correo: horamalean@protonmail.com

   Última modificación: 04/07/2022
*/

// RFID-RC522 Module
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9 // pin de reset
#define SS_PIN 10 // pin de slave select

MFRC522 mfrc522(SS_PIN, RST_PIN); // crea objeto MFRC522 a través de pines de slave select y reset.

// UID con permisos 
byte LecturaUID[4];         // crea array para almacenar el UID leído
byte llavero[4] = {0x4C, 0x64, 0xF1, 0x37} ;    // LLAVERO  -> 4C 64 F1 37
byte card[4] = {0x83, 0xF0, 0xCE, 0xA9} ;    // TARJETA -> 83 F0 CE A9
byte Manolo_phone[1] = {0x08} ; // MANOLO'S PHONE -> 08
byte Manolo_watch[1] = {0x08} ; // MANOLO'S WATCH -> 08
byte Horacio_phone[4] = {0x08} ; // HORACIO'S PHONE -> 08
byte Marina_phone[4] = {0x08} ; // MARINA'S PHONE -> 08
byte Marina_watch[4] = {0x08} ; // MARINA'S WATCH -> 08

// LCD Screen
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int screenWidth = 16;
int screenHeight = 16;

int stringStart, stringStop;
int scrollCursor = screenWidth;

LiquidCrystal_I2C lcd (0x27, 16, 2);

// LEDs
#define LED_pass 7 // led verde
#define LED_close 6 // led rojo

// Cerradura
#define cerradura 4

void setup() {
    Serial.begin(9600);     // Inicializa comunicación por monitor serie a 9600bps
    SPI.begin();           // Incializa el bus SPI
    mfrc522.PCD_Init();    // Inicializa el módulo lector
    Serial.println("LISTO");

    // Turn ON LEDs
    pinMode(LED_pass, OUTPUT);
    pinMode(LED_close, OUTPUT);

    // Cerradura
    pinMode(cerradura, OUTPUT);
    digitalWrite(cerradura, HIGH);

    // LCD Screen
    lcd.init();
    lcd.setCursor(0,0);
    lcd.backlight();

    // Mensaje que aparece al iniciar la Arduino en la pantalla LCD
    lcd.print("Iniciando");
    delay(500);

    lcd.clear();
    lcd.print("Iniciando.");
    delay(500);

    lcd.clear();
    lcd.print("Iniciando..");
    delay(500);

    lcd.clear();
    lcd.print("Iniciando...");
    delay(1000);

    lcd.clear();
    lcd.print("Listo!");

    delay(2500);

    // Turn OFF LEDs
    digitalWrite(LED_pass, HIGH);
    digitalWrite(LED_close, HIGH);
}

void loop() {

    // RFID-RC522 Module
    if (! mfrc522.PICC_IsNewCardPresent()) { // si no hay una tarjeta persente
        lcd.noBacklight();
        lcd.clear();
        return;                             // vuelve a ejecutar el loop en esperea de una tarjeta
    }

    if (! mfrc522.PICC_ReadCardSerial()) {    // si no puede obtener datos de la tarjeta
        return;
    }

    Serial.print("UID:");    // muestra el UID de la tarjeta en el monitor serie

    for (byte i = 0; i < mfrc522.uid.size; i++) { // el bucle recorre cada byte del UID

        if (mfrc522.uid.uidByte[i] < 0x10) { // si el byte leído es menor a 0x10
            Serial.print(" 0");
        }

        else {
            Serial.print(" ");
        }

        Serial.print(mfrc522.uid.uidByte[i], HEX); // imprime el byte del UID leído en hexadecimal
        LecturaUID[i] = mfrc522.uid.uidByte[i]; // almacena en array el byte leído del UID
    }

    Serial.println("\t");   // imprime un "enter"

    // LLAVERO
    if (comparaUID(LecturaUID, llavero)){

        digitalWrite(LED_pass, LOW);    // enciende el LED verde

        digitalWrite(cerradura, HIGH);  // desbloquea la cerradura

        lcd.clear();
        lcd.backlight();
        lcd.setCursor(0,0);
        lcd.print("Bienvenido Key!");
        delay(4000);

        return;
    }

    // CARD
    if (comparaUID(LecturaUID, card)){

        digitalWrite(LED_pass, LOW);    // enciende el LED verde

        digitalWrite(cerradura, HIGH);  // desbloquea la cerradura

        lcd.clear();
        lcd.backlight();
        lcd.setCursor(0,0);
        lcd.print("Bienvenido Card!");
        delay(4000);
    }
    
    // WITHOUT ACCESS
    else {

        digitalWrite(LED_close, LOW);   // enciende el LED rojo

        lcd.clear();
        lcd.backlight();
        lcd.setCursor(0,0);
        lcd.print("YOU MUST BE ROOT!");
        delay(3000);

        lcd.clear();
        lcd.print("ACCESS DENIED");
        delay(5000);
        digitalWrite(LED_close, HIGH);

        mfrc522.PICC_HaltA();   // detener comunicación con la tarjeta
    }
}

// Extrae y compara el UID de las tarjetas.

boolean comparaUID(byte lectura[], byte usuario[]) {  // función compara UID
    for (byte i; i < mfrc522.uid.size; i++) { // bucle que recorre de un byte en un byte por vez el UID
        if (lectura[i] != usuario[i])   // si el byte de UID leído no corresponde con ningún usuario
            return(false);      // devuelve "falso"
    }

    return(true);   // si los 4 byte corresponde con el de un usuario, devuelve "verdadero"
}