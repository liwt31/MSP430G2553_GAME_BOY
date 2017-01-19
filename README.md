# **A Game Boy Based on MSP430G2553**

## **Introduction**

This is the final term project of *The Principles of Computer Hardware* course in THU, delivered by Xiaomei Zhu. The course is focused on Microcontrollers. In the final project, the students are requested to develop an application on the hardware platform of MCU. 

I chose to implement my own little Game boy. Because I believe every male coder has once dreamed to develop his own games, and starting from drawing every pixel on the screen is almost the coolest way to realize this dream.

In the aspects of hardware, the system consists of 4 parts, a MSP430G2533 launchpad, a 12864B LCD screen, a key(n. rather than adj., button) module and a power module. The connection of these hardwares could be found in below-listed files. Major inner modules of the MCU is used in the project, including basic input&output system(or maybe BIOS), interruption system, basic clock system, timer module and so on.

In the aspects of software, the program is developed by C, and is probably only compatible with MSP430G2533 platform. 2 classic games was implemented in the program, including Greedy Snake and Push Box. Initially I also wondered if I could add Reverse Chess to the system, and implement a bit of graph searching because the game would require PVE. But the memory of the MCU is really too small for this(512 Byte), so I finally gave up.

## **Contents**

#### **SouceCode-dir**

All source code of the project, compilable in IAR Embeded Workbench.

#### **ProjectReport-pdf**

A detailed description of the project submitted to the professor for evaluation, in Chinese.

#### **IntroToPresentation-doc**

An introduction on how to use this Game Boy when the hardwares are ready, in Chinese.

