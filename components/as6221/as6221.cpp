// Implementation based on:
//  - TMP117 Component

#include "as6221.h"
#include "esphome/core/log.h"

namespace esphome {
namespace as6221 {

static const char *const TAG = "as6221";

void AS6221Component::update() {
  int16_t data;
  if (!this->read_data_(&data)) {
    this->status_set_warning();
    return;
  }
  
  float temperature = data * 0.0078125f;

  ESP_LOGD(TAG, "Got temperature=%.2f°C", temperature);
  this->publish_state(temperature);
  this->status_clear_warning();

}
void AS6221Component::setup() {
  if (!this->write_config_(this->config_)) {
    this->mark_failed();
    return;
  }

  int16_t data;
  if (!this->read_data_(&data)) {
    this->mark_failed();
    return;
  }
}
void AS6221Component::dump_config() {
  ESP_LOGD(TAG, "as6221:");
  LOG_I2C_DEVICE(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, ESP_LOG_MSG_COMM_FAIL);
  }
  LOG_SENSOR("  ", "Temperature", this);
}
float AS6221Component::get_setup_priority() const { return setup_priority::DATA; }
bool AS6221Component::read_data_(int16_t *data) {
  if (!this->read_byte_16(0, (uint16_t *) data)) {
    ESP_LOGW(TAG, "Updating as6221 failed!");
    return false;
  }
  return true;
}

bool AS6221Component::read_config_(uint16_t *config) {
  if (!this->read_byte_16(1, (uint16_t *) config)) {
    ESP_LOGW(TAG, "Reading as6221 config failed!");
    return false;
  }
  return true;
}

bool AS6221Component::write_config_(uint16_t config) {
  if (!this->write_byte_16(1, config)) {
    ESP_LOGE(TAG, "Writing as6221 config failed!");
    return false;
  }
  return true;
}

}  // namespace as6221
}  // namespace esphome
