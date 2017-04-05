#include <bc_tca9534a.h>

#define BC_TCA9534A_REGISTER_INPUT_PORT 0x00
#define BC_TCA9534A_REGISTER_OUTPUT_PORT 0x01
#define BC_TCA9534A_REGISTER_POLARITY_INVERSION 0x02
#define BC_TCA9534A_REGISTER_CONFIGURATION 0x03

bool bc_tca9534a_init(bc_tca9534a_t *self, bc_i2c_channel_t i2c_channel, uint8_t i2c_address)
{
    memset(self, 0, sizeof(*self));

    self->_i2c_channel = i2c_channel;
    self->_i2c_address = i2c_address;

    bc_i2c_init(self->_i2c_channel, BC_I2C_SPEED_400_KHZ);

    uint8_t direction;

    if (!bc_i2c_read_8b(self->_i2c_channel, self->_i2c_address, BC_TCA9534A_REGISTER_CONFIGURATION, &direction))
    {
        return false;
    }

    return true;
}

bool bc_tca9534a_read_port(bc_tca9534a_t *self, uint8_t *value)
{
    if (!bc_i2c_read_8b(self->_i2c_channel, self->_i2c_address, BC_TCA9534A_REGISTER_INPUT_PORT, value))
    {
        return false;
    }

    return true;
}

bool bc_tca9534a_write_port(bc_tca9534a_t *self, uint8_t value)
{
    if (!bc_i2c_write_8b(self->_i2c_channel, self->_i2c_address, BC_TCA9534A_REGISTER_OUTPUT_PORT, value))
    {
        return false;
    }

    return true;
}

bool bc_tca9534a_read_pin(bc_tca9534a_t *self, bc_tca9534a_pin_t pin, bc_tca9534a_state_t *value)
{
    uint8_t port;

    if (!bc_tca9534a_read_port(self, &port))
    {
        return false;
    }

    if (((port >> (uint8_t) pin) & 1) == 0)
    {
        *value = BC_TCA9534A_PIN_STATE_LOW;
    }
    else
    {
        *value = BC_TCA9534A_PIN_STATE_HIGH;
    }

    return true;
}

bool bc_tca9534a_write_pin(bc_tca9534a_t *self, bc_tca9534a_pin_t pin, bc_tca9534a_state_t value)
{
    uint8_t port;

    if (!bc_tca9534a_read_port(self, &port))
    {
        return false;
    }

    port &= ~(1 << (uint8_t) pin);

    if (value != BC_TCA9534A_PIN_STATE_LOW)
    {
        port |= 1 << (uint8_t) pin;
    }

    if (!bc_tca9534a_write_port(self, port))
    {
        return false;
    }

    return true;
}

bool bc_tca9534a_get_port_direction(bc_tca9534a_t *self, uint8_t *direction)
{
    if (!bc_i2c_read_8b(self->_i2c_channel, self->_i2c_address, BC_TCA9534A_REGISTER_CONFIGURATION, direction))
    {
        return false;
    }

    return true;
}

bool bc_tca9534a_set_port_direction(bc_tca9534a_t *self, uint8_t direction)
{
    if (!bc_i2c_write_8b(self->_i2c_channel, self->_i2c_address, BC_TCA9534A_REGISTER_CONFIGURATION, direction))
    {
        return false;
    }

    return true;
}

bool bc_tca9534a_get_pin_direction(bc_tca9534a_t *self, bc_tca9534a_pin_t pin, bc_tca9534a_pin_direction_t *direction)
{
    uint8_t port_direction;

    if (!bc_tca9534a_get_port_direction(self, &port_direction))
    {
        return false;
    }

    if (((port_direction >> (uint8_t) pin) & 1) == 0)
    {
        *direction = BC_TCA9534A_PIN_DIRECTION_OUTPUT;
    }
    else
    {
        *direction = BC_TCA9534A_PIN_DIRECTION_INPUT;
    }

    return true;
}

bool bc_tca9534a_set_pin_direction(bc_tca9534a_t *self, bc_tca9534a_pin_t pin, bc_tca9534a_pin_direction_t direction)
{
    uint8_t port_direction;

    if (!bc_tca9534a_get_port_direction(self, &port_direction))
    {
        return false;
    }

    port_direction &= ~(1 << (uint8_t) pin);

    if (direction == BC_TCA9534A_PIN_DIRECTION_INPUT)
    {
        port_direction |= 1 << (uint8_t) pin;
    }

    if (!bc_tca9534a_set_port_direction(self, port_direction))
    {
        return false;
    }

    return true;
}
