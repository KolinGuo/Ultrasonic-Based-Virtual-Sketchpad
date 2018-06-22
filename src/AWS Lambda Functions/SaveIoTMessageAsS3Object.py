from __future__ import print_function

import json
import urllib
import boto3
import struct

print('Loading function')

s3 = boto3.client('s3')

def lambda_handler(event, context):
    print("Received event: " + json.dumps(event, indent=2))

    # Get the object from the event and show its content type
    data = event['default']
    print("Received data: " + data)

    data2 = [int(x,16) for x in data.split(',')]

    download_path = '/tmp/data'

    out_file = open(download_path, "wb") # opening for [w]riting as [b]inary

    out_file.write(bytearray(data2))
    out_file.close()

    s3.upload_file(download_path, 'testforjpeg', 'ImageRGBData.bin')