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

In this example there are multiple memory registers that have been exposed to the linux guest so that it can run properly. Compared to the simple example an additional memory register ```efuse@30350000``` has been exposed. It is not known whether or not this will need to be mapped in for this example.

The uart serial device at '''serial@30860000''' is also mapped into the guest in the system file. This shouldn't be here as this example shouldn't be relying on the serial port, instead it should be transimitting data out through virtio.



