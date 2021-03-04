# Sandwich
Sandwich is a small and versatile robot, powered by Jetson Nano and a Raspberry Pi PICO. It has 2 DC motors and a Raspberry Camera on the front.
![](https://raw.githubusercontent.com/tolomeis/sandwich/main/imgs/building_2.jpg)
- NVIDIA Jetson Nano
-  Raspberry Pi Pico-based custom board
- 10.000mAh power bank
- Raspberry Camera v2.1 for autonomous driving
- 2x Pololu Micro gear motors
- Pololu TB6612FNG Motor driver

 
 Future upgrades:
 - Sharp IR distance sensor
 - IMU
 
## The hardware
Sandwich is built over an old robotic kit named Axemblo, now it doesn't sell anymore, but it similar, at least on the idea, to the MakeBlock.
All the parts are made in aluminum and held together by M3 screws. On the lower part, between the aluminum racks, is the battery, a 10.000 power bank from INUI capable of providing 3A of current. In the back is the custom board based on a Raspberry Pi PICO, which is connected to the Jetson Nano over UART.

## The software
Currently, the robot runs with ROS melodic. A custom node reads the ```/cmd_vel``` topic, computes the velocity of every motor, and sends all the information to the Pico. A simple program written in C runs on the Pico, which receives the desired motor speed and drives the h-bridge using PWM.
