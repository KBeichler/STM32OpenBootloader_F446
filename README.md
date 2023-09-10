# STM OpenBootLoader Port to STM32F446RE Nucleo Board

With this Bootloader the Chip can be programmed with the STM32CubeProgrammer. An example programm is in the examples folder.

This Project was created with the stm32-for-vscode externsion.

Links:

[OpenBootLoader on Github](https://github.com/STMicroelectronics/stm32-mw-openbl)

[Bootloader Info](https://www.st.com/resource/en/application_note/cd00167594-stm32-microcontroller-system-memory-boot-mode-stmicroelectronics.pdf)

[Bootloader Can Protocoll](https://www.st.com/resource/en/application_note/cd00264321-can-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf)

[Bootloader USART Protocoll](https://www.st.com/resource/en/application_note/cd00264342-usart-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf)


### TODO

#### Prio 1
- [ ] Timing is off when Programm is uploaded via bootloader? RRC config is not the same
- [ ] Implement Optionbytes Interface correctly
- [ ] Check if Bootloader Deinit is sufficient

#### Prio 2
- [ ] Implement CAN (not on this chip, but need it)
- [ ] Change flash interface to support multiple Flash Banks (for H7 series)
- [ ] more testing on userprograms.
  	- [x] check if interrupts are working correctly
  	- [ ] maybe add shared RAM space for communication
  	- [ ] maybe add programm header at beginning of userprogramm section (appversion, crc....)


#### Prio 3 
- [ ] Implement startup code to check if Valid userprogram is installed (now we just check if there is data in Flash sector 2)´
- [ ] check if the user program start address can be read on compile time (now its hardcoded into openbootloader_conf.h)
- [ ]  keep size of image small as possible (should not exceed 16k)
- [ ] Check system memory interface read (although not needed)
- [ ] check if Specialcommands are needed

# Infos

Userprograms need to execute the following code rigt after the entry to main. With the the interrupt vector table REgister gets set to the correct position.

```
extern const uint32_t g_pfnVectors[];
SCB->VTOR = (uint32_t)&g_pfnVectors[0];
```


