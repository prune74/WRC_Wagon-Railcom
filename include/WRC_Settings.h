#pragma once
#include <stdint.h>
#include <arduino.h>

class WRC_Settings
{
public:
    // Paramètres généraux
    static bool WIFI_ACTIF;
    static uint16_t ADRESSE;

    // ESSIEUX
    static bool ESSIEUX;
    
    // Type WAGON
    static String TYPE_WAGON;

    // Servo porte
    static uint16_t SERVO_PORTE_ANGLE_OUVERT;
    static uint16_t SERVO_PORTE_ANGLE_FERME;
    static uint16_t SERVO_PORTE_VITESSE;

    // Fonctions FX:
    static bool FEU_ARRIERE;
    static bool LUMIERE_INTERIEURE;
    static bool SERVO_PORTE;



    // Fonctions
    static void Begin();
    static void readFile();
    static void writeFile();

    static uint16_t getAdresse();
};
