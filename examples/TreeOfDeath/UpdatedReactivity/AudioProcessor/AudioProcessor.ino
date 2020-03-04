const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;         //current sound level during sample window

int windowMin = 0;    //min value from ADC
int windowMax = 600; //max value from ADC
int scaledMin = 0;    //no added brightness
int scaledMax = 150;  //approx. 70% of brightness

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    unsigned long startMillis = millis(); // Start of sample window
    unsigned int peakToPeak = 0;          // peak-to-peak level

    unsigned int signalMax = 0;    //running max for current window
    unsigned int signalMin = 600; //running min for current window

    // collect data for 50 mS
    while (millis() - startMillis < sampleWindow)
    {
         sample = analogRead(0);
        //  Serial.println(sample);
        sample = constrain(sample, 0, 600);
        if (sample < 600) // toss out spurious readings
        {
            if (sample > signalMax)
            {
                signalMax = sample; // save just the max levels
            }
            else if (sample < signalMin)
            {
                signalMin = sample; // save just the min levels
            }
        }
    }
    peakToPeak = signalMax - signalMin; // max - min = peak-peak amplitude

    peakToPeak = map(peakToPeak, windowMin, windowMax, scaledMin, scaledMax);

    Serial.println(peakToPeak);
}