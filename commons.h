#ifndef _COMMONS_H
#define _COMMONS_H

/* these are relays on Arduino Control Board V2 */
#define RT1         0b01110000
#define RT2         0b01111000
#define RT3         0b01110100
#define RT4         0b01111100
#define RT5         0b01110010
#define RT6         0b01111010
#define RT7         0b01110110
#define RT8         0b01111110
#define RT9         0b01110001
#define RT10        0b01111001
#define RT11        0b01110101
#define RT12        0b01111101
#define RT13        0b01110011
#define RT14        0b01111011
#define RT15        0b01110111
#define RT16        0b01111111

#define RT17        0b10110000
#define RT18        0b10111000
#define RT19
#define RT20
#define RT21
#define RT22
#define RT23
#define RT24
#define RT25
#define RT26
#define RT27
#define RT28
#define RT29
#define RT30
#define RT31
#define RT32

#define RT33
#define RT34
#define RT35
#define RT36
#define RT37
#define RT38
#define RT39
#define RT40
#define RT41
#define RT42
#define RT43
#define RT44
#define RT45
#define RT46
#define RT47
#define RT48

#define RT49
#define RT50
#define RT51
#define RT52
#define RT53
#define RT54
#define RT55
#define RT56
#define RT57
#define RT58
#define RT59
#define RT60
#define RT61
#define RT62
#define RT63
#define RT64

/* single relays or 2/4/8/16 relay modules are usually connected to TB5/6/7 */

/* one way to call them is to use digitalWrite() in Arduino framework */
// #define RLY1_ON     pinMode(37, OUTPUT);digitalWrite(37, LOW);
// #define RLY1_OFF    pinMode(37, OUTPUT);digitalWrite(37, HIGH);
// #define RLY2_ON     pinMode(36, OUTPUT);digitalWrite(36, LOW);
// #define RLY2_OFF    pinMode(36, OUTPUT);digitalWrite(36, HIGH);
// #define RLY3_ON     pinMode(35, OUTPUT);digitalWrite(35, LOW);
// #define RLY3_OFF    pinMode(35, OUTPUT);digitalWrite(35, HIGH);
// #define RLY4_ON     pinMode(34, OUTPUT);digitalWrite(34, LOW);
// #define RLY4_OFF    pinMode(34, OUTPUT);digitalWrite(34, HIGH);
// #define RLY5_ON     pinMode(33, OUTPUT);digitalWrite(33, LOW);
// #define RLY5_OFF    pinMode(33, OUTPUT);digitalWrite(33, HIGH);
// #define RLY6_ON     pinMode(32, OUTPUT);digitalWrite(32, LOW);
// #define RLY6_OFF    pinMode(32, OUTPUT);digitalWrite(32, HIGH);
// #define RLY7_ON     pinMode(A15, OUTPUT);digitalWrite(A15, LOW);
// #define RLY7_OFF    pinMode(A15, OUTPUT);digitalWrite(A15, HIGH);
// #define RLY8_ON     pinMode(A14, OUTPUT);digitalWrite(A14, LOW);
// #define RLY8_OFF    pinMode(A14, OUTPUT);digitalWrite(A14, HIGH);
// #define RLY9_ON     pinMode(A13, OUTPUT);digitalWrite(A13, LOW);
// #define RLY9_OFF    pinMode(A13, OUTPUT);digitalWrite(A13, HIGH);
// #define RLY10_ON     pinMode(A12, OUTPUT);digitalWrite(A12, LOW);
// #define RLY10_OFF    pinMode(A12, OUTPUT);digitalWrite(A12, HIGH);
// #define RLY11_ON     pinMode(A11, OUTPUT);digitalWrite(A11, LOW);
// #define RLY11_OFF    pinMode(A11, OUTPUT);digitalWrite(A11, HIGH);
// #define RLY12_ON     pinMode(A10, OUTPUT);digitalWrite(A10, LOW);
// #define RLY12_OFF    pinMode(A10, OUTPUT);digitalWrite(A10, HIGH);

/* another way is to use #define in AVR framework */
// #define RLY1        PC0
// #define RLY2        PC1
// #define RLY3        PC2
// #define RLY4        PC3
// #define RLY5        PC4
// #define RLY6        PC5
// #define RLY7        PK7
// #define RLY8        PK6
// #define RLY9        PK5
// #define RLY10       PK4
// #define RLY11       PK3
// #define RLY12       PK2

/* or use array of structs if automatic iterations are involved */
struct relay_module_t {
    uint8_t pin;
    char* descriptor;
};

struct relay_module_t relay_16[] = {
    {PC0, "SW_L1_IN"},
    {PC1, "SW_L2_IN"},
    {PC2, "SW_GND_IN"},
    {PC3, "SW_STAK_L1"},
    {PC4, "SW_STAK_L2"},
    {PC5, "SW_L1_LL"},
    {PK7, "SW_L1_HL"},
    {PK6, "SW_L2_LL"},
    {PK5, "SW_L2_HL"},
    {PK4, "SW_STATE_B"},
    {PK3, "SW_STATE_A"},
    {PK2, "SW_DIODE"}
};

/* use the same struct for onboard relays */
struct relay_module_t test_points[] = {
    {0b01110000, "RT1"},
    {0b01111000, "RT2"},
    {0b01110100, "RT3"},
    {0b01111100, "RT4"},
    {0b01110010, "RT5"},
    {0b01111010, "RT6"},
    {0b01110110, "RT7"},
    {0b01111110, "RT8"},
    {0b01110001, "RT9"},
    {0b01111001, "RT10"},
    {0b01110101, "RT11"},
    {0b01111101, "RT12"},
    {0b01110011, "RT13"},
    {0b01111011, "RT14"},
    {0b01110111, "RT15"},
    {0b01111111, "RT16"}
};

/* protocol for return message */
struct message_packet_t {
        int number_beep;
        int number_blink;
        int output_volt;
        int TP12;
        int TP19;
        int TP21;
        int TP22;
        int TP25;
        int TP26;
        int TP35;
    };

#endif