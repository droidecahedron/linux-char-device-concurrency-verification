
1) Compile driver module : $ make

2) Load module : $ sudo insmod asp.ko NUM_DEVICES=<num_devices>

3) Test driver :
	1) Compile userapp : $ make app
	2) Run userapp : $ sudo ./userapp <device_number>
		where device_number identifies the id number of the device to be tested.   

	Note : userapp has to be executed with sudo privilege as the device files
		   in /dev/ are created in the driver with root privileges.
		   
4) Unload module : $ sudo rmmod asp

CHANGELOG OF DEVICE DRIVER
static int majorNo = 511 
I cannot have a majorNo larger than 700 on my OS no matter what I try.




Deadlocks : 
the deadlocks are described in print statements in the terminal when you run the user app.

On my system, the deadlocks require a system restart as even pgrep->kill SIGVAL PID wouldn't get rid of the process
correctly.


Data Races (or lack thereof): 
1: data race
mode 2
thread a reads from device
thread b writes to device

both access ramdisk of the device
synch device : sem1

the result of the read/write depends on the order of execution of the threads, have accomplished it a lot in testing.


2: safe
Pair of critical region : 
Both access devc->mode
Line 159 in ioctl: devc->mode = MODE2; (WRITE)
Line 111 in write: if (devc->mode == MODE1) (READ)
Flags held at time : sem1
There won't be a data race between these two lines of code because sem1 is always grabbed before a read or write occurs,
ensuring they are mutually exclusive.


3: data race
Pair of critical region :
Both access devc->ramdisk 
Line 117 in write: ret = count - copy_from_user(devc->ramdisk, buf, count); (WRITE)
Line 90 in read :  ret = count - copy_to_user(buf, devc->ramdisk, count); (READ)

both functions grab the flag when checking the mode, but at the moment of both of these lines no flags are held since they both release it, so the order of execution will affect the result.

4: safe
Pair of critical region 
Line 47 in open: if (devc->mode == MODE1) (READ)
line 159 in ioctl : devc->mode = MODE2;
Flags held at time : sem1
There won't be a data race between these two lines of code because sem1 is always grabbed before a read or write occurs,
ensuring they are mutually exclusive.



