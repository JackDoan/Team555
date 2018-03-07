# STMicroelectronics
#----------------------------------------------------------------------------------
# DO NOT CHANGE FOLLOWING LINES!
# Any change of following code can result in malfuncion in scripting environment.
# Customization of this file is NOT SUPPORTED.
#----------------------------------------------------------------------------------

import array
import clr
import sys
import time

sys.path.insert(0, "C:/AirHockeyRobot/python")

clr.AddReference("System.Windows.Forms")
clr.AddReference("xSpinFamily")
clr.AddReference("mscorlib.dll")

from xSpinFamily import *
from System.Threading import Timer
from System.Windows.Forms import DialogResult, MessageBoxButtons, MessageBoxIcon, MessageBox
from time import sleep
  
class Capsule:
    Verbose = False
    WaitBUSYen = False
    Chain = None
    
    TimeoutValue = 60000
    TimoutExpired = False
    
    def __init__(self, ch):             # Constructor takes no argument
        self.Chain = ch         # clear the chain variable
        
    # Timeout expiration event
    def __t_Tick(self, *args):
        #if (self.Verbose):    print "Tick!"
        self.TimoutExpired = True
        self.__t.Dispose()
    
    def StartTimeout(self):
        self.TimoutExpired = False
        self.__t = Timer(self.__t_Tick, None, self.TimeoutValue * 1000, 0)
        if (self.Verbose):    print "Timeout timer started"
    
    def StopTimeout(self):
        self.__t.Dispose()
        self.TimoutExpired = False
        if (self.Verbose):    print "Timeout timer stopped"

# Python interface to xSPINFamily pack
#----------------------------------------------------------------------------------

# Instantiate the capsule
_C = Capsule(None)
        
# Init the capsule variables
def SetChain(c):
    """"""
    _C.Chain = c
    #print "Initialization completed"

def Verbose(*args):
    """Sets or clears the verbose function execution mode
       If no argument are present, the the acual setting is returned."""
    if(len(args) == 0):
        pass
    elif(len(args) == 1):
        _C.Verbose = args[0]
        print "Verbose mode set", _C.Verbose
    return _C.Verbose

def AutoWaitBUSY(*args):
    """Sets or clears the automatic BUSY synchronization execution mode
       If no argument are present, the the acual setting is returned."""
    if(len(args) == 0):
        pass
    elif(len(args) == 1):
        _C.WaitBUSYen = args[0]
        if _C.Verbose:    print "BUSY synchronization set", _C.WaitBUSYen
    return _C.WaitBUSYen
    
def Delay(msDelay):
    """Waits for the specified ms delay elapsed"""
    if _C.Verbose:  print ("%d ms delay" % msDelay)
    sleep(msDelay/1000) # Sleep takes seconds as argument"
    
def StopMessage(message):
    """Exits the script and show the specified message in the output text box"""
    raise Exception(message)

def SendnReceive():
    """Performs a send/receive cycle in the SPI chain"""
    _C.Chain.Send()
    _C.Chain.Receive()

def GetNdevices():
    """Returns the number of chain connected devices"""
    return _C.Chain.DevCount

