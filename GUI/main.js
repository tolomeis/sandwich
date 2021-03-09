var app = new Vue({
    el: '#app',
    // storing the state of the page
    data: {
        connected: false,
		cstarted: false,
        ros: null,
        ws_address: 'ws://192.168.1.203:9090',
        logs: [],
    },
    // helper methods to connect to ROS
    methods: {
        connect: function() {
            this.logs.unshift('connect to rosbridge server!!')
            this.ros = new ROSLIB.Ros({
                url: this.ws_address
            })
            this.ros.on('connection', () => {
                this.connected = true
                this.logs.unshift('Connected!')
                // console.log('Connected!')
            })
            this.ros.on('error', (error) => {
                this.logs.unshift('Error connecting to websocket server')
                // console.log('Error connecting to websocket server: ', error)
            })
            this.ros.on('close', () => {
                this.connected = false
                this.logs.unshift('Connection to websocker server closed')
                // console.log('Connection to websocket server closed.')
            })

        },
        disconnect: function() {
            this.ros.close()
        },
		setTopic: function() {
            this.topic = new ROSLIB.Topic({
                ros: this.ros,
                name: '/cmd_vel',
                messageType: 'geometry_msgs/Twist'
            })
        },
		stop: function() {
            this.message = new ROSLIB.Message({
                linear: { x: 0, y: 0, z: 0, },
                angular: { x: 0, y: 0, z: 0, },
            })
            this.setTopic()
            this.topic.publish(this.message)
        },
		move: function(linear, angular) {
            this.message = new ROSLIB.Message({
                linear: { x: linear, y: 0, z: 0, },
                angular: { x: 0, y: 0, z: angular, },
            })
            this.setTopic()
            this.topic.publish(this.message)
        },
    },
})