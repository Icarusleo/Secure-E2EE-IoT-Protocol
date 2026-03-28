# Secure-E2EE-IoT-Protocol
### End-to-End Encrypted (E2EE) Communication for ESP32 Nodes

![PlatformIO](https://img.shields.io/badge/PlatformIO-Orange?style=flat-square&logo=platformio)
![C++](https://img.shields.io/badge/Language-C++-blue?style=flat-square&logo=c%2B%2B)
![Security](https://img.shields.io/badge/Security-AES--256--CBC-red?style=flat-square)
![Simulation](https://img.shields.io/badge/Simulation-Wokwi-green?style=flat-square)

## 📌 Project Overview
This project demonstrates a high-security communication layer between two **ESP32 microcontrollers** using a custom **End-to-End Encrypted (E2EE)** protocol. It is designed to ensure data **confidentiality**, **integrity**, and **authenticity** over insecure serial (UART) channels, mimicking real-world secure IoT deployments.

The implementation leverages the **mbedTLS** library for hardware-optimized cryptographic operations, providing a robust defense against "Man-in-the-Middle" (MitM) and data tampering attacks.

## 🛡️ Security Features
* **AES-256-CBC Encryption:** Military-grade encryption for the message payload, ensuring that data remains confidential even if intercepted.
* **SHA-256 Integrity Check:** Each packet includes a digital fingerprint to verify that the message has not been modified during transit.
* **Dynamic IV (Initialization Vector):** A unique, randomized 16-byte IV is generated for every transmission, preventing "Replay Attacks" and ensuring semantic security.
* **PKCS#7 Padding:** Ensures that variable-length data is correctly aligned for the AES block cipher.

## 🏗️ System Architecture
The secure packet is structured as follows to ensure reliable parsing and high security:

| Field | Size | Description |
| :--- | :--- | :--- |
| **Sync Header** | 2 Bytes | Identifies the start of a secure frame (`0xAA 0xBB`). |
| **IV** | 16 Bytes | Randomized Initialization Vector for AES-CBC. |
| **Payload Length** | 2 Bytes | Size of the encrypted data. |
| **Ciphertext** | Variable | The AES-256 encrypted message. |
| **SHA-256 Hash** | 32 Bytes | Integrity signature for the ciphertext. |

## 💻 Installation & Usage

1.  **Clone the Repository:**
    ```bash
    git clone [https://github.com/your-username/Secure-E2EE-IoT-Protocol.git](https://github.com/your-username/Secure-E2EE-IoT-Protocol.git)
    ```

2.  **Build the Firmware:**
    
    * Open Secure_Comm_Sender in PlatformIO and click Build.
    * Open Receiver in PlatformIO and click Build. (This generates the necessary .bin and .elf files for simulation)

3.  **Run Simulation:**
    * Open the root project folder in VS Code.
    * Press F1 and select Wokwi: Start Simulator.
      
4.  **Verify Output:**
    * Monitor the serial terminals. You will see the Sender encrypting the message and the Receiver validating the hash before decrypting it to reveal: DUNE_IS_AMAZING.   

---

## 📂 Directory Structure
```text
.
├── Secure_Comm_Sender/      # ESP32 Firmware for the Encryption Node
│   ├── src/main.cpp        # AES/SHA Implementation & Packet Generation
│   └── platformio.ini      # Build Configuration
├── Receiver/                # ESP32 Firmware for the Decryption Node
│   ├── src/main.cpp        # Packet Parsing & Decryption Logic
│   └── platformio.ini      # Build Configuration
├── diagram.json             # Wokwi Hardware Simulation Circuit
└── wokwi.toml               # Simulation Environment Configuration

