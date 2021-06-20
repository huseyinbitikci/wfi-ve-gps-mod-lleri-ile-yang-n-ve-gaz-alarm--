#include <dht11.h> //DHT11 Sıcaklık Sensörü Kütüphanesi
#include <SoftwareSerial.h> //SoftwareSerial kütüphanimizi ekliyoruz.
SoftwareSerial gsm(8,9); //rx tx
SoftwareSerial esp(10,11);//Seri haberleşme pin ayarlarını yapıyoruz. 
dht11 DHT11;

String agAdi = "esp";                 //Ağımızın adını buraya yazıyoruz.    
String agSifresi = "12345678";           //Ağımızın şifresini buraya yazıyoruz.
String smsMetni = "";

int dhtPin = 6;

String ip = "184.106.153.149";//Thingspeak ip adresi

int buzzer=2; // Buzzer Sinyal Pini
int AOUTpin=A0; // Duman Sensörü Analog Pini

 
// RGB Led
int kirmizi=12; // RGB Led Kırmızı Işık Pini
int yesil=13; // RGB Led Yeşil Işık Pini
 
int sicaklik; // Sıcaklık Değişkeni

int value; // Duman Sensörü Alanlog Değişkeni


void SMSgonder(String mesaj) {

  gsm.begin(9600);
 // gsm.print("AT+CMGF=1");
  delay(100);
  gsm.println("AT+CMGS=\"+905444444444\"");// telefon numarasi degistir
  delay(100);
  gsm.println(mesaj);
  delay(100);
  gsm.println((char)26);
  delay(100);
  gsm.println();
  delay(100);
  gsm.println("AT+CMGD=1,4");
  delay(100);
  gsm.println("AT+CMGF=1");
  delay(100);
  gsm.println("AT+CNMI=1,2,0,0,0");
  delay(200);
  smsMetni = "";
}


void setup()
{
Serial.begin(9600); // Seri Bağlantı Hızı

pinMode(kirmizi, OUTPUT); // RGB Led Kırmızı Işık Çıkışı
pinMode(yesil, OUTPUT); // RGB Led Yeşil Işık Çıkışı
pinMode(buzzer, OUTPUT); // Buzzer Çıkışı
beep(50);
beep(50);
beep(50);
Serial.println("Started");
esp.begin(115200);  //ESP8266 ile seri haberleşmeyi başlatıyoruz.
esp.println("AT");  //AT komutu ile modül kontrolünü yapıyoruz.
  Serial.println("AT Yollandı");
  while(!esp.find("OK")){    //Modül hazır olana kadar bekliyoruz.
    esp.println("AT");
    Serial.println("ESP8266 Bulunamadı.");
  }
  Serial.println("OK Komutu Alındı");
  esp.println("AT+CWMODE=1"); //ESP8266 modülünü client olarak ayarlıyoruz.
  while(!esp.find("OK")){     //Ayar yapılana kadar bekliyoruz.
    esp.println("AT+CWMODE=1");
    Serial.println("Ayar Yapılıyor....");
  }
  Serial.println("Client olarak ayarlandı");
  Serial.println("Aga Baglaniliyor...");
  esp.println("AT+CWJAP=\""+agAdi+"\",\""+agSifresi+"\"");//Ağımıza bağlanıyoruz.
  while(!esp.find("OK"));//Ağa bağlanana kadar bekliyoruz.
  Serial.println("Aga Baglandi.");
  delay(1000);
}
 
void loop(){

esp.println("AT+CIPSTART=\"TCP\",\""+ip+"\",80");//Thingspeak'e bağlanıyoruz.
  if(esp.find("Error")){                           //Bağlantı hatası kontrolü yapıyoruz.
    Serial.println("AT+CIPSTART Error");
  }
  delay(2000);
  DHT11.read(dhtPin);
  sicaklik = (int)DHT11.temperature; // Sıcaklık Değeri Okunuyor
  value= (int)analogRead(AOUTpin); // Duman Sensörü Analog Değeri Okunuyor
  delay(1000);
  String veri = "GET https://api.thingspeak.com/update?api_key=7A2FPDQ8TYO0TEO4";   //Thingspeak komutu. Key kısmına kendi api keyimizi yazıyoruz.
  veri += "&field1=";
  veri += String(sicaklik);                                     //Göndereceğimiz sıcaklık değişkeni
  veri += "&field2=";
  veri += String(value);                                        //Göndereceğimiz gaz değişkeni
  veri += "\r\n\r\n"; 
  esp.print("AT+CIPSEND=");                                   //ESP'ye göndereceğimiz veri uzunluğunu veriyoruz.
  esp.println(veri.length()+2);
  delay(2000);
  if(esp.find(">")){                                          //ESP8266 hazır olduğunda içindeki komutlar çalışıyor.
    esp.print(veri);                                          //Veriyi gönderiyoruz.
    Serial.println(veri);
    Serial.println("Veri gonderildi.");
    delay(1000);
  }
  Serial.println("Baglantı Kapatildi.");
  esp.println("AT+CIPCLOSE");                                //Bağlantıyı kapatıyoruz
  delay(1000);                                               //Yeni veri gönderimi için 1 dakika bekliyoruz.
delay(300);
if (value > 330){
beep(200);
//Kullanılan RGB Led Anot (Pozitif) Uçlu Olduğudan LOW Değeri Ledi Yakar
digitalWrite(kirmizi, LOW); // Kırmızı Işık Açık
digitalWrite(yesil, HIGH); // Yeşil Işık Kapalı
    gsm.begin(9600);
    delay(100);
    gsm.println("AT+CMGF=1");
    delay(100);
    gsm.println("AT+CNMI=1,2,0,0,0");
    gsm.println("AT+CMGD=1,4");
    SMSgonder("sms içeriği");
    gsm.flush();
}
 
else if (DHT11.temperature > 26){
beep(200);
digitalWrite(kirmizi, LOW); // Kırmızı Işık Açık
digitalWrite(yesil, HIGH); // Yeşil Işık Kapalı
    gsm.begin(9600);
    delay(100);
    gsm.println("AT+CMGF=1");
    delay(100);
    gsm.println("AT+CNMI=1,2,0,0,0");
    gsm.println("AT+CMGD=1,4");
    SMSgonder("Motor Calistirildi");
    gsm.flush();
 
}
 
else {
digitalWrite(kirmizi, HIGH); // Kırmızı Işık Kapalı
digitalWrite(yesil, LOW); // Yeşil Işık Açık
}
 
}
// Buzzer Alarm Fonksiyonu
void beep(unsigned char delayms){
digitalWrite(buzzer, HIGH); // Buzzer Ses Verir
delay(delayms); // Belirlilen MS cinsinden bekletme
digitalWrite(buzzer, LOW); // Buzzer Sesi Kapatır
delay(delayms); // Belirlilen MS cinsinden bekletme
}