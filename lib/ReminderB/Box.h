#ifndef BOX_H
#define BOX_H
#include <Arduino.h>
class Box {
    int medicine_id=0;
    String medicine_name="";
    byte dosage = 0;
    boolean success =false;
public:
    Box()=default;
    ~Box()=default;
    Box(int medicine_id, String medicine_name, byte dosage, boolean success);

    int & get_medicine_id();
    String & get_name();
    boolean & get_success();
    byte & get_dosage();

    void set_medicine_id(int medicine_id);
    void set_name(String medicine_name);
    void set_dosage(byte dosage);
    void set_success(boolean success);
};



#endif //BOX_H
