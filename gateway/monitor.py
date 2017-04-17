import os
import sys
from influxdb import InfluxDBClient
from serial import Serial
import json
import logging as log
from logging import DEBUG, INFO
import datetime

DEFAULT_DEVICE = '/dev/tty.usbmodem1451'
LOG_FORMAT = '%(asctime)s %(levelname)s: %(message)s'

def processTalk(influx_client, talk):
    print (talk)
    type = talk[0].split("/")[1]
    node = talk[0].split("/")[0]
    measurement = talk[0].split("/")[3]
    value = talk[1]

    json_body = [
        {
            "measurement": type,
            "tags": {
                "node": node
            },
            "fields": {
                measurement: value
            }
        }
    ]

    influx_client.write_points(json_body)

def main():
    log.basicConfig(level=INFO, format=LOG_FORMAT)

    serial = Serial(DEFAULT_DEVICE, timeout=3.0)
    serial.write(b'\n')

    client = InfluxDBClient('localhost', 8086, '', '', 'uclClimateStation')

    while True:
        line = serial.readline()
        if line:
            log.debug(line)
            try:
                talk = json.loads(line.decode())
                log.info(line)
                processTalk(client, talk)

            except Exception as e:
                log.error('Received malformed message: %s', line)

if __name__ == '__main__':
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
    except Exception as e:
        log.error(e)
        if os.getenv('DEBUG', False):
            raise e
        sys.exit(1)
