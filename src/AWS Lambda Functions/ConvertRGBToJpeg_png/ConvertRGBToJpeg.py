from __future__ import print_function

import json
import urllib
import boto3
import uuid
import numpy as np
import cv2
import struct

print('Loading function')

s3 = boto3.client('s3')
img = np.zeros((32,32,3),np.uint8)

def lambda_handler(event, context):

    # Get the object from the event and show its content type
    bucket = event['Records'][0]['s3']['bucket']['name']
    key = urllib.unquote_plus(event['Records'][0]['s3']['object']['key'].encode('utf8'))
	
    # Download to a tmp file named 'test.txt'
    download_path = '/tmp/{}{}'.format(uuid.uuid4(), key)
    s3.download_file(bucket, key, download_path)
	
    # Upload to a tmp file named 'jpeg-test.png'
    upload_path = '/tmp/jpeg-{}{}'.format(key, '.png')
	
    in_file = open(download_path, "rb") # opening for [r]eading as [b]inary

    for row in range(32):
	for col in range(32):
            img[row][col][0] = struct.unpack('B',in_file.read(1))[0] # BLUE
            img[row][col][1] = struct.unpack('B',in_file.read(1))[0] # GREEN
            img[row][col][2] = struct.unpack('B',in_file.read(1))[0] # RED
            #in_file.read(1) # extra 0 not needed

    in_file.close()

    cv2.imwrite(upload_path, img)

    s3.upload_file(upload_path, '{}jpeg'.format(bucket), 'image.png')
	
    # try:
        # response = s3.get_object(Bucket=bucket, Key=key)
        # print("CONTENT TYPE: " + response['ContentType'])
        # return response['ContentType']
	
    # except Exception as e:
        # print(e)
        # print('Error getting object {} from bucket {}. Make sure they exist and your bucket is in the same region as this function.'.format(key, bucket))
        # raise e
