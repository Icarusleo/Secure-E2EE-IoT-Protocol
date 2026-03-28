#include <Arduino.h>
#include <mbedtls/aes.h>
#include <mbedtls/sha256.h>

#define MAX_MSG_LEN 64
#define AES_BLOCK_SIZE 16

unsigned char key[32] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0x20, 0x21, 0x22, 0x23, 0x24,
    0x25, 0x26, 0x27, 0x28, 0x29, 0x30, 0x31, 0x32
};

void print_hex(const char* label, unsigned char* data, int len) {
    Serial.print(label);
    for (int i = 0; i < len; i++) {
        Serial.printf("%02X ", data[i]);
    }
    Serial.println();
}

int apply_padding(unsigned char* input, int input_len, int target_len) {
    uint8_t padding_val = target_len - input_len;
    for (int i = input_len; i < target_len; i++) {
        input[i] = padding_val;
    }
    return target_len;
}

void send_secure_packet(const char * plaintext) {
    mbedtls_aes_context aes;
    mbedtls_sha256_context sha;
    
    int original_len = strlen(plaintext);
    // PKCS#7 kuralı: Veri blok boyutunun katı olsa bile mutlaka yeni blok eklenir
    int padded_len = ((original_len / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;

    unsigned char iv[AES_BLOCK_SIZE];
    unsigned char iv_for_mbedtls[AES_BLOCK_SIZE]; // Kritik kopya!
    unsigned char padded_input[MAX_MSG_LEN];
    unsigned char cipher_text[MAX_MSG_LEN];
    unsigned char hash[32];

    // 1. Gerçek Rastgele IV Üret
    for(int i=0; i<16; i++) {
        iv[i] = (uint8_t)esp_random();
    }
    // IV'nin kopyasını şifreleme fonksiyonuna vereceğiz
    memcpy(iv_for_mbedtls, iv, 16);

    // 2. Padding Hazırla
    memset(padded_input, 0, MAX_MSG_LEN);
    memcpy(padded_input, plaintext, original_len);
    apply_padding(padded_input, original_len, padded_len);

    // 3. AES Şifreleme
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 256);
    
    int ret = mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, padded_len, iv_for_mbedtls, padded_input, cipher_text);
    
    if(ret != 0) {
        Serial.printf("HATA: AES Basarisiz! Kod: %d\n", ret);
        mbedtls_aes_free(&aes);
        return;
    }

    // 4. SHA-256 (Şifreli verinin özeti)
    mbedtls_sha256_init(&sha);
    mbedtls_sha256_starts(&sha, 0);
    mbedtls_sha256_update(&sha, cipher_text, padded_len);
    mbedtls_sha256_finish(&sha, hash);

    // 5. Paketi UART'tan Gönder (Binary format)
    Serial.write(0xAA); Serial.write(0xBB); // Header
    Serial.write(iv, 16);                   // Orijinal IV
    Serial.write((uint8_t)(padded_len >> 8)); 
    Serial.write((uint8_t)(padded_len & 0xFF));
    Serial.write(cipher_text, padded_len);
    Serial.write(hash, 32);

    // Debug Çıktıları (İnsan okuması için)
    Serial.println("\n--- Paket Gonderildi ---");
    Serial.println("Mesaj: " + String(plaintext));
    print_hex("IV:      ", iv, 16);
    print_hex("Cipher:  ", cipher_text, padded_len);
    print_hex("SHA-256: ", hash, 32);

    mbedtls_aes_free(&aes);
    mbedtls_sha256_free(&sha);
    memset(padded_input, 0, MAX_MSG_LEN); // Güvenlik için temizle
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    Serial.println("Sistem Hazir...");
}

void loop() {
    send_secure_packet("DUNE_IS_AMAZING");
    delay(5000);
}