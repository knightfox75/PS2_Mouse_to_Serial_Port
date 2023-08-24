/*******************************************************************************
  
    PS2-Mouse-Arduino

    https://github.com/kristopher/PS2-Mouse-Arduino
  
    "PS2-Mouse-Arduino" is under MIT License
    Copyright (c) 2017 by Kris Chambers

    Modifications by Cesar Rincon "NightFox" are under MIT License
    Copyright (c) 2023 by Cesar Rincon

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal	in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software,
    and to permit persons to whom the Software is furnished to do so,
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all	copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER	LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 
*******************************************************************************/



#ifndef PS2Mouse_h

#define PS2Mouse_h
#define REMOTE 1
#define STREAM 2

class PS2Mouse
{
  private:
    int _clock_pin;
    int _data_pin;
    int _mode;
    int _initialized;
    int _enabled;
    int _disabled;
    int8_t read_byte();
    int read_bit();
    int16_t read_movement_x(int);
    int16_t read_movement_y(int);
    void pull_high(int);
    void pull_low(int);
    void set_mode(int);
    const int32_t TIME_OUT = 100;         // Added by NightFox
  public:
    PS2Mouse(int, int, int mode = REMOTE);
    bool initialize();                    // Modified by NightFox
    int clock_pin();
    int data_pin();
    int read();
    int16_t* report(int16_t data[]);
    void write(int);
    void enable_data_reporting();
    void disable_data_reporting();
    void set_remote_mode();
    void set_stream_mode();
    void set_resolution(int);
    void set_scaling_2_1();
    void set_scaling_1_1();
    void set_sample_rate(int);

    bool SafeWrite(int data);             // Added by NightFox

};

#endif
