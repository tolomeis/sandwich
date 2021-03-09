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
Currently, the robot runs with ROS melodic. A custom node reads the ```/cmd_vel``` topic, computes the velocity of every motor, and sends all the information to the Pico. A simple program written in C runs on the Pico, which receives the desired motor speed and drives the h-bridge using PWM. There is a web GUI, called Bacon, based on ```rosbridge_suite``` and the ```rosjs``` library; it is capabe of controlling the robot using a joystick and visualizing the camera stream; either on desktop or mobile devices. 
![](https://raw.githubusercontent.com/tolomeis/sandwich/main/imgs/bacon.png)

All the nodes required for the GUI are started via a single ```bacon.launch``` file, which is indipendent fron the main nodes and launch files.
#### bacon.launch
```xml
<launch>
	<node pkg="web_video_server" type="web_video_server" name="web_video_server" output="screen">
		<param name="quality" value="50"/>
		<param name="port" value="11315"/>
	</node>
	<include file="$(find rosbridge_server)/launch/rosbridge_websocket.launch"/>
</launch>
```

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
```



### Motor control side
TODO


