import esphome.codegen as cg
from esphome.components import i2c, sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_UPDATE_INTERVAL,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
)

DEPENDENCIES = ["i2c"]
CODEOWNERS = ["@drakesoft"]

as6221_ns = cg.esphome_ns.namespace("as6221")
AS6221Component = as6221_ns.class_(
    "AS6221Component", cg.PollingComponent, i2c.I2CDevice, sensor.Sensor
)

CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(
        AS6221Component,
        unit_of_measurement=UNIT_CELSIUS,
        accuracy_decimals=1,
        device_class=DEVICE_CLASS_TEMPERATURE,
        state_class=STATE_CLASS_MEASUREMENT,
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x48))
)


def determine_config_register(polling_period):
    if polling_period >= 8000:
        # 8 conv/s
        return 0x00C0
    if polling_period >= 4000:
        # 4 conv/s
        return 0x0080
    if polling_period >= 1000:
        # 1 conv/s
        return 0x0040
    return 0x0000


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    update_period = config[CONF_UPDATE_INTERVAL].total_milliseconds
    cg.add(var.set_config(determine_config_register(update_period)))
