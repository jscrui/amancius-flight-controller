// ROCKET FLIGHT COMPUTER - ESP8266 WEMOS D1 MINI

#include <Servo.h>
#include <Wire.h>
#include <SFE_BMP180.h>

Servo servo;
SFE_BMP180 bmp180; // Create a BMP180 object

double thresholdFall = 2; // Meters
double thresholdFly = 1;  // Meters

double pressure;
double temperature;
double initPressure;
double initAltitude;
double maxAltitude;

bool isFlying = false;
bool isFalling = false;
bool isLanded = false;

const int LED_GREEN = 14;
const int LED_RED = 12;
const int BUZZER = 15;

double getPressure();
void openParachute();
void saveData();

void Ready();
void Flying();
void Landed();

void Success();
void Error();

void setup()
{
    /** Starting */
    Serial.begin(9600);
    Serial.println("Starting...");

    /** Servo */
    servo.attach(2); // D4
    servo.write(0);  // 0 degrees

    /** Led's */
    pinMode(LED_RED, OUTPUT);     // D6 -> Red
    pinMode(LED_GREEN, OUTPUT);   // D5 -> Green
    pinMode(LED_BUILTIN, OUTPUT); // LED_BUILTIN -> Blue

    /** BuZZER */
    pinMode(BUZZER, OUTPUT); // D8

    /** BMP180 - Start */
    if (!bmp180.begin())
    {
        Serial.println("Could not find a valid BMP085 sensor, check wiring!");
        Error();
    }

    /** Altitudes */
    initPressure = getPressure();
    initAltitude = bmp180.altitude(initPressure, initPressure);
    maxAltitude = initAltitude;

    /** Save data */
    // TODO

    /** Success */
    Success();

    /** Delay */
    delay(2000);
}

void loop()
{
    double Altitude, Pressure;

    Pressure = getPressure();
    Altitude = bmp180.altitude(Pressure, initPressure);

    if (Altitude > maxAltitude)
    {
        maxAltitude = Altitude;
    }

    if (Altitude > (initAltitude + thresholdFly))
    {
        isFlying = true;
        Flying();
    }

    if (isFlying && Altitude < (maxAltitude - thresholdFall))
    {
        openParachute();
        isFalling = true;
    }

    if (isFalling && Altitude < (initAltitude + thresholdFly))
    {
        isLanded = true;
        Landed();
    }

    if (!isLanded && !isFalling && !isFlying)
    {
        Ready();
    }

    delay(20);
}

double getPressure()
{
    char status;
    double T, P;

    // You must first get a temperature measurement to perform a pressure reading.

    // Start a temperature measurement:
    // If request is successful, the number of ms to wait is returned.
    // If request is unsuccessful, 0 is returned.

    status = bmp180.startTemperature();
    if (status != 0)
    {
        // Wait for the measurement to complete:

        delay(status);

        // Retrieve the completed temperature measurement:
        // Note that the measurement is stored in the variable T.
        // Use '&T' to provide the address of T to the function.
        // Function returns 1 if successful, 0 if failure.

        status = bmp180.getTemperature(T);
        if (status != 0)
        {
            // Start a pressure measurement:
            // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
            // If request is successful, the number of ms to wait is returned.
            // If request is unsuccessful, 0 is returned.

            status = bmp180.startPressure(3);
            if (status != 0)
            {
                // Wait for the measurement to complete:
                delay(status);

                // Retrieve the completed pressure measurement:
                // Note that the measurement is stored in the variable P.
                // Use '&P' to provide the address of P.
                // Note also that the function requires the previous temperature measurement (T).
                // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
                // Function returns 1 if successful, 0 if failure.

                status = bmp180.getPressure(P, T);
                if (status != 0)
                {
                    return (P);
                }
                else
                    Serial.println("error retrieving pressure measurement\n");
                Error();
            }
            else
                Serial.println("error starting pressure measurement\n");
            Error();
        }
        else
            Serial.println("error retrieving temperature measurement\n");
        Error();
    }
    else
        Serial.println("error starting temperature measurement\n");
    Error();

    return (0);
}

void saveData()
{
}

void openParachute()
{
    servo.write(180);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
}

void Ready()
{
    digitalWrite(LED_GREEN, HIGH);
}

void Flying()
{
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, HIGH);
}

void Landed()
{
    while (1)
    {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(BUZZER, HIGH);
        delay(750);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(BUZZER, LOW);
        delay(500);
    }
}

void Success()
{
    for (int i = 0; i < 3; i++)
    {
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(BUZZER, HIGH);
        delay(750);
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(BUZZER, LOW);
        delay(500);
    }
}

void Error()
{
    while (true)
    {
        digitalWrite(LED_RED, HIGH);
        delay(200);
        digitalWrite(LED_RED, LOW);
        delay(200);
    }
}