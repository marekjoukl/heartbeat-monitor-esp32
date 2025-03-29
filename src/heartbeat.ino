#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// OLED display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1 // No reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin for the Pulse Sensor
const int PulsePin = 36;  // Using GPIO36 for the analog pin
int Signal;
float smoothedSignal = 0;  // For low-pass filtering
bool peakDetected = false;  // Flag for detecting rising edge

// Variables to store BPM data
int bpm = 0;
int lastValidBPM = 0;
int totalBeats = 0;
int beatCount = 0;
int avgBPM = 0;

// Peak detection variables
int threshold = 2976;
unsigned long lastBeatTime = 0;
unsigned long currentTime = 0;
unsigned long beatInterval = 0;
const int debounceTime = 400;  // Debounce time in milliseconds
const int minBeatInterval = 300;

// For waveform drawing
int lastX = 0; // Last X position of the plot
int lastY = SCREEN_HEIGHT - 1; // Last Y position (bottom of the screen)
const int PLOT_TOP = SCREEN_HEIGHT / 2 + 10;  // Start plotting at the middle + padding
const int PLOT_BOTTOM = SCREEN_HEIGHT - 1;  // Bottom boundary for the plot

// Valid signal range for computing BPM
const int minValidSignal = 2950;
const int maxValidSignal = 2998;

void setup() {
  // Initialize Serial Monitor' for debugging'
  Serial.begin(115200);
  Serial.println("Initializing...");

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }

  // Clear the display buffer
  display.clearDisplay();
  display.display();

  // Show Welcome message
  displayWelcomeMessage();

  // Display initial BPM and AVG BPM right away
  displayBPM(bpm);  // Show initial BPM (0)
  displayAVG(avgBPM);  // Show initial AVG (0)

  Serial.println("Pulse Sensor Initialized!");
}

void loop() {
  Signal = analogRead(PulsePin);
  // Apply a simple low-pass filter
  smoothedSignal = 0.9 * smoothedSignal + 0.1 * Signal;
  Serial.println(smoothedSignal);

  // Only calculate BPM if signal is within the valid range (2940-2980)
  if (smoothedSignal >= minValidSignal && smoothedSignal <= maxValidSignal) {
    currentTime = millis();

    // Detect the rising edge (signal crossing threshold on the way up)
    if (!peakDetected && smoothedSignal > threshold && (currentTime - lastBeatTime) > debounceTime) {
      peakDetected = true;  // Mark that a peak has been detected
    }

    // Confirm the peak by waiting for the signal to fall below the threshold
    if (peakDetected && smoothedSignal < threshold - 3) {
      beatInterval = currentTime - lastBeatTime;  // Time since the last beat

      // Ignore beats that happen too fast (less than 300 ms apart)
      if (beatInterval > minBeatInterval) {
        bpm = 60000 / beatInterval;  // Calculate BPM (60,000 ms / time between beats)

        // If the BPM is valid (within 40-200 range), calculate the average BPM
        if (bpm >= 40 && bpm <= 200) {
          totalBeats += bpm;
          beatCount++;
          avgBPM = totalBeats / beatCount;
          lastValidBPM = bpm;  // Store the last valid BPM

          // Update OLED display
          displayBPM(lastValidBPM);
          displayAVG(avgBPM);
        } else {
          displayBPM(lastValidBPM);  // Continue displaying the last valid BPM
        }

        Serial.print("BPM: ");
        Serial.println(bpm);

        lastBeatTime = currentTime;  // Update the last beat time
      }
      peakDetected = false;  // Reset peak detection for the next heartbeat
    }

    plotHeartbeat(smoothedSignal);
  }

  display.display();  // Update the OLED
  delay(20);  // Small delay for smooth plotting
}

// Function to display the Welcome message at the beginning
void displayWelcomeMessage() {
  display.clearDisplay();
  display.setCursor(10, 25);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.println("HeartBeat");
  display.setCursor(10, 45);
  display.println("Monitor");
  display.display();
  delay(2000);
  display.clearDisplay();  // Clear the screen after the welcome message
}

// Function to display the current BPM on the screen
void displayBPM(int currentBPM) {
  display.fillRect(0, 5, SCREEN_WIDTH, 15, SSD1306_BLACK);  // Clear only the top area for the BPM
  display.setCursor(10, 10);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("BPM: ");
  display.print(currentBPM);
}

// Function to display the AVG BPM on the screen
void displayAVG(int averageBPM) {
  display.fillRect(0, 20, SCREEN_WIDTH, 15, SSD1306_BLACK);  // Clear the area for the AVG
  display.setCursor(10, 25);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.print("AVG: ");
  display.print(averageBPM);
}

// Function to plot the heartbeat waveform on the OLED
void plotHeartbeat(float analogValue) {
  int x = lastX + 1;  // Move 1 pixel to the right
  if (x >= SCREEN_WIDTH) {
    x = 0;  // Wrap around if we reach the end of the screen
    lastX = 0;
    // Clear the entire plot area from the top boundary to the bottom boundary
    display.fillRect(0, PLOT_TOP - 1, SCREEN_WIDTH, PLOT_BOTTOM - PLOT_TOP + 2, SSD1306_BLACK);  
  }

  // Map the analog value to the plot range (2940-2980), within the bottom half of the screen
  int y = map(analogValue, minValidSignal, maxValidSignal, PLOT_BOTTOM, PLOT_TOP);  // Inverted Y axis for plotting in bottom half

  // Ensure the plot stays within the bottom half of the screen
  if (y < PLOT_TOP) {
    y = PLOT_TOP;
  }
  if (y > PLOT_BOTTOM) {
    y = PLOT_BOTTOM;
  }

  // Draw a thicker line for the heartbeat plot
  display.drawLine(lastX, lastY, x, y, SSD1306_WHITE);
  display.drawLine(lastX, lastY - 1, x, y - 1, SSD1306_WHITE);  // Thicker line for the plot

  // Update the last positions for the next iteration
  lastX = x;
  lastY = y;
}