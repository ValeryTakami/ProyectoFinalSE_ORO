#include <EEPROM.h>
#include <Arduino.h>  // para definiciones de tipos

// Esta función escribe un valor de cualquier tipo en la memoria EEPROM.
// Toma la dirección de memoria 'ee' y el valor 'value' a ser escrito.
template <class T>
int EEPROM_writeAnything(int ee, const T& value)
{
    const byte* p = (const byte*)(const void*)&value;
    unsigned int i;

    // Itera a través de cada byte del valor y lo escribe en la EEPROM.
    for (i = 0; i < sizeof(value); i++)
        EEPROM.write(ee++, *p++);

    // Devuelve el número total de bytes escritos.
    return i;
}

// Esta función lee un valor de cualquier tipo desde la memoria EEPROM.
// Toma la dirección de memoria 'ee' y una referencia 'value' para almacenar el valor leído.
template <class T>
int EEPROM_readAnything(int ee, T& value)
{
    byte* p = (byte*)(void*)&value;
    unsigned int i;

    // Itera a través de cada byte de la EEPROM, lee y almacena en la variable 'value'.
    for (i = 0; i < sizeof(value); i++)
        *p++ = EEPROM.read(ee++);

    // Devuelve el número total de bytes leídos.
    return i;
}
