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

#define MCP_IRQ_GPIO_PIN PIN_BUTTON_A1

bool interrupt = false;

void I2CIOExpanderAddon::gpio_callback(uint gpio, uint32_t events) {
	if ((gpio == MCP_IRQ_GPIO_PIN) && (events & GPIO_IRQ_EDGE_FALL)) {
		interrupt = true;
	}
}

bool I2CIOExpanderAddon::available() {
    return true;
}

void I2CIOExpanderAddon::setup() {
    const AddonOptions& options = Storage::getInstance().getAddonOptions();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);	

	auto setupi2c = [](){
		i2c_init(I2C_IOEXPANDER_BLOCK, I2C_IOEXPANDER_SPEED);
		gpio_set_function(I2C_IOEXPANDER_SDA_PIN, GPIO_FUNC_I2C);
		gpio_set_function(I2C_IOEXPANDER_SCL_PIN, GPIO_FUNC_I2C);
		gpio_pull_up(I2C_IOEXPANDER_SDA_PIN);
		gpio_pull_up(I2C_IOEXPANDER_SCL_PIN);
		gpio_pull_up(MCP_IRQ_GPIO_PIN);
	};
	setupi2c();
	expander = new Mcp23017(I2C_IOEXPANDER_BLOCK, I2C_IOEXPANDER_ADDRESS);
	
	expander->setup(MIRROR_INTERRUPTS, POLARITY_INTERRUPT_ACTIVE_LOW);
	expander->set_io_direction(0xFEFF);
	expander->set_pullup(0xFEFF);
	expander->set_interrupt_type(MCP_ALL_PINS_COMPARE_TO_LAST);
	expander->enable_interrupt(0xFEFF);
	// clear interrupt
	int input_values_ok = expander->update_and_get_input_values();

	gpio_set_irq_enabled_with_callback(
		MCP_IRQ_GPIO_PIN, 
		GPIO_IRQ_EDGE_FALL,
		true, 
		&I2CIOExpanderAddon::gpio_callback
	);
}

void I2CIOExpanderAddon::preprocess() {
    Gamepad * gamepad = Storage::getInstance().GetGamepad();
	bool s1 = gamepad->pressedS1();

	if (interrupt) {
		interrupt = false;
		expander->update_and_get_input_values();
		gpio_put(PICO_DEFAULT_LED_PIN, !expander->get_last_input_pin_value(0));
	}
	expander->set_output_bit_for_pin(8, s1);
	expander->flush_output();
}

I2CIOExpanderAddon::~I2CIOExpanderAddon() {
	delete expander;
}