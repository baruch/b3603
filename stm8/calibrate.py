#!/usr/bin/python

import serial
import sys
import time
import csv
import os
import math

def calc_average(data):
    return reduce(lambda x,y: x+y, data) / len(data)

def calc_stddev(data, avg):
    tmp1 = map(lambda x: (x-avg)*(x-avg), data)
    tmp2 = calc_average(tmp1)
    return math.sqrt(tmp2)

class B3603(object):
    def __init__(self, portname):
        self.portname = portname
        self.debug = False

    def open(self):
        self.s = serial.Serial(self.portname, baudrate=38400, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=0.2)
        if self.s.isOpen():
            self.clear_input()
            s = self.model()
            print 'OPEN "%s"' % s
            if s  == 'MODEL: B3603':
                return True
            else:
                print 'Couldnt read the right model out of the serial port, got "%s", expected to see "MODEL: B3603"' % s
        else:
            return False

    def close(self):
        self.s.close()

    def ser_write(self, data):
        if self.debug:
            print 'DEBUG OUT:', data
        for d in data:
            self.s.write(d)

    def clear_input(self):
        # Clear previous buffer
        while self.s.read() != '':
            pass

    def command(self, cmd):
        self.ser_write("%s\n" % cmd)
        data = ''
        while 1:
            a = self.s.read()
            if a == '':
                break
            else:
                data += a
        lines = [line.strip() for line in data.split('\r')]
        if self.debug:
            for line in lines:
                print 'DEBUG IN', line
        return lines

    def model(self):
        return self.command('MODEL')[0]

    def status(self):
        lines = self.command("STATUS")
        output = 'UNKNOWN'
        vin = 0
        vout = 0
        cout = 0
        constant = 'UNKNOWN'

        for line in lines:
            part = line.split(':')
            if part[0] == 'OUTPUT':
                output = part[1].strip()
            elif part[0] == 'VIN':
                vin = float(part[1].strip())
            elif part[0] == 'VOUT':
                vout = float(part[1].strip())
            elif part[0] == 'COUT':
                cout = float(part[1].strip())
            elif part[0] == 'CONSTANT':
                constant = part[1].strip()

        return dict(output=output, vin=vin, vout=vout, cout=cout, constant=constant)

    def rstatus(self):
        lines = self.command("RSTATUS")
        output = 'UNKNOWN'
        vin = 0
        vout = 0
        cout = 0
        vin_calc = 0
        vout_calc = 0
        cout_calc = 0
        constant = 'UNKNOWN'

        for line in lines:
            part = line.split(':')
            if part[0] == 'OUTPUT':
                output = part[1].strip()
            elif part[0] == 'VIN':
                vin_calc = float(part[1].strip())
            elif part[0] == 'VOUT':
                vout_calc = float(part[1].strip())
            elif part[0] == 'COUT':
                cout_calc = float(part[1].strip())
            elif part[0] == 'VIN ADC':
                vin = float(part[1].strip())
            elif part[0] == 'VOUT ADC':
                vout = float(part[1].strip())
            elif part[0] == 'COUT ADC':
                cout = float(part[1].strip())
            elif part[0] == 'CONSTANT':
                constant = part[1].strip()

        return dict(output=output, vin_adc=vin, vout_adc=vout, cout_adc=cout, constant=constant, vin_calc=vin_calc, vout_calc=vout_calc, cout_calc=cout_calc)


    def output_on(self):
        return self.command("OUTPUT 1")

    def output_off(self):
        return self.command("OUTPUT 0")

    def voltage(self, v):
        lines = self.command("VOLTAGE %.2f" % v)
        pwm_vout = None
        pwm_cout = None
        for line in lines:
            word = line.split(' ')
            if word[0] != 'PWM' and word[0] != 'aWM': continue
            if word[1] == 'VOLTAGE': pwm_vout = float(word[2])
            if word[1] == 'CURRENT': pwm_cout = float(word[2])
        return (pwm_vout, pwm_cout)

    def current(self, c):
        return self.command("CURRENT %.3f" % c)

class Multimeter(object):
    def __init__(self, portname, model):
        self.portname = portname
        self.model = model

    def open(self):
        return self._sample() != None

    def _sample(self):
        p = os.popen('sigrok-cli -d %s:conn=%s --samples 1' % (self.model, self.portname))
        s = p.read()
        p.close()
        return float(s.split(' ')[1])

    def sample1(self, count):
        count = int(count)
        if count < 1:
            raise Exception("Invalid count value, must be above 0")
        if count == 1:
            return self._sample()

        data = []
        for i in xrange(count):
            data.append(self._sample())
            if data[-1] == None:
                return None

        avg = calc_average(data)
        stddev = calc_stddev(data, avg)
        if stddev > 0.1:
            print 'Multimeter samples vary too much, stddev=%f, data:' % stddev, data
            return None
        return avg

    def sample3(self, count):
        for i in range(3):
            s = self.sample1(count)
            if s != None:
                return s
            print 'Failed to read stable value, trying again, maybe'
            time.sleep(1)
        return None

