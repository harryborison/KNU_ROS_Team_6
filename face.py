import os
import sys
sys.path.remove('/opt/ros/kinetic/lib/python2.7/dist-packages')
import dlib, cv2, os
from imutils import face_utils
import numpy as np
import matplotlib.pyplot as plt

flag = 0

detector = dlib.cnn_face_detection_model_v1('/home/mystery/rosgit/KNU_ROS_Team_6/dogHeadDetector.dat')
predictor = dlib.shape_predictor('/home/mystery/rosgit/KNU_ROS_Team_6/landmarkDetector.dat')

img_path = '/home/mystery/rosgit/KNU_ROS_Team_6/1.jpg'
filename, ext = os.path.splitext(os.path.basename(img_path))
img = cv2.imread(img_path)
img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
# img = cv2.resize(img, dsize=None, fx=0.5, fy=0.5)

plt.figure(figsize=(16, 16))
#print("강아지인지 판별중입니다")
dets = detector(img, upsample_num_times=1)
img_result = img.copy()
for i, d in enumerate(dets):
    print("Detection {}: Left: {} Top: {} Right: {} Bottom: {} Confidence: {}".format(i, d.rect.left(), d.rect.top(), d.rect.right(), d.rect.bottom(), d.confidence))
    x1, y1 = d.rect.left(), d.rect.top()
    x2, y2 = d.rect.right(), d.rect.bottom()

    cv2.rectangle(img_result, pt1=(x1, y1), pt2=(x2, y2), thickness=2, color=(255,0,0), lineType=cv2.LINE_AA)
    flag = 1

plt.figure(figsize=(16, 16))

if flag == 1:
	print("자동주행을 종료합니다")
	result = os.popen('ps -a | grep turtle_ca').read()
	print(result)
	list = result.split(' ')

	os.system('kill '+ list[0])
	os.system('rosrun knu_ros_lecture turtle_position_move 0 0')
	print("강아지를 쫓아갑니다")
	#os.system('roslaunch turtlebot3_follow_filter turtlebot3_follow_filter.launch &')
	os.system('roslaunch turtlebot3_follower turtlebot3_follower.launch')
