#include "main.h"

typedef enum {
    ABORT,
    LAUNCH,
    MOVE
} commandID_t;

typedef enum {
    MEV,
    PRES,
    OX,
    IGNIT,
    NO_VALVE
} valve_t;

typedef enum {
    CLOSE,
    OPEN,
    NO_ACTION
} action_t;

typedef struct {
    commandID_t id;
    valve_t valve;
    action_t action;
} commandPacket_t;

typedef struct {
    valve_t valve;
    action_t action;
    uint32_t time;
} launchAction_t;

typedef struct {
    uint8_t id;
    float value;
} instrumentationPacket_t;

commandPacket_t string_to_command(uint8_t *line);
void command_to_string(uint8_t *line, commandPacket_t command);

