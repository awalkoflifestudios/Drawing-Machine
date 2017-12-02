import sys,os
import serial.tools.list_ports
import serial

# linux/windows
print "os.name %s", os.name
if os.name == 'nt':
    import msvcrt  # windows
    # import win32com.client
elif os.name == 'posix':
    import tty # linux
else:
    print "What os: %s" % os.name
    exit(1)


def getchar():
    if os.name == 'nt':
        return msvcrt.getch()
    else:
        return sys.stdin.read(1)

#if os.name =='nt':
#    print "Ports ",[ usb.DeviceID for usb in wmi.InstancesOf("Win32_USBHub") ]
#    exit(0)
#elif os.name == 'posix':
if True:
    print "Ports ",[port.name for port in serial.tools.list_ports.comports()]
    default_port = serial.tools.list_ports.comports()[0].name
    print "Which port? [%s] " % default_port,

want_port = sys.stdin.readline().rstrip()
if want_port == '':
    want_port = default_port

# 'list index out of range' means bad port name
prefix = '/dev/' if os.name=='posix' else ''
serial_port = serial.Serial( prefix + want_port, 9600 ) # [port for port in serial.tools.list_ports.comports() if port.name == want_port][0] )

if os.name == 'posix':
    tty.setraw(sys.stdin.fileno())

while (1):
    c = getchar()
    if c == '\x03':
        exit(0)
    elif ord(c) != 255: # windows returns 255 for "no char"
        serial_port.write(c)
        print "'%c'" % c,ord(c)
