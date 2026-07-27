#pragma once
#include <Arduino.h>

class WRC_ServoDriver
{
public:
    static void Begin();
    static void setTargetAngle(int angle); // demande une position
    static void update();                  // avance progressivement

    // ⭐ Ajout pour SERVO_PORTE
    static void ouvrirPorte(); // angle ouvert depuis Settings.json
    static void fermerPorte(); // angle fermé depuis Settings.json

private:
    static int s_channel;
    static int s_pin;

    static int currentAngle; // position réelle
    static int targetAngle;  // position désirée
    static int servoSpeed;   // vitesse (° par update)
};
