#include "addons/i2cioexpander.h"
#include "storagemanager.h"
#include "hardware/i2c.h"
#include "mcp23017.h"

static const bool MIRROR_INTERRUPTS = true; //save a gpio by mirroring interrupts across both banks
static const bool OPEN_DRAIN_INTERRUPT_ACTIVE = false;
static const bool POLARITY_INTERRUPT_ACTIVE_LOW = false;
static const int MCP_ALL_PINS_INPUT = 0xffff;
static const int MCP_ALL_PINS_PULL_UP = 0xffff;
static const int MCP_ALL_PINS_COMPARE_TO_LAST = 0x0000;
static const int MCP_ALL_PINS_INTERRUPT_ENABLED = 0xffff;


bool I2CIOExpanderAddon::available() {
    return true;
}

void I2CIOExpanderAddon::setup() {
    const AddonOptions& options = Storage::getInstance().getAddonOptions();

	expander = new Mcp23017(I2C_IOEXPANDER_BLOCK, I2C_IOEXPANDER_ADDRESS);

	i2c_init(I2C_IOEXPANDER_BLOCK, I2C_IOEXPANDER_SPEED);
	gpio_set_function(I2C_IOEXPANDER_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(I2C_IOEXPANDER_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(I2C_IOEXPANDER_SDA_PIN);
	gpio_pull_up(I2C_IOEXPANDER_SCL_PIN);

    int result;
	
	result = expander->setup(MIRROR_INTERRUPTS, POLARITY_INTERRUPT_ACTIVE_LOW);
	result = expander->set_io_direction(MCP_ALL_PINS_INPUT);
	result = expander->set_pullup(MCP_ALL_PINS_PULL_UP);
	result = expander->set_interrupt_type(MCP_ALL_PINS_COMPARE_TO_LAST);
	result = expander->enable_interrupt(MCP_ALL_PINS_INTERRUPT_ENABLED);

	// gpio_set_irq_enabled_with_callback(gpio_irq, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

	//once we are listening for interrupts clear previous ones just incase
	// int int_values = mcp.get_interrupt_values();

    // preprocess()
}

void I2CIOExpanderAddon::preprocess() {
    // Gamepad * gamepad = Storage::getInstance().GetGamepad();

	// int pin = expander->get_last_interrupt_pin();
	// int int_values = expander->get_interrupt_values();
	// int input_values_ok = expander->update_and_get_input_values();

	// printf("MCP(0x%2x), PIN: %d, Int:%d, InputOK:%d\n", expander->get_address(), pin, int_values, input_values_ok);
	bool pin28 = expander->get_last_input_pin_value(28);
	// printf("InputPin0: %d\n", pin28);
	gpio_put(25, 1);
	// printf("InputPin1: %d\n", expander->get_last_input_pin_value(1));
	// printf("InputPin2: %d\n", expander->get_last_input_pin_value(2));
	// printf("InputPin3: %d\n", expander->get_last_input_pin_value(3));
}

void I2CIOExpanderAddon::process() {
}