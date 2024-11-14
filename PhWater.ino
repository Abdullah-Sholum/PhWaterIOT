#define BLYNK_TEMPLATE_ID "TMPL6TR7lygb4"
#define BLYNK_TEMPLATE_NAME "AquaSmartIOT"
#define BLYNK_AUTH_TOKEN "GU2i7muowvRlpjzPqHyxjK4mPVdyQuWu"
#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//inisiasi wifi
char ssid[] = "Daviandra";
char pass[] = "12345678899";

// inisiasi pin GPIO
#define ledTest 13  
#define ledWifi 27
#define ledS1 26
#define pinPh 35

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
  
  //set pin buat I/O
  pinMode(ledWifi, OUTPUT);
  pinMode(ledTest, OUTPUT);
  pinMode(ledS1, OUTPUT);
  pinMode(pinPh, INPUT);
  
  //animasi awal LED
  digitalWrite(ledTest, HIGH);
  digitalWrite(ledS1, HIGH);
  delay(1500);
  digitalWrite(ledTest, LOW); delay(500);
  digitalWrite(ledTest, HIGH); delay(200);
  digitalWrite(ledTest, LOW); delay(200);
  digitalWrite(ledTest, HIGH); delay(200);
  digitalWrite(ledTest, LOW); 
  digitalWrite(ledS1, LOW);
  delay(2000);
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
BLYNK_WRITE(V1) {
  int switchState = param.asInt(); // Membaca status dari Virtual Pin V0
  
  if (switchState == 1) {
    digitalWrite(ledS1, HIGH); // Nyalakan LED jika tombol di Blynk ditekan
  } else {
    digitalWrite(ledS1, LOW);  // Matikan LED jika tombol di Blynk dilepas
  }
}

// Fungsi membaca nilai pH dari sensor
void readPh() {
  int sensorValue = analogRead(pinPh);
  float voltage = sensorValue * (3.3 / 4095.0); // Konversi nilai ADC ke tegangan

  Serial.print("Tegangan dari sensor: ");
  Serial.println(voltage);

  // Konversi tegangan ke nilai pH
  float phValue = 7.0 + ((1.65 - voltage) / 0.18);

  // Batasi nilai pH dalam rentang 1 hingga 14
  if (phValue < 1.0) phValue = 1.0;
  if (phValue > 14.0) phValue = 14.0;

  Serial.print("Nilai pH: ");
  Serial.println(phValue);

  // Kirim nilai pH ke Virtual Pin V0 di Blynk
  Blynk.virtualWrite(V0, phValue);
}
