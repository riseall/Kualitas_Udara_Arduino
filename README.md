# Air Quality Monitoring System

Proyek ini adalah sistem pemantauan kualitas udara menggunakan berbagai sensor dan menampilkan hasilnya pada LCD serta mengirimkannya ke NodeMCU. Sistem ini menggunakan sensor MQ-3, MQ-7, MQ-135, DHT22, dan SharpGp2y1010a0uf.

## Daftar Isi

- [Perangkat Keras yang Digunakan](#perangkat-keras-yang-digunakan)
- [Perangkat Lunak yang Digunakan](#perangkat-lunak-yang-digunakan)
- [Instalasi dan Penggunaan](#instalasi-dan-penggunaan)
- [Cara Kerja](#cara-kerja)
- [Kode Utama](#kode-utama)
- [Kontribusi](#kontribusi)
- [Lisensi](#lisensi)

## Perangkat Keras yang Digunakan
1. **Arduino**
2. **LCD 16x2 dengan I2C**
3. **Sensor Gas MQ-3**
4. **Sensor Gas MQ-7**
5. **Sensor Gas MQ-135**
6. **Sensor Debu SharpGp2y1010a0uf**
7. **Sensor Suhu dan Kelembaban DHT22**
8. **NodeMCU**
9. **Komponen Pendukung Lainnya (Resistor, Kabel, Breadboard, dll)**

## Perangkat Lunak yang Digunakan
1. **Arduino IDE**
2. **Library Arduino**:
   - `Wire.h`
   - `LiquidCrystal_I2C.h`
   - `MQUnifiedsensor.h`
   - `DHT.h`
   - `SoftwareSerial.h`

## Instalasi dan Penggunaan

1. **Kloning Repository**:
    ```bash
    git clone https://github.com/username/repo-name.git
    cd repo-name
    ```

2. **Instalasi Library yang Dibutuhkan**:
   - Buka Arduino IDE.
   - Pergi ke `Sketch` > `Include Library` > `Manage Libraries...`.
   - Cari dan instal library berikut:
     - `LiquidCrystal_I2C`
     - `MQUnifiedsensor`
     - `DHT sensor library`
     - `SoftwareSerial`

3. **Upload Kode ke Arduino**:
   - Buka file `.ino` dari repository ini di Arduino IDE.
   - Sambungkan Arduino ke komputer dan pilih port yang sesuai.
   - Klik tombol `Upload`.

## Cara Kerja

- Sistem membaca data dari sensor gas (MQ-3, MQ-7, MQ-135), sensor debu (SharpGp2y1010a0uf), dan sensor suhu & kelembaban (DHT22).
- Data yang diperoleh akan ditampilkan pada LCD.
- Data juga dikirim ke NodeMCU melalui komunikasi serial.

