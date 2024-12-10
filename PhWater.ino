#define BLYNK_TEMPLATE_ID "TMPL6TR7lygb4"
#define BLYNK_TEMPLATE_NAME "AquaSmartIOT"
#define BLYNK_AUTH_TOKEN "GU2i7muowvRlpjzPqHyxjK4mPVdyQuWu"
#define BLYNK_PRINT Serial

//masukkan pustaka wifi, blynk & Adafruit_GFX
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//inisiasi wifi
//char ssid[] = "wifiPunten";
//char pass[] = "1234567890";

char ssid[] = "AquasmartIoT";
char pass[] = "aqua1234";

// inisiasi pin GPIO
#define ledTest 13  
#define ledWifi 27
#define ledS1 26
#define pinPh 35
#define pinBuzzer 32
#define pinResetBuz 18

//inisiasi layar
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);


//inisiasi variabel buat led
unsigned long previousMillis = 0;         // Variabel untuk menyimpan waktu sebelumnya
const long intervalDisconnected = 90;     // Interval berkedip cepat (200 ms) ketika WiFi tidak tersambung
unsigned long previousMillisTest = 0;     // Variabel untuk menyimpan waktu sebelumnya LED test
const long intervalTest = 1000;           // Interval berkedip 1 detik untuk LED test
bool wifiConnected = false;               // Status koneksi WiFi
BlynkTimer timer;                         // Timer untuk interval pembacaan pH

void setup() {
  //mulai serial monitor 
  Serial.begin(115200); 

  //konek wifi
  WiFi.begin(ssid, pass);   

  //coba sambung blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Blynk.connect();
  
  // Memanggil fungsi readPhWater setiap 2 detik
  timer.setInterval(2000L, readPh); 

  analogReadResolution(12); // Mengatur resolusi ADC ESP32 ke 12-bit

  //inisiasi layar oled
  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Failed to initialize OLED");
    while (true);
  }

  Serial.println("OLED begun");

  // Clear the buffer and show initial display.
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("System Ready");
  display.display();
  delay(2000);

  
  //set pin buat I/O
  pinMode(ledWifi, OUTPUT);
  pinMode(ledTest, OUTPUT);
  pinMode(ledS1, OUTPUT);
  pinMode(pinPh, INPUT);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinResetBuz, INPUT_PULLUP);
  
  //animasi awal OUPUT
  digitalWrite(pinBuzzer, HIGH);
  delay(200);
  digitalWrite(pinBuzzer, LOW);
  delay(200);
  digitalWrite(pinBuzzer, HIGH);
  delay(200);
  digitalWrite(pinBuzzer, LOW);
  delay(200);
  digitalWrite(ledTest, HIGH);
  delay(200);
  digitalWrite(ledTest, LOW); 
  delay(200);
  digitalWrite(ledWifi, HIGH); 
  delay(200);
  digitalWrite(ledWifi, LOW); 
  delay(200);
  digitalWrite(ledS1, HIGH); 
  delay(200);
  digitalWrite(ledS1, LOW); 
  delay(200);
}

void loop() {
  //jalankan fungsi
  Blynk.run();
  timer.run();
  wifiIndicator();
  ledTestBlink();
}

//fungsi cek koneksi wifi
void wifiIndicator() {
  if (WiFi.status() != WL_CONNECTED) {        // Jika tidak terhubung ke WiFi
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= intervalDisconnected) {
      previousMillis = currentMillis;         // Simpan waktu sekarang

      // Toggle LED untuk berkedip
      digitalWrite(ledWifi, !digitalRead(ledWifi));
    }
    wifiConnected = false; // Status WiFi tidak terhubung
    Serial.println("Menghubungkan ke WiFi...");
  } else if (!wifiConnected) {            // Jika terhubung ke WiFi pertama kali
    digitalWrite(ledWifi, HIGH);          // LED tetap menyala
    wifiConnected = true;                 // Ubah status menjadi terhubung
    Serial.println("WiFi Terhubung!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

//fungsi led Test
void ledTestBlink() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisTest >= intervalTest) {
    previousMillisTest = currentMillis;  // Simpan waktu sekarang

    // Toggle LED untuk berkedip 1 detik
    digitalWrite(ledTest, !digitalRead(ledTest));
  }
}

