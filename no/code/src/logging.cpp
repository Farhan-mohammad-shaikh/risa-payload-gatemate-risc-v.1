#include "logging.h"

Serial* serial;

void setupLogging(Serial* new_serial)
{
    if(new_serial)
    {
        serial = new_serial;
    }
}

void log(const char* prefix, const char* file, const char* function, uint32_t line, const char* format, ...)
{
    if(serial)
    {
        char buffer[256];
        char formattedMessage[256];

        va_list args;
        va_start(args, format);
        vsnprintf(formattedMessage, sizeof(formattedMessage), format, args);
        va_end(args);

        snprintf(buffer, sizeof(buffer), "[%s] %s:%s:%lu: %s\n", prefix, file, function, line, formattedMessage);

        serial->writeString(buffer);
    }
}

void print(const char* format, ...)
{
    if(serial)
    {
        char buffer[256];

        va_list args;
        va_start(args, format);
        vsprintf(buffer, format, args);
        va_end(args);

        serial->writeString(buffer);
    }
}
