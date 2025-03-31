#include "controls.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

const uint8_t *ids[] = {
    "ABORT",
    "LAUNCH",
    "MOVE"
};

const uint8_t *valves[] = {
	"MEV",
    "PRES",
    "OX",
    "IGNIT"
};

const uint8_t *actions[] = {
    "CLOSE",
    "OPEN"
};

int _find_array_match(uint8_t *str, uint8_t n, uint8_t *options[n])
{
    for(int i = 0; i < n; i++)
    {
        if(strcmp(str, options[i]) == 0)
            return i;
    }
    return n;
}

commandPacket_t string_to_command(uint8_t *line) 
{
    commandPacket_t command = {0, 0, 0};
    uint8_t str[10];
    uint8_t str_size = 0;
    uint8_t current_val = 0;

    for(int i = 0; line[i] != '\0'; i++)
    {
        str[str_size] = line[i];
        str_size++;
        if(line[i] == ',' || i == strlen(line) - 1)
        {
            i == strlen(line) - 1 ? str_size : str_size--;
            str[str_size] = '\0';
            if(current_val == 0)
                command.id = _find_array_match(str, 3, ids);
            if(current_val == 1)
                command.valve = _find_array_match(str, 3, valves);
            if(current_val == 2)
                command.action = _find_array_match(str, 2, actions);
            str_size = 0;
            current_val++;
        }
    }
    
    return command;
}

void command_to_string(uint8_t *str, commandPacket_t command)
{
    sprintf(str, "%s,%s,%d\n", ids[command.id], valves[command.valve], command.action);
}
