# Character Device Driver 
Loadable Kernel Modules, file I/O, and how to implement a basic device driver.
This assignment consists of three parts.  First, you will compile, install, and run a simple Loadable Kernel Module (LKM).  Second, you will write a user-space program that takes commands from the user to read, write and seek on a file.  You will use this program to test the functionality of a custom device driver that you'll create in Part C.  Finally, you will write your own LKM that implements the device driver.

## User-Space Test Program 
Write an interactive test program that will allow you to read from, write to and seek in a file.  Your program should accept the name of the file on the command line:

If the filename doesn't exist, or isn't readable/writeable, your program should print an error message, and terminate with a non-zero exit status.
Once successfully invoked, your interactive program should open filename for reading/writing, prompt the user with the string option? and then accept the following input, followed each time by the carriage return/enter key:

r - Your test program should immediately ask for the number of bytes to read using the prompt:
Enter the number of bytes you want to read:
Making sure you create a large enough buffer using malloc(), read the file starting from it's current position.  Then, print the returned data out to the console (stdout), followed by a newline ("\n").

w - Your program should ask for the data to be written to the file, using the prompt:
Enter the data you want to write:
The user then enters the desired data terminated by a carriage return.  Your program should then write this data to the file.

s - Your program should prompt for values for offset and whence:
Enter an offset value: 
Enter a value for whence:
Your program should then set it's position in the file according to the offset and whence.  See the lseek manpage (Links to an external site.) for more info.

control+d - If the user enters ctrl+d at the option? prompt, then you should close the file, exit the program, and return the appropriate status.
Other - If the user enters something other than listed above, ignore it and prompt the user again.

## Device Driver LKM 
### Two kinds of device drivers 
- A character device driver reads and writes from/to the device character by character.  This is the most basic type of device driver and usually the simplest to implement.
- A block device driver reads or writes large chunks (blocks) of data with a single read/write operation.​  These types of drivers are more complex, but usually more efficient in their use of system resources.  Network interfaces and disk controllers generally prefer to use a block driver.
(This particular implementation is of a character device driver) 

### Creating a Device File 
Creating A Device File
To access your device driver, you will need to create a corresponding device file in the /dev directory, using the command mknod:
**sudo mknod -m <permission> <device_file_location> <type of driver> <major number> <minor number>**

### Device Driver Overview 
Device Driver Overview
The diagram below summarizes what is going on when you are working with a device driver.  From a user-space program, you will issue calls to ​open(), read(), write(), seek() or close().  These calls will access the device file in /dev which is associated with your device driver. 

For example, when you run echo hello >file.txt, the operations performed are: open the file, write “hello” to the file, and then close the file.

Similarly, when you run the command cat file.txt, the operations performed are: open the file, read the file content, and then close the file.

The device file, by way of its major and minor numbers, indicates to the kernel that you are trying to perform file operations on a device.  The kernel will invoke the corresponding file operations in the device driver.  The device driver then executes it's implementation of these file operations against the physical device. 

## Things I learned: 
1. Strings in C is weird: For all the '\n' that gets added on by fgets, need to change that to '\0' in order to have the full representation of strings in C. Which ends in '\0'. 
2. atoi when fail returns 0 but atoi of a character "0" also returns 0. So there's no way to tell if it's an error versus just 0. 
3. Bound checking is extremely important: The functions themselves are not built to check for bounds so as programmers we must implement bound checking. We have to make sure we don't read over the buffer, write over the buffer, seek over the buffer and seek past the start of the buffer. 
4. Write and Read functions cannot return 0 because functions that are asynchronous will just think it reads 0 bytes everytime and that will just have the programming in a loop because it will keep requesting for the data. 
