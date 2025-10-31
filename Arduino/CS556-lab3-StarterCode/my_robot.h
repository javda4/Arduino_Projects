#ifndef my_robot_h
#define my_robot_h
#include <Pololu3piPlus32U4.h>
using namespace Pololu3piPlus32U4;

class MyRobot {
  public:
    MyRobot();
    float obtain_time(int distance, int speed);
    long move_time(long start, int time_ms);
    void forward(int distance, int speed);
    void backward(int distance, int speed);
    void spin_left(int duration, int speed);
    void spin_right(int duration, int speed);
    void turn_left(int duration, int speed);
    void turn_right(int duration, int speed);
    void halt();

  private:
    Pololu3piPlus32U4::Motors motors;
    const int offset = 50;
};

#endif