def lse(xdata, ydata):
    assert(len(xdata) == len(ydata))
    sum_xy = 0
    sum_x = 0
    sum_y = 0
    sum_x2 = 0
    n = len(xdata)
    for i in xrange(n):
        x_i = xdata[i]
        y_i = ydata[i]
        sum_xy += x_i*y_i
        sum_x += x_i
        sum_y += y_i
        sum_x2 += x_i*x_i

    alpha = (n * sum_xy - sum_x*sum_y) / (n * sum_x2 - sum_x*sum_x)
    beta = (sum_y - alpha * sum_x) / n

    return (alpha, beta)

def auto_calibration():
    psu = B3603(sys.argv[2])
    if not psu.open():
        print 'Failed to open serial port to B3603 on serial %s' % sys.argv[2]
        return

    dmm = Multimeter(sys.argv[3], sys.argv[4])
    if not dmm.open():
        print 'Failed to open serial port to multimeter on serial %s model %s' % (sys.argv[3], sys.argv[4])
        psu.close()
        return

    vin = psu.status()['vin']
    NUM_STEPS = 20
    MIN_VOLTAGE = 1.0
    MAX_VOLTAGE = vin - 1.0
    STEP_SIZE_INT = int(100 * (MAX_VOLTAGE - MIN_VOLTAGE) / NUM_STEPS)
    STEP_SIZE = STEP_SIZE_INT / 100.0
    print 'PSU Input voltage is %s, will use %d steps between %s and %s' % (vin, NUM_STEPS, MIN_VOLTAGE, MAX_VOLTAGE)

    if STEP_SIZE < 0.01:
        print 'Step size is below 0.1, cannot test'
        return

    psu.output_on()
    psu.current(1.0)
    psu.voltage(MIN_VOLTAGE)

    pwm_data = []
    adc_data = []
    vout_data = []
    valid = True

    for step in xrange(NUM_STEPS):
        volt = MIN_VOLTAGE + step * STEP_SIZE
        print 'Setting voltage to', volt
        (pwm_vout, pwm_cout) = psu.voltage(volt)
        # Wait 1 second for things to stabilize
        time.sleep(1)
        vout = dmm.sample3(3) # Use three samples
        if vout == None:
            print 'Failed to get vout'
            valid = False
            break
        if vout < 0.1:
            print 'Vout is too low (%s), something broke, try to reset the B3603 to defaults with RESTORE command first' % vout
            valid = False
            break
        if vin - vout < 0.5:
            print 'Vout is %s and Vin is %s, this means that pwm calibration is saturated and the test will be meaningless' % (vout, vin)
            valid = False
            break
        rstatus = psu.rstatus()
        adc_vout = rstatus['vout_adc']
        vout_calc = rstatus['vout_calc']

        pwm_data.append(pwm_vout)
        adc_data.append(adc_vout)
        vout_data.append(int(vout*1000))
        print 'Step %d Set voltage %f Read voltage %f PWM %s ADC %s (%s)' % (step, volt, vout, pwm_vout, adc_vout, vout_calc)

    print psu.output_off()

    if not valid:
        print 'Test is invalid, calibration cancelled'
        return

    print 'ADC'
    val = lse(adc_data, vout_data)
    adc_a = int(val[0]*65536)
    adc_b_tmp = val[1]
    if adc_b_tmp < 0:
        adc_b_tmp = -adc_b_tmp
    else:
        print 'Expected ADC_B to be negative... for some reason it\'ts not'
        adc_b_tmp = 0
    adc_b = int(adc_b_tmp*65536)
    print val, adc_a, adc_b
    print psu.command('CALVOUTADCA %d' % adc_a)
    print psu.command('CALVOUTADCB %d' % adc_b)
    print
    print 'PWM'
    val = lse(vout_data, pwm_data)
    pwm_a = int(val[0]*65536)
    pwm_b = int(val[1]*65536)
    print val, pwm_a, pwm_b
    print psu.command('CALVOUTPWMA %d' % pwm_a)
    print psu.command('CALVOUTPWMB %d' % pwm_b)

    psu.close()

def manual_calibration():
    print 'Not implemented'

def usage():
    print '%s -a <b3603 serial> <multimeter serial> <multimeter model>' % sys.argv[0]
    print
    print 'or:'
    print
    print '%s -m <b3603 serial>' % sys.argv[0]

def main():
    if len(sys.argv) < 2:
        return usage()

    if sys.argv[1] == '-a':
        if len(sys.argv) != 5:
            return usage()
        else:
            auto_calibration()

    if sys.argv[1] == '-m':
        if len(sys.argv) != 3:
            return usage()
        else:
            manual_calibration()

if __name__ == '__main__':
    main()
