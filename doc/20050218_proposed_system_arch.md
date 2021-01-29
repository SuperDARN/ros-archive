
# Radar Operating System: Proposed System Architecture

## Author

R.J.Barnes

## Summary

An overview of the proposed system architecture for the Radar Operating System.

## Description:

### 1. Introduction

The Radar Operating System (ROS) is the key software component that controls the radar hardware. Part of the success of SuperDARN can be attributed to the fact that all radars run identical control software. However in recent years a number of hardware modifications at different radar sites have made it increasingly difficult to run the common ROS across the network. These changes include the implementation of Stereo operations at certain sites and the adoption of digital receivers. In the current generation of ROS, these changes are accomadated by using the site library to handle site specific variations in hardware and operations. The problem with this approach is that with more and more functionality having to be built into the site library the goal of having a single set of common operating software across the network is in danger of being lost.

This document will attempt to define in broad terms a System Architecture that can accommodate the hardware differences between the radars without having to provide different versions of the Operating System for each site.

### 2. System Architecture - The Layer Model

The Radar Operating System can be thought of as a series of interconnected layers.

The lowest layer, the "Hardware" layer consists of the device drivers that control the interface cards connected to the radar hardware; the DIO and the A/D or digital receiver drivers.

The device drivers provide a software interface that allow the next layer of software, the "Signal Processing" layer, operate the radar. This layer consists of the Radar Control Program (RCP). The RCP processes the signals received by the radar to produce the standard SuperDARN data products; raw ACF and derived parameters from the FITACF algorithm.

The final "Data Processing" layer consists of the software for storing data, providing user displays and relaying data over the Internet.

```
      +---------------------+
      |                     |
      |                     |
  3   |   Data Processing   |
      |                     |
      |                     |
      +---------------------+
      |                     |
      |                     |
  2   |  Signal Processing  |
      |                     |
      |                     |
      +---------------------+
      |                     |
      |                     |
  1   |      Hardware       |
      |                     |
      |                     |
      +---------------------+
```

This model is very similar to the layered approach of the TCP/IP approach where each successive layer provides solves a particular set of problems and provides a well defined set of services to the layer above. The lowest layers are closest to the hardware, while the highest deal with the data delivered to the end user.

### 3. Hardware Differences

To solve the problems caused by differences in radar hardware it is proposed to modify the layer model by splitting the "Hardware" layer into a "Physical Hardware" layer and a "Virtual Hardware" layer. The "Physical Hardware" layer will consist of the low-level device drivers, while the "Virtual Hardware" layer which consists of software that presents a standard interface to the "Signal Processing" Layer regardless of the underlying hardware.

```
      +---------------------+
      |                     |
      |                     |
  3   |  Data Processing    |
      |                     |
      |                     |
      +---------------------+
      |                     |
      |                     |
  2   |  Signal Processing  |
      |                     |
      |                     |
      +---------------------+
      |                     |
  1B  |  Virtual Hardware   |
      |                     |
      +---------------------+
      |                     |
  1A  |  Physical Hardware  |
      |                     |
      +---------------------+
```

#### 3.1 Physical Hardware Layer

In the current implementation of ROS, the interface between the control program and the device drivers consists of a sequence of high-level operations that instructs a driver to perform a function such as transmit a pulse sequence or take a series of samples. Consequently the device drivers contain a lot of extra functionality to translate these operations into commands to control the interface cards. This is especially true of the digital receiver driver which must calculate a set of operating parameters and filter co-efficients for the receiver.</p><p>The problem with this approach is that the device drivers do not allow the full capabilities of the interface card to be used. Also, any changes in the way in which the hardware is operated will require modifications the device driver, which is a major undertaking.

The intention of the "Physical Hardware" layer is make the device drivers as simple as possible; making the full capabilities of an interface card available from software.

In the case of the digital receiver this would allow filter-coefficients, sampling rates and gain factors to be controlled by sending commands to the driver. For the existing analagous receiver design the low-level driver for the A/D card will be virtually identical to the current driver.

#### 3.2 Virtual Hardware Layer

The programming interface to the various low-level drivers will be very different, reflecting the various capabilities of the hardware. However the basic hardware operations of the radar are the same, regardless of the underlying hardware; the radar sets a frequency, transmits a pulse sequence and receives a sequence of samples.

The "Virtual Hardware" Layer is consists of a set of "virtual" device drivers that translate the basic radar operations into commands to the low-level drivers. This approach retains the flexibility of having full control of the underlying hardware, while allowing a single control program to run on the entire network.

Although the majority of the basic operations are the same, major differences in the underlying hardware, such as the differences between a digital and analogue receiver, still present a problem as there are differences in the way the radar is operated. (For example, the clear frequency search for a digital receiver involves sampling over a broad bandwidth and performing an FFT on the result).

To solve this problem there will be a mechanism for the virtual drivers to communicate these differences to the higher level software. The control program will request from the virtual driver a "capabilities list" that will allow it to transparently adjust for the underlying hardware.

## References

None

## Revision

2005/02/18  Initial Revision.
