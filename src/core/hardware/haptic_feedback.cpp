#include "haptic_feedback.h"
#include <Wire.h>

// DRV2605 Configuration
#define DRV2605_ADDR     0x5A    // I2C address for DRV2605
#define DRV2605_STATUS   0x00
#define DRV2605_MODE     0x01
#define DRV2605_RTPIN    0x02
#define DRV2605_LIBRARY  0x03
#define DRV2605_WAVESEQ1 0x04
#define DRV2605_WAVESEQ2 0x05
#define DRV2605_WAVESEQ3 0x06
#define DRV2605_WAVESEQ4 0x07
#define DRV2605_WAVESEQ5 0x08
#define DRV2605_WAVESEQ6 0x09
#define DRV2605_WAVESEQ7 0x0A
#define DRV2605_WAVESEQ8 0x0B
#define DRV2605_GO       0x0C
#define DRV2605_OVERDRIVE 0x0D
#define DRV2605_SUSTAIN  0x0E
#define DRV2605_BRAKE    0x0F
#define DRV2605_AUDIOCTRL 0x10

bool HapticFeedback::haptic_enabled = true;

void HapticFeedback::begin() {
    Wire.begin(); // Initialize I2C
    initialize();
}

void HapticFeedback::setEnabled(bool enabled) {
    haptic_enabled = enabled;
}

bool HapticFeedback::isEnabled() {
    return haptic_enabled;
}

void HapticFeedback::playEffect(HapticEffect effect) {
    if (!haptic_enabled) return;
    
    // Clear any previous sequence
    writeRegister(DRV2605_WAVESEQ1, 0);
    writeRegister(DRV2605_WAVESEQ2, 0);
    writeRegister(DRV2605_WAVESEQ3, 0);
    writeRegister(DRV2605_WAVESEQ4, 0);
    writeRegister(DRV2605_WAVESEQ5, 0);
    writeRegister(DRV2605_WAVESEQ6, 0);
    writeRegister(DRV2605_WAVESEQ7, 0);
    writeRegister(DRV2605_WAVESEQ8, 0);
    
    // Set the effect
    writeRegister(DRV2605_WAVESEQ1, (uint8_t)effect);
    
    // Start playback
    writeRegister(DRV2605_GO, 1);
}

void HapticFeedback::click() {
    playEffect(HAPTIC_CLICK_MEDIUM);
}

void HapticFeedback::screenChange() {
    playEffect(HAPTIC_CLICK_STRONG);
}

void HapticFeedback::peakReached() {
    playEffect(HAPTIC_PULSE_SHARP);
}

void HapticFeedback::touch() {
    playEffect(HAPTIC_CLICK_LIGHT);
}

void HapticFeedback::error() {
    playEffect(HAPTIC_ERROR);
}

void HapticFeedback::confirmation() {
    playEffect(HAPTIC_DOUBLE_CLICK);
}

void HapticFeedback::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(DRV2605_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t HapticFeedback::readRegister(uint8_t reg) {
    Wire.beginTransmission(DRV2605_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(DRV2605_ADDR, 1);
    return Wire.read();
}

void HapticFeedback::initialize() {
    // Check if DRV2605 is connected
    uint8_t status = readRegister(DRV2605_STATUS);
    if (status == 0xFF) {
        Serial.println("DRV2605 not found - haptic feedback disabled");
        haptic_enabled = false;
        return;
    }
    
    // Initialize DRV2605
    writeRegister(DRV2605_MODE, 0x00);       // Exit standby
    writeRegister(DRV2605_RTPIN, 0x00);      // Disable real-time input
    writeRegister(DRV2605_LIBRARY, 1);       // Use ROM library A
    writeRegister(DRV2605_OVERDRIVE, 0);     // No overdrive
    writeRegister(DRV2605_SUSTAIN, 0);       // No sustain
    writeRegister(DRV2605_BRAKE, 0);         // No brake
    writeRegister(DRV2605_AUDIOCTRL, 0x10);  // Default audio control
    
    Serial.println("DRV2605 haptic feedback initialized");
    
    // Test haptic feedback
    playEffect(HAPTIC_DOUBLE_CLICK);
}
