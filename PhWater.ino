// Definisikan pin
#define ledTest 13    // Pin LED pada GPIO 13
#define pinPh 14      // Pin pH pada GPIO 14

unsigned long previousMillis = 0;  // Variabel untuk menyimpan waktu sebelumnya
const long interval = 1000;        // Interval LED berkedip (1 detik)

// Fungsi untuk membaca nilai dari sensor pH
void phAir() {
  int phValue = analogRead(pinPh);   // Membaca nilai analog dari sensor pH
  float voltage = phValue * (3.3 / 4095.0);  // Konversi ke tegangan (untuk ESP32 dengan ADC 12-bit)
  float ph = 3.5 * voltage;  // Konversi tegangan ke nilai pH (menyesuaikan kalibrasi sensor Anda)
  Serial.print("ph air: ");
  Serial.println(ph);        // Tampilkan nilai pH ke Serial Monitor
}

void setup() {
  // Mulai komunikasi Serial
  Serial.begin(115200);
  
  // Atur pin LED sebagai output
  pinMode(ledTest, OUTPUT);
  
  // Atur pin pH sebagai input
  pinMode(pinPh, INPUT);
}

void loop() {
  // Baca nilai pH dan tampilkan ke Serial Monitor
  phAir();

  // Mengelola LED untuk berkedip dengan interval 1000 ms
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;   // Reset waktu sebelumnya

    // Toggle LED
    digitalWrite(ledTest, !digitalRead(ledTest));
  }

  delay(1000);  // Baca pH setiap 1 detik
}
