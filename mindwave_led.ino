/*
  Display signal value, attention and relaxation value through UART serial port
*/
#define BAUDRATE 9600
#define DEBUGOUTPUT 0

// Checking related variables
int   generatedChecksum = 0;
byte  checksum = 0;
int led_first = 4;
int led_second = 5;

// Receive data length and data array
byte  payloadLength = 0;
byte  payloadData[32] = {0};//Receive a total of 32 own data

// Information variable to be read
byte signalquality = 0;// signal quality
byte attention = 0;    // attention value
byte meditation = 0;   // relaxation value

// initialization
void setup()
{
  Serial.begin(BAUDRATE);
}

// Read a byte of data from the serial port
byte ReadOneByte()
{
  int ByteRead;
  while (!Serial.available());
  ByteRead = Serial.read();
  return ByteRead; // Returns the bytes read
}

// Read serial data
void read_serial_data()
{
  // Look for packet start sync byte, 2
  if (ReadOneByte() == 0xAA) // Read one first
  {
    if (ReadOneByte() == 0xAA) // Read the second
    {
      payloadLength = ReadOneByte();// Read the third, packet byte length
      if (payloadLength == 0x20) // If it receives large packets of data, it will continue to read, and small packets of data will be discarded and not read.
      {
        generatedChecksum = 0; // Check variable cleared to 0
        for (int i = 0; i < payloadLength; i++) // Read 32 bytes in a row
        {
          payloadData[i] = ReadOneByte();// Read data from a specified length packet
          generatedChecksum += payloadData[i];// Calculate data accumulation sum
        }
        checksum = ReadOneByte();// Read check byte
        // check
        generatedChecksum = (~generatedChecksum) & 0xff;
        // Compare check byte
        if (checksum == generatedChecksum) // Data received correctly, continue processing
        {
          signalquality = 0;// Signal quality variable
          attention = 0;    // Attention value variable
          // Assignment data
          signalquality = payloadData[1];// Signal value
          attention = payloadData[29];// Attention value
          meditation = payloadData[31];// Relaxation value
#if !DEBUGOUTPUT
          // Print signal quality
          Serial.print("SignalQuality: ");
          Serial.print(signalquality, DEC);
          // Print attention value
          Serial.print("Attation: ");
          Serial.print(attention, DEC);
          // Print relaxation value
          Serial.print("Meditation: ");
          Serial.print(meditation, DEC);
          // Newline
          Serial.print("\n");
#endif

          if (attention >= 50 && attention <= 80) {
            digitalWrite(led_first, HIGH);   // turn the LED on
          } else if (attention >= 80 && attention <= 100) {
            digitalWrite(led_first, HIGH);
            digitalWrite(led_second, HIGH);
          }
          else {
            digitalWrite(LED_BUILTIN, LOW); // turn the LED off
          }
        }
      }
    }
  }
}

// Main loop
void loop()
{
  read_serial_data(); // Read serial data
}
