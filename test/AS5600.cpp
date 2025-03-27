#include <Wire.h>

class AS5600
{
public:
    AS5600();
    AS5600(int SDA, int SCL);
    float getAngle();

private:
    int _i2c_addr;
    float angle_offset = 0.0;
    int SDA;
    int SCL;
};

AS5600::AS5600()
{
    _i2c_addr = 0x36;

    // for esp32 
    SDA = 21;
    SCL = 22;

    Wire.begin();
}

AS5600::AS5600(int SDA, int SCL)
{
    this->SDA = SDA;
    this->SCL = SCL;
    _i2c_addr = 0x36;

    Wire.begin(SDA, SCL);
}

float AS5600::getAngle()
{
    Wire.beginTransmission(_i2c_addr);
    Wire.write(0x0C); // Address for angle data (high byte)
    Wire.endTransmission();
    Wire.requestFrom(_i2c_addr, 2); // Read 2 bytes of angle data

    if (Wire.available() == 2)
    {
        int highByte = Wire.read();
        int lowByte = Wire.read();
        int rawAngle = (highByte << 8) | lowByte;
        return (rawAngle * 360.0) / 4096.0; // Convert to degrees (12-bit resolution)
    }
    return -1.0; // Return -1 if reading fails
}