//fungsi cek koneksi dengan led
//BLYNK_WRITE(V1) {
//  int switchState = param.asInt(); // Membaca status dari Virtual Pin V0
//  
//  if (switchState == 1) {
//    digitalWrite(ledS1, HIGH); // Nyalakan LED jika tombol di Blynk ditekan
//  } else {
//    digitalWrite(ledS1, LOW);  // Matikan LED jika tombol di Blynk dilepas
//  }
//}








//===================================================================================================================
// Fungsi membaca nilai pH dari sensor
bool buzzerMuted = false; // Flag untuk mematikan buzzer secara permanen setelah di-reset
void readPh() {
  int sensorValue = analogRead(pinPh);
  float voltage = sensorValue * (3.3 / 4095.0); // Konversi nilai ADC ke tegangan

  Serial.print("Tegangan dari sensor: ");
  Serial.println(voltage);

  // Konversi tegangan ke nilai pH dengan interpolasi
  float phValue;
  if (voltage >= 2.01) {
    phValue = 7 - ((voltage - 2.01) / 0.1905); // Rentang pH 7 ke pH 3.8
  } else {
    phValue = 7 - ((voltage - 2.01) / 0.0459); // Rentang pH 7 ke pH 11.5
  }

  // Batasi nilai pH dalam rentang 1 hingga 14
  if (phValue < 1.0) phValue = 1.0;
  if (phValue > 14.0) phValue = 14.0;

  Serial.print("Nilai pH: ");
  Serial.println(phValue);

  // Kirim nilai pH ke Virtual Pin V0 di Blynk
  Blynk.virtualWrite(V0, phValue);

  // Kontrol LED indikasi berbahaya berdasarkan nilai pH, terpisah dari kontrol buzzer
  if (phValue < 5.6 || phValue > 8.7) {
    digitalWrite(ledS1, HIGH); // LED menyala jika pH berbahaya
  } else {
    digitalWrite(ledS1, LOW); // LED mati jika pH normal
  }

  // Kontrol buzzer terpisah dari LED, cek kondisi pH berbahaya dan flag buzzerMuted
  if ((phValue < 5.6 || phValue > 8.7) && !buzzerMuted) {
    // Nyalakan buzzer selama 5 detik atau hingga tombol reset ditekan, atau pH kembali normal
    unsigned long startMillis = millis();
    bool buzzerActive = true;

    while (millis() - startMillis < 5000 && buzzerActive) {
      digitalWrite(pinBuzzer, HIGH); // Nyalakan buzzer
      
      // Cek apakah tombol reset ditekan
      if (digitalRead(pinResetBuz) == LOW) {
        Serial.println("Buzzer dimatikan oleh tombol reset.");
        digitalWrite(pinBuzzer, LOW); // Matikan buzzer
        buzzerMuted = true; // Atur flag untuk mematikan buzzer secara permanen
        buzzerActive = false; // Keluar dari loop
        break;
      }

      // Cek apakah pH sudah kembali normal
      if (phValue >= 5.6 && phValue <= 8.7) {
        Serial.println("pH kembali normal, buzzer dimatikan.");
        digitalWrite(pinBuzzer, LOW); // Matikan buzzer
        buzzerActive = false; // Keluar dari loop
        break;
      }

      // Refresh nilai pH untuk mengecek perubahan dalam loop
      sensorValue = analogRead(pinPh);
      voltage = sensorValue * (3.3 / 4095.0);
      phValue = 7.0 + ((1.65 - voltage) / 0.18);
    }

    // Matikan buzzer setelah 5 detik atau jika kondisi normal/reset terjadi
    digitalWrite(pinBuzzer, LOW);

  } else {
    // Jika kondisi pH normal atau buzzer telah di-reset, buzzer tetap mati
    digitalWrite(pinBuzzer, LOW);
  }
    //kirim ke layar    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("PH Air");
    display.setCursor(0, 16); // Posisikan di bawah tulisan "PH Air"
    display.setTextSize(2);  // Perbesar ukuran teks
    display.print(phValue, 2); // Tampilkan nilai pH dengan 2 desimal
    display.display();
}
