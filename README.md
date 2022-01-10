# ICS2020 Programming Assignment

This project is the programming assignment of the class ICS(Introduction to Computer System)
in Department of Computer Science and Technology, Nanjing University.

For the guide of this programming assignment,
refer to http://nju-ics.gitbooks.io/ics2020-programming-assignment/content/

To initialize, run
```bash
bash init-s0duku.sh
source ~/.bashrc
```

The following subprojects/components are included. Some of them are not fully implemented.
* [NEMU](https://github.com/NJU-ProjectN/nemu)
* [Abstract-Machine](https://github.com/NJU-ProjectN/abstract-machine)
* [Nanos-lite](https://github.com/NJU-ProjectN/nanos-lite)
* [Navy-apps](https://github.com/NJU-ProjectN/navy-apps)


# Personal Progress

[Notion about this proj](https://tough-close-477.notion.site/PA-775201eea91a49538eb2979f439678d5)

![1](https://raw.githubusercontent.com/s0duku/ics2020-s0duku/main/examples/1.png)  
![1](https://raw.githubusercontent.com/s0duku/ics2020-s0duku/main/examples/2.png)   
![1](https://raw.githubusercontent.com/s0duku/ics2020-s0duku/main/examples/3.png)  

## Kown Bugs
* fceux-am can not run with audio, this looks like the orginal AM has some problem deal with divd.
* audio play some times works not normal, and may have noise. 

## PA1
* Expression Evaluate 
* Watchpoint Set/Delete
* Simple Debugger

## PA2
* TRM
* Audio
* Vga
* ...

## PA3
* vfs
* syscall

### For Testing

```
cd nemu
make
./build/x86-nemu-interpreter
```

