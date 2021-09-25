# Sandwich
Sandwich is a small and versatile robot, powered by Jetson Nano and a Raspberry Pi PICO. It has 2 DC motors and a Raspberry Camera on the front.
![](https://raw.githubusercontent.com/tolomeis/sandwich/main/imgs/sandwich-full.jpg)
- NVIDIA Jetson Nano
-  Raspberry Pi Pico-based custom board
- 10.000mAh power bank
- Raspberry Camera v2.1 for autonomous driving
- 2x Pololu Micro gear motors
 

## What can it do?
Sandwich can operate autonomously in different modes. At any time, a custom web-based GUI named (of course) Bacon is always continuously hosted on the robot and can be used to monitor the camera and control the robot.
- ### Obstacle Avoidance:
  Sandwich uses a fully connected network, resnet18, trained on the SUN RGB-D dataset (the model is provided by the jetson-inference package) to avoid obstacles. The network runs on the NVIDIA Jetson Nano’s GPU and performs a pixel-level classification of the image (semantic segmentation). The pixels classified as “floor” are considered free and thus safe space. A custom node reads the output of the network, and by computing the centroid of the floor pixels, which is used as a reference direction, and the total forward clearance, used as an indication of the front obstacle’s distance, can compute linear and angular velocity to avoid obstacles.
- ### Apriltag Pose Control:
  In this mode, Sandwich runs the AprilTag Detection Algorithm to detect and compute the pose of a printed tag. Once acquired, the tag position and orientation are used to set a navigation goal, which will be followed by the robot using a custom Lyapunov control. Basically, it tries to arrive in front of the tag in a perpendicular direction.
- ### Beer follower:
  Sandwich uses a fine-tuned DetectNet-COCO-Bottle network to detect beer bottles and follow them. It’s a simple script that uses the angular and linear velocity to center and regulates the size of a bottle in the image acquired by the camera.
## The hardware
Sandwich is built over an old robotic kit named Axemblo, now it isn't for sale anymore, but it similar, at least on the idea, to the MakeBlock.
All the parts are made in aluminum and held together by M3 screws. On the lower part, between the aluminum racks, is the battery, a 10.000 power bank from INUI capable of providing 3A of current. In the back is the custom board based on a Raspberry Pi PICO, which is connected to the Jetson Nano over UART. 
| Component					    | Distributor|
|-------------------------------|-----------|
| Jetson NANO Developer Kit     | [Arrow](https://www.arrow.com/it-it/products/945-13450-0000-100/nvidia "Arrow")     |
| Raspberry PI Camera           | [Amazon.it](https://www.amazon.it/gp/product/B01ER2SKFS "Amazon.it") |
| Powerbank                     | [Amazon.it](https://www.amazon.it/gp/product/B07PNL5STG "Amazon.it") |
| Wifi USB adapter              | [Amazon.it](https://www.amazon.it/gp/product/B07KRCW6LZ "Amazon.it") |
| Pololu MicroGear Motors (2x)  | [Pololu.com ](https://www.pololu.com/product/3077 "Pololu.com ")      |
| Motor Driver TB6612FNG module | [Pololu.com](https://www.pololu.com/product/713 "Pololu.com")       |
| Raspberry Pi PICO             | [Melopero.com](https://www.melopero.com/shop/raspberry-pi/boards/single-boards/raspberry-pi-pico/ "Melopero.com")       |
| MicroSD card  64GB            | [Amazon.it](https://www.amazon.it/gp/product/B08GYBBBBH "Amazon.it") |
| Noctua NF-A4x20 5V FAN		| [Amazon.it ](https://www.amazon.it/gp/product/B071FNHVXN "Amazon.it ")|

![](https://raw.githubusercontent.com/tolomeis/sandwich/main/imgs/schematic.png)





## Basically, how does it work:
Currently, the robot runs with ROS melodic. A custom node reads the ```/cmd_vel``` topic, computes the velocity of every motor, and sends all the information to the Pico. A simple program written in C runs on the Pico, which receives the desired motor speed and drives the h-bridge using PWM. The web GUI is based on ```rosbridge_suite``` and the ```rosjs``` library; it is capabe of controlling the robot using a joystick and visualizing the camera stream; either on desktop or mobile devices. 
![](https://raw.githubusercontent.com/tolomeis/sandwich/main/imgs/bacon.png)

### Camera side
The camera stream is achieved using a couple of nodes on the Jetson. First there is the ```video_source``` node from the ```ros_deep_learning``` package provided by NVIDIA. It acquires the camera and publish the raw images on the ```video_source/raw``` topic.  The ```web_video_server``` node, then, opens the stream on the ```11315``` port. On the GUI web page the stream is loaded as an image, using the url of the video stream provided by the server and the correct topic, that is something like
```html
 <img src="http://robot_address:11315/stream?topic=/video_source/raw&width=800&height=600&quality=50"/>
```
A simple javascript function which read the textbox containing the robot address and load the stream is called when pressed the "start camera" button
```javascript
// load the camera stream as an image, with the right IP address (from the wp_address input box)
//port 11315 and topic /video_source/raw are fixed.
//TODO: make port, topic and parameters dynamic via input boxes
loadCamera = function(){
 document.getElementById('cam').innerHTML = '<img src="http://' + app.ws_address.slice(5,-5) + ':11315/stream?topic=/video_source/raw&width=800&height=600&quality=50" class="w3-image"/>';
 app.cstarted = true; //"started" flag for change the button appereance
}




### Motor control side
To control the motors, Bacon uses ```rosjs```  library together with a ```nipple.js``` script. The joystick is based on a tutorial from "msadowski" [that you can find here](https://msadowski.github.io/ros-web-tutorial-pt1/ "here"). The webpage make also use of Vue js to better link evey action to the html elements. 

