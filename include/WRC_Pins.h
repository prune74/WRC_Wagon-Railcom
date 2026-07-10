#pragma once
#include <driver/gpio.h>

/*
 * 📌 Notes GPIO pour ESP32‑C3 (à lire avant portage)
 * --------------------------------------------------
 * L’ESP32‑C3 n’a pas les mêmes broches que l’ESP32 classique.
 * Beaucoup de GPIO sont absents, et certains sont réservés.
 *
 * 🟢 GPIO SÛRS (utilisables sans risque)
 * -------------------------------------
 * GPIO 0  → OK (attention : strapping pin, éviter pour servo)
 * GPIO 1  → OK
 * GPIO 2  → OK
 * GPIO 3  → OK
 * GPIO 4  → OK
 * GPIO 5  → OK
 * GPIO 6  → OK
 * GPIO 7  → OK
 * GPIO 8  → OK
 * GPIO 9  → OK
 *
 * 🟡 GPIO SPÉCIAUX (utilisables mais avec précautions)
 * ----------------------------------------------------
 * GPIO 0  → Strapping pin (boot mode). Éviter pour signaux critiques.
 * GPIO 1  → UART TX par défaut.
 * GPIO 2  → UART RX par défaut.
 * GPIO 3  → Peut être utilisé pour I2C / LED / servo.
 *
 * 🟥 GPIO INTERDITS / RÉSERVÉS
 * -----------------------------
 * ❌ GPIO 10 → réservé au flash interne (ne jamais utiliser)
 * ❌ GPIO 11 → réservé au flash interne (ne jamais utiliser)
 *
 * 🟦 Recommandations pour ton wagon RailCom
 * -----------------------------------------
 * - RAILCOM_IN : GPIO 4 (parfait, compatible ESP32‑C3)
 * - LED_STATUS : GPIO 2 (OK sur C3)
 * - SERVO_DOOR : choisir parmi 3, 4, 5, 6, 7, 8, 9
 * - LED_INTERIEURE : choisir parmi 3, 4, 5, 6, 7, 8, 9
 * - LED_ARRIERE : choisir parmi 3, 4, 5, 6, 7, 8, 9
 *
 * 🟢 Broches recommandées pour ton futur portage ESP32‑C3 :
 * ---------------------------------------------------------
 * SERVO_DOOR      = GPIO 7
 * LED_INTERIEURE  = GPIO 8
 * LED_ARRIERE     = GPIO 9
 *
 * Ces trois broches sont :
 * - disponibles
 * - sûres
 * - non réservées
 * - parfaites pour PWM / LED / servo
 *
 * ⚠ Attention : GPIO 10 et 11 sont interdits sur ESP32‑C3.
 */


namespace WRC_Pins
{
    // Entrée RailCom (sortie comparateur TLV3501)
    static const gpio_num_t RAILCOM_IN = GPIO_NUM_4;

    // Mesure tension voie (optionnel, ADC)
    static const gpio_num_t TRACK_ADC  = GPIO_NUM_5;

    // LED interne (statut wagon)
    static const gpio_num_t LED_STATUS = GPIO_NUM_2;   // OK sur ESP32
}

namespace WRC_FxPins
{
    // Servo porte → GPIO sûr
    static const gpio_num_t SERVO_DOOR = GPIO_NUM_14;   // GPIO_NUM_7

    // LED intérieure → GPIO sûr
    static const gpio_num_t LED_INTERIEURE = GPIO_NUM_27;  // GPIO_NUM_8

    // LED arrière → GPIO sûr
    static const gpio_num_t LED_ARRIERE    = GPIO_NUM_26;  // GPIO_NUM_9
}
