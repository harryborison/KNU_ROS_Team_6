import os
import requests
from flask import Flask, flash, request, redirect, url_for,Response,json
from werkzeug.utils import secure_filename

app = Flask(__name__)

@app.route('/direction',methods=['GET','POST'])
def upload_file():
	if request.method == 'POST':
		#file = request.files['direction']
		print ('get direction : '+request.form.get('direction'))
		#UPLOAD_FOLDER is not a configuration option recongnized by Flask
		
		#Replace ':	
		return 'OK'
	
if __name__=='__main__':
	app.run(host='0.0.0.0',port=5000,debug=True)
