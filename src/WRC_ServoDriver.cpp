#include "WRC_ServoDriver.h"
#include "WRC_Pins.h"
#include "WRC_Settings.h"

int WRC_ServoDriver::s_channel = 0;
int WRC_ServoDriver::s_pin     = WRC_FxPins::SERVO_DOOR;

int WRC_ServoDriver::currentAngle = 0;
int WRC_ServoDriver::targetAngle  = 0;
int WRC_ServoDriver::servoSpeed   = 3;

void WRC_ServoDriver::Begin()
{
    const int freq = 50;        // 50 Hz servo
    const int resolution = 16;  // 16 bits

    ledcSetup(s_channel, freq, resolution);
    ledcAttachPin(s_pin, s_channel);

    // Lecture de la vitesse depuis settings.json
    servoSpeed = WRC_Settings::SERVO_PORTE_VITESSE;

    currentAngle = 0;
    targetAngle  = 0;

    // Position initiale (500 µs)
    uint32_t duty = (500 * 65535) / 20000;
    ledcWrite(s_channel, duty);
}

void WRC_ServoDriver::setTargetAngle(int angle)
{
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;

    targetAngle = angle;
}

void WRC_ServoDriver::update()
{
    if (currentAngle == targetAngle)
        return;

    // Vitesse dynamique depuis settings.json
    int speed = WRC_Settings::SERVO_PORTE_VITESSE;

    // Avance progressivement
    if (currentAngle < targetAngle)
        currentAngle += speed;
    else
        currentAngle -= speed;

    // Clamp
    if (currentAngle < 0) currentAngle = 0;
    if (currentAngle > 180) currentAngle = 180;

    // Convertit angle → pulse µs
    int pulse_us = map(currentAngle, 0, 180, 500, 2500);

    // Convertit pulse → duty LEDC
    uint32_t duty = (pulse_us * 65535) / 20000;

    ledcWrite(s_channel, duty);
}

/* ---------------------------------------------------------------------------
 * OUVERTURE / FERMETURE PORTE
 * ------------------------------------------------------------------------- */
void WRC_ServoDriver::ouvrirPorte()
{
    setTargetAngle(WRC_Settings::SERVO_PORTE_ANGLE_OUVERT);
}

void WRC_ServoDriver::fermerPorte()
{
    setTargetAngle(WRC_Settings::SERVO_PORTE_ANGLE_FERME);
}
