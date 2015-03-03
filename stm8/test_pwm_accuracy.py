#!/usr/bin/python

import serial
import sys
import time
import csv
import os

s = serial.Serial('/dev/ttyUSB0', baudrate=38400, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=2)
assert s.isOpen()

def get_multimeter_sample():
    p = os.popen('sigrok-cli -d mastech-ms8250b:conn=/dev/ttyUSB2 --samples 1')
    s = p.read()
    p.close()
    return s.split(' ')[1]

def ser_write(data):
    for d in data:
        s.write(d)
        time.sleep(0.01)
    time.sleep(0.1)

# Clear previous buffer
while s.read() != '':
    pass

def command(cmd):
    ser_write("%s\n" % cmd)
    data = ''
    while 1:
        a = s.read()
        if a == '':
            break
        else:
            data += a
    lines = [line.strip() for line in data.split('\r')]
    return lines

def status():
    lines = command("STATUS")
    output = 'UNKNOWN'
    vin = 0
    vout = 0
    cout = 0
    constant = 'UNKNOWN'

    for line in lines:
        part = line.split(':')
        if part[0] == 'OUTPUT':
            output = part[1].strip()
        elif part[0] == 'VOLTAGE IN':
            vin = float(part[1].strip())
        elif part[0] == 'VOLTAGE OUT':
            vout = float(part[1].strip())
        elif part[0] == 'CURRENT OUT':
            cout = float(part[1].strip())
        elif part[0] == 'CONSTANT':
            constant = part[1].strip()

    return dict(output=output, vin=vin, vout=vout, cout=cout, constant=constant)

def rstatus():
    lines = command("RSTATUS")
    output = 'UNKNOWN'
    vin = 0
    vout = 0
    cout = 0
    constant = 'UNKNOWN'

    for line in lines:
        part = line.split(':')
        if part[0] == 'OUTPUT':
            output = part[1].strip()
        elif part[0] == 'VOLTAGE IN ADC':
            vin = float(part[1].strip())
        elif part[0] == 'VOLTAGE OUT ADC':
            vout = float(part[1].strip())
        elif part[0] == 'CURRENT OUT ADC':
            cout = float(part[1].strip())
        elif part[0] == 'CONSTANT':
            constant = part[1].strip()

    return dict(output=output, vin_adc=vin, vout_adc=vout, cout_adc=cout, constant=constant)


def output_on():
    command("OUTPUT 1")

def output_off():
    command("OUTPUT 0")

def voltage(v):
    command("VOLTAGE %.2f" % v)

def current(c):
    command("CURRENT %.3f" % c)


voltage(0.01)
current(0.01)
output_on()
csvf = csv.writer(sys.stdout)
csvf.writerow(['set voltage', 'read voltage', 'adc voltage', 'multimeter'])

for v in range(100, 700, 1):
    v1 = v / 100.0
    voltage(v1)
    time.sleep(2)
    t = status()
    r = rstatus()
    m = get_multimeter_sample()
    csvf.writerow([v1, t['vout'], r['vout_adc'], m])
    sys.stdout.flush()

output_off()
s.close()