def ConfirmPopup(message, caption):
    """Display a message box to get the user to confirm a question and returns the user answer"""
    if (MessageBox.Show(message, caption, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes):
        return True
    else:
        return False

def WaitBUSY(*args):
    """Wait until the BUSY line is released.
       If an argument is specified, it define the maximum timeout in seconds.
       If no argument is specified the maximum timeout is imposed (1 minute).
       Timeout event will make the script to be aborted."""
    if (len(args) == 0):
        _C.TimeoutValue = 60
    elif(len(args) >= 1):
        _C.TimeoutValue = args[0]
    
    # Starts timeout timer
    _C.StartTimeout()
    if _C.Verbose:   print "Waiting for BUSY line (Timeout @ %d seconds)..." % _C.TimeoutValue
    while(_C.Chain.BUSY):
        if (_C.TimoutExpired):
        # Timeout is expered --> error
            if _C.Verbose:   print "... WaitBUSY timeout"
            raise Exception("WaitBUSY timeout")
    # Stops timeout timer
    _C.StopTimeout()
    if _C.Verbose:   print "... BUSY line released"
    
# Commands
#----------------------------------------------------------------------------------

def NOP(dev):
    """No OPeration
        - dev: zero-based index of the target device"""
    if _C.Verbose:  print "NOP - device:", dev
    _C.Chain[dev].NOP()
    SendnReceive()
    
def SetParam(dev, addr, val):
    """Sends SetParam command to target device
        - dev: zero-based index of the target device
        - addr: register address or mnemonic string
        - val: value"""
    if _C.Verbose:   print "SetParam - device:", dev, "param:", addr, " value: %#08X" % val
    _C.Chain[dev].SetParam(addr, val)
    SendnReceive()

def GetParam(dev, addr):
    """Sends GetParam command to target device
        - dev: zero-based index of the target device
        - addr: register address or mnemonic string
    Returns the register value"""
    if _C.Verbose:   print "GetParam - device:", dev, "param:", addr
    _C.Chain[dev].GetParam(addr)
    SendnReceive()
    if _C.Verbose:   print "Returned value:", _C.Chain[dev][addr].RegValueStr()
    return _C.Chain[dev][addr].Value

def Run(dev, dir, spd):
    """Sets the target speed and the motor direction of the dev
        - dev: zero-based index of the target device
        - dir: True - forward, False - reverse
        - spd: speed"""
    
    if _C.Verbose:   print "Run - device:", dev, "dir:", dir, "speed:", spd
    _C.Chain[dev].Run(spd, dir)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def StepClock(dev, dir):
    """Puts the dev in step clock mode and impose dir direction
        - dev: zero-based index of the target device
        - dir: True - forward, False - reverse"""
    if _C.Verbose:   print "StepClock - device:", dev, "dir:", dir
    _C.Chain[dev].StepClock(dir)
    SendnReceive()
    
def Move(dev, dir, nstep):
    """Makes nstep (micro)steps in dir direction
        - dev: zero-based index of the target device
        - dir: True - forward, False - reverse
        - nstep: number of steps to perform"""
    if _C.Verbose:   print "Move - device:", dev, "dir:", dir, "n.step: %#08X" % nstep
    _C.Chain[dev].Move(nstep, dir)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def GoTo(dev, targetpos):
    """Brings motor in targetpos position (minimum path)
        - dev: zero-based index of the target device
        - targetpos: target absolute position"""
    if _C.Verbose:   print "GoTo - device:", dev, "target pos.: %#08X" % targetpos
    _C.Chain[dev].GoTo(targetpos)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()

def GoTo_DIR(dev, dir, targetpos):
    """Brings motor in targetpos position forcing dir direction
        - dev: zero-based index of the target device
        - dir: True - forward, False - reverse
        - targetpos: target absolute position"""
    if _C.Verbose:   print "GoTo_DIR - device:", dev, "dir:", dir, "target pos.: %#08X" % targetpos
    _C.Chain[dev].GoTo(targetpos, dir)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def GoUntil(dev, act, dir, spd):
    """Performs a motion in dir direction with speed spd until SW is closed,
    then the act action is executed and a SoftStop takes place
        - dev: zero-based index of the target device
        - dir: True - forward, False - reverse
        - act: True - reset ABS_POS register, False - store ABS_POS value into MARK register
        - spd: target speed"""
    if _C.Verbose:   print "GoUntil - device:", dev,  "act:", act, "dir:", dir, "speed:", spd
    _C.Chain[dev].GoUntil(spd, act, dir)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()

def ReleaseSW(dev, act, dir):
    """Performs a motion in dir direction with minimum speed until SW is open,
    then the act action is executed and a HardStop takes place
        - dev: zero-based index of the target device
        - dir: True - forward, False - reverse
        - act: True - reset ABS_POS register, False - store ABS_POS value into MARK register"""
    if _C.Verbose:   print "ReleaseSW - device:", dev, " act:", act, " dir:", dir
    _C.Chain[dev].ReleaseSW(act, dir)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def GoHome(dev): 
    """Brings the motor in HOME position
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "GoHome - device:", dev
    _C.Chain[dev].GoHome()
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def GoMark(dev):
    """Brings the motor in MARK position
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "GoMark - device:", dev
    _C.Chain[dev].GoMark()
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def ResetPos(dev):
    """Resets the targetpos register (set HOME position)
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "ResetPos - device:", dev
    _C.Chain[dev].ResetPos()    
    SendnReceive()
    
def ResetDevice(dev):
    """Target device is reset to power-up conditions
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "ResetDevice - device:", dev
    _C.Chain[dev].ResetDevice()    
    SendnReceive()
    
def SoftStop(dev):
    """Stops the motor after a deceleration phase
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "SoftStop - device:", dev
    _C.Chain[dev].Stop(False, False)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def HardStop(dev):
    """Immediately stops the motor 
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "HardStop - device:", dev
    _C.Chain[dev].Stop(True, False)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def SoftHiZ(dev):
    """Disables the power bridges after a deceleration phase
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "SoftHiZ - device:", dev
    _C.Chain[dev].Stop(False, True)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()

def HardHiZ(dev):
    """Immediately disables the power bridges
        - dev: zero-based index of the target device"""
    if _C.Verbose:   print "HardHiZ - device:", dev
    _C.Chain[dev].Stop(True, True)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()
    
def Enable(dev, target):
    """Enable the target bridge
        - dev: zero-based index of the target device
        - target: Bridge to enable (0 or 1, 'a' or 'b')"""
    if _C.Verbose:   print "Enable - device:", dev, "target : ", target
    _C.Chain[dev].Enable(target)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()

def Disable(dev, target):
    """Disbale the target bridge
        - dev: zero-based index of the target device
        - target: Bridge to enable (0 or 1, 'a' or 'b')"""
    if _C.Verbose:   print "Enable - device:", dev, "target : ", target
    _C.Chain[dev].Disable(target)
    SendnReceive()
    if _C.WaitBUSYen:   WaitBUSY()


def GetStatus(dev):
    """Returns the status register value
        - dev: zero-based index of the target device
    Returns the STATUS register value"""
    if _C.Verbose:   print "GetStatus - device:", dev
    _C.Chain[dev].GetStatus()
    SendnReceive()
    
    if _C.Verbose: print _C.Chain[dev]["STATUS"].RegValueStr()
    return _C.Chain[dev]["STATUS"].Value

def SPIByte(dev, b):
    """Transmit one byte through the SPI
        - dev: zero-based index of the target device
        - b: byte"""
    if _C.Verbose:   print "Send %#010X" % b, "to device", dev
    _C.Chain[dev].AddTXPack(b)
    SendnReceive()

def BUSY():
    """Reads the status of the BUSY line of the chain
    Returns True if BUSY line is low, otherwise it returns False"""
    res = _C.Chain.BUSY
    if _C.Verbose:   print "BUSY - %s" % res
    return res

def FLAG():
    """Reads the status of the FLAG line of the chain
    Returns True if FLAG line is low, otherwise it returns False"""
    res = _C.Chain.FLAG
    if _C.Verbose:   print "FLAG - %s" % res
    return res

def RESET(*args):
    """Sets or reads the RESET line:
    True <---> RESET @ 0, False <---> RESET @ 1
    Returns True if RESET line is low, otherwise it returns False"""
    if(len(args) == 0):
        if _C.Verbose:   print "RESET reading"
        pass
    elif(len(args) == 1):
        if _C.Verbose:   print "RESET writing @ %s" % args[0]
        _C.Chain.RESET = args[0]
    # read the RESET line status
    return _C.Chain.RESET  
    
def LoadConfiguration(dev, path):
    """Loads a configuration file
        - dev: zero-based index of the target device
        - path: file path"""
    if _C.Verbose:   print "Loading configuration from", path, "file"
    _C.Chain[dev].LoadDeviceSetup(path)
    for reg in _C.Chain[dev].RegisterMap:
        if (reg.ReadOnly == False):    _C.Chain[dev].SetParam(reg.Address, reg.Value)
    SendnReceive()

def SaveConfiguration(dev, path):
    """Saves a configuration file
        - dev: zero-based index of the target device
        - path: file path"""
    if _C.Verbose:   print "Saving configuration to", path, "file"
    for reg in _C.Chain[dev].RegisterMap:
        if (reg.ReadOnly == False):    _C.Chain[dev].GetParam(reg.Address)
    SendnReceive()
    _C.Chain[dev].SaveDeviceSetup(path)