import os
import serial
import requests
import time
from flask import Flask, flash, request, redirect, url_for,Response,json
from werkzeug.utils import secure_filename

app = Flask(__name__)
#arduino = serial.Serial('/dev/ttyACM0',9600)
@app.route('/direction',methods=['GET','POST'])
def upload_file():
	if request.method == 'POST':
		#file = request.files['direction']
		value = request.form.get('direction')
		print ('get direction : '+value)
	
		if value =='8' : #up
			print('up')		
			time.sleep(1)
			op ='rosrun knu_ros_lecture turtle_position_move 0 0.0005'
	
			print('up finished')

			os.system(op)
			print("here up")

			op = "rostopic pub /cmd_vel geometry_msgs/Twist '[0.0, 0.0, 0.0]' '[0.0, 0.0, 0.0]'"
			os.system(op)
			print("here")
		elif value =='2' : # down
			print('down')
			time.sleep(1)
			op ='rosrun knu_ros_lecture turtle_position_move 0 -0.0005'
			
			print('down finished')

			os.system(op)
			time.sleep(1)
			op = "rostopic pub /cmd_vel geometry_msgs/Twist '[0.0, 0.0, 0.0]' '[0.0, 0.0, 0.0]'"

			os.system(op)
		elif value == '4':
			print('left')
			time.sleep(1)
			op ='rosrun knu_ros_lecture turtle_position_move 10 0'
			
			print('left finish')
			os.system(op)
			time.sleep(1)
			op = "rostopic pub /cmd_vel geometry_msgs/Twist '[0.0, 0.0, 0.0]' '[0.0, 0.0, 0.0]'"
			os.system(op)
		elif value == '6': # right
			print('right')

			time.sleep(1)
			
			op ='rosrun knu_ros_lecture turtle_position_move -10 0'
			print('right finish')
			os.system(op)
			time.sleep(1)
			op = "rostopic pub /cmd_vel geometry_msgs/Twist '[0.0, 0.0, 0.0]' '[0.0, 0.0, 0.0]'"
			os.system(op)
		elif value == '102':
			print('auto race')
			op ='rosrun knu_ros_lecture turtle_ca'
			os.system(op)
 
		elif value == '1':
			arduino.write(str(1).encode())


		#UPLOAD_FOLDER is not a configuration option recongnized by Flask
		
		#Replace ':	
		return 'OK'
	
if __name__=='__main__':
	app.run(host='0.0.0.0',port=5000,debug=True)
