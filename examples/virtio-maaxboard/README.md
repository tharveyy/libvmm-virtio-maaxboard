# Development notes

## Current issues

In its current state the project runs without memory faults or exceptions. For this to happen some errors have been ignored which will need to be investigated.

### SMC unknown service value

The following error message is outputted:

```CLIENT_VMM-1|ERROR: Unhandled SMC: unknown value service: 0x2, function number: 0x6```

This logging is from the function ```handle_smc()``` in the file src/arch/aarch64/smc.c.

The only service that is handled without error is the ```SMC_CALL_STD_SERVICE``` which is defined as 0x4. The service that needs to be hanled in this case is 0x2 which is defined as ```SMC_CALL_SIP_SERVICE```. In the simple maaxboard example as well as this example there are 6 calls to this function, each with the service number of 0x2 followed by the function numbers: 0,6,10,10,10,10. However in this example there is an additional call with 0x2 as the service which causes an error. 

In https://chasinglulu.github.io/downloads/ARM_DEN0028B_SMC_Calling_Convention.pdf the services are described:

* SiP Service (0x2) Provides interfaces to SoC implementation-specific services on this platform, for
example secure platform initialization, configuration, and some power control
services.

* Standard Secure Service (0x4) -  Standard Service Calls for the management of the overall system. By standardizing
such calls, the job of implementing Operating Systems on ARM is made easier.

It is not known why this is being called and needs to be investigated. The Sip Service is currently handled by making it process the data as if it was a Standard secure service, which will need to be changed.

### Passthrough

In this example there are multiple memory registers that have been exposed to the linux guest so that it can run properly. All memory registers mapped in from the simple example and an additional memory register ```efuse@30350000``` has been exposed. It is not known whether or not this will need to be mapped in for this example.

The uart serial device at '''serial@30860000''' is also mapped into the guest in the system file. This shouldn't be here as this example shouldn't be relying on the serial port, instead it should be transimitting data out through virtio.

### virtio_console_reset() 

In src/virtio/console.c the function ```virtio_console_reset()``` is called which produces an error. It seems as if this function shouldn't be called in the first place, so further investigation needs to be done to see why this is happening. For now this error message has been commented out. The virtio part of this appears to have not been configured properly, especially as it still expects to have access to the phyiscal serial device.

## Notes

### Device tree

The disable.dts file has been copied from the maaxboard simple example so it is not fully known what here is needed. There could also very well be additional things that must be disabled here, because of the virtio stuff. The serial port has been added to this but there is still a fault at that memory register when it is not mapped in. More things potentially need to be mapped in here, although they don't now have an effect, they may be needed.

In the init.dts file the memory has been included, which is then used in the system file. There were issues getting this to work, but the current set up seems to have resolved the virtual memory faults I was initially seeing. This memory location is also used so that linux,initrd-start and linux,initrd-end are set properly.

The bootargs are the same as the odroid example and may need changing.

### Linux kernel image

The odroid virtio image works on the maaxboard simple example, so it appears that if the images have extra config settings (like virtio in this case) then it is not a problem and can still be run. In this case there is some output stating that it is attempting to probe for a device that the linuxx kernel is expecting but cannot find. 

I may be seeing issues though because my config file is missing things and because it has not been told to do certain imx8 specific things then it goes to the defaults settings. For example it could be trying to access the clock because i have not set it to use a virtual clock in the config settings. The following page lists how the kvm paravirtualised clock should be present:

https://www.tencentcloud.com/document/product/213/9929 

The page also mentions about the drivers and the filesystem - so it looks like the correct filesystem should be used. 

It would be useful to see the difference between the odroid and the maaxboard config file to see what differences there are. It would also be useful to see the differences between the odroid simple and virtio examples to see what specific virtio things are needed. 


## Links

### SMC
https://chasinglulu.github.io/downloads/ARM_DEN0028B_SMC_Calling_Convention.pdf <br />
https://docs.sel4.systems/Tutorials/threads.html <br />

### Device trees
https://www.nxp.com/docs/en/application-note/AN5125.pdf  <br /> 

### Virtio
https://blogs.oracle.com/linux/post/introduction-to-virtio  <br />
https://projectacrn.github.io/latest/developer-guides/hld/virtio-console.html <br />

### VGIC
https://developer.arm.com/documentation/ihi0048/b/GIC-Support-for-Virtualization/Managing-the-GIC-virtual-CPU-interface  <br />


