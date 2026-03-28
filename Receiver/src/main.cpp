#include <Arduino.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>

#define AES_BLOCK_SIZE 16
unsigned char key[32] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31, 0x32
};

void print_hex(const char* label, unsigned char* data, int len) {
    Serial.print(label);
    for (int i = 0; i < len; i++) Serial.printf("%02X ", data[i]);
    Serial.println();
}

void process_secure_packet() {
    if (Serial.read() == 0xAA && Serial.read() == 0xBB) { // Header Kontrolü
        unsigned char iv[16];
        unsigned char hash_received[32];
        unsigned char hash_calculated[32];
        
        // 1. IV'yi Oku
        Serial.readBytes(iv, 16);

        // 2. Uzunluğu Oku (2 byte)
        uint16_t len = (Serial.read() << 8) | Serial.read();
        unsigned char ciphertext[len];
        Serial.readBytes(ciphertext, len);

        // 3. SHA-256 Hash'i Oku
        Serial.readBytes(hash_received, 32);

        // 4. Bütünlük Kontrolü (Integrity Check)
        mbedtls_sha256(ciphertext, len, hash_calculated, 0);
        if (memcmp(hash_received, hash_calculated, 32) != 0) {
            Serial.println("!!! GÜVENLİK UYARISI: VERİ BÜTÜNLÜĞÜ BOZULMUŞ !!!");
            return;
        }

        // 5. AES Decryption (Şifre Çözme)
        mbedtls_aes_context aes;
        unsigned char decrypted_output[len + 1];
        
        mbedtls_aes_init(&aes);
        mbedtls_aes_setkey_dec(&aes, key, 256); // Çözme için setkey_dec!
        
        int ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, len, iv, ciphertext, decrypted_output);
        
        if (ret == 0) {
            decrypted_output[len] = '\0'; // String sonlandırıcı
            Serial.println("\n--- YENİ GÜVENLİ MESAJ ALINDI ---");
            Serial.printf("Çözülen Mesaj: %s\n", decrypted_output);
            print_hex("Kullanılan IV: ", iv, 16);
        } else {
            Serial.printf("Hata: Şifre çözülemedi! Kod: %d\n", ret);
        }
        mbedtls_aes_free(&aes);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Receiver Hazır, Veri Bekleniyor...");
}

void loop() {
    if (Serial.available() > 50) { // Minimum paket boyutu gelene kadar bekle
        process_secure_packet();
    }
}