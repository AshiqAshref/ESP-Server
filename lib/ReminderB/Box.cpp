#include "Box.h"

Box::Box(int medicine_id, String medicine_name, byte dosage, boolean success) {
    this->medicine_id = medicine_id;
    this->medicine_name = medicine_name;
    this->dosage = dosage;
    this->success=success;
}

int & Box::get_medicine_id() {return medicine_id;}
[[nodiscard]] String & Box::get_name() {return medicine_name;}
[[nodiscard]] boolean & Box::get_success() {return success;}
[[nodiscard]] byte & Box::get_dosage() {return dosage;}

void Box::set_medicine_id(int medicine_id) {this->medicine_id = medicine_id;}
void Box::set_name(String medicine_name) {this->medicine_name = medicine_name;}
void Box::set_dosage(byte dosage) {this->dosage = dosage;}
void Box::set_success(boolean success) {this->success = success;}
