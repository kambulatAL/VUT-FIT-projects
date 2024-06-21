# IPK project 2 documentation
## Content
 - [Description](#1-description)
 - [Implementation](#2-implementation)
 - [Tests](#3-tests)
 - [Bibliography](#bibliography)
 - [Author](#author)


## 1. Description
<p>
    The project represents a sniffer program, that is capable to capture packets with the specified protocols (TCP,UDP,ICMPv4,ICMPv6,MLD,NDP,IGMP,ARP) and on a specific port. The user selects the interface/device with the [-i|--interface] option, on which the program captures packets. Then user can select on of the protocols to capture packets with that type of the protocol and chose number of the packets to display with -n option.<br> 
    Packets are displayed to stdout, each packet is separated by a new line. The displayed information consists of: packet capture time, source/destination MAC addresses, length of the packet frame, source/destination IP, type of the protocol, source/destination port(if any) and the packet byte offset. 
</p>

### 1.1 Theory
<p>
Packet sniffers are tools that allow the capturing and analysis of network traffic. They can be used for various purposes, such as network troubleshooting, security analysis, or performance optimization.
<ul>
<li>

The main goal of **UDP** (User Datagram Protocol) is to provide a lightweight transport layer protocol that allows for quick and unreliable communication between network hosts. UDP is often used for real-time applications such as video streaming or online gaming.
</li>
<li>

**TCP** (Transmission Control Protocol), on the other hand, is a reliable transport layer protocol that provides error detection, flow control, and congestion control mechanisms. TCP is used for applications that require reliable communication, such as file transfer or email.
</li>
<li>

**NDP** (Neighbor Discovery Protocol) is a protocol used in IPv6 networks for neighbor discovery, address resolution, and parameter discovery. NDP is similar to ARP (Address Resolution Protocol) in IPv4 networks.

</li>
<li>

**MLD** (Multicast Listener Discovery) and **IGMP** (Internet Group Management Protocol) are used for multicast communication. MLD is used in IPv6 networks, while IGMP is used in IPv4 networks. These protocols enable hosts to join or leave multicast groups, and routers to learn which hosts are interested in receiving multicast traffic.

</li>
<li>

**ICMPv4** (Internet Control Message Protocol version 4) and **ICMPv6** (Internet Control Message Protocol version 6) are used for error reporting and diagnostic messages. ICMPv4 is used in IPv4 networks, while ICMPv6 is used in IPv6 networks.

</li>
<li>

**ARP** (Address Resolution Protocol) is used in IPv4 networks to map a network address (IP address) to a physical address (MAC address). ARP is used to resolve the destination MAC address when sending an IPv4 packet to another host on the same network.
</li>
</ul>
</p>

## 2. Implementation
<p>
    The program is written in C language. For sniffering was used the <i>PCAP</i> library. The program consists of 4 parts: <br>
        - argument parsing; <br>
        - packet filter creation; <br>
        - packet capturing; <br>
        - displaying the information from a packet.

</p>

### 2.1 Argument parsing
<p>
    To parse the program argument was used the <i>getopt</i> library, that alows to parseboth long and short type of arguments.
    If the program gets an undefined parameter, it will throw an error.Similarly, if a parameter that should have the required value does not receive it or receives an incorrect value, the program will generate an error. If a protocol was passed to the program, than it sets true value to the variable for that protocol, which will be used in the filter creation. Argument parsing is implemented in the 'parse_cmd_args' function.
</p>

### 2.2 Packet filter creation
<p>
    The elements for the filter were taken from the PCAP-FILTER MAN PAGE at https://www.tcpdump.org. The filter is made based on the protocols and the port passed to the program as arguments. The protocol flags are checked and if the protocol was passed to the program, it is added to the filter string.
</p>

### 2.3 Packet capturing
<p>
    All packets are captured with the 'pcap_loop' function and are processed in the 'parse_packet' function (callback), that displays basic information from packet like timestamp, source/destination MAC addresses and frame length.
</p>

### 2.4 Displaying packet information
<p>
    There are several functions in the program, that display information about inner parts of the packet depending on the type of packet protocol. All infromation related to the IP addresses and ports is printed from  the  'print_ip_ports' function.The packet byte offset is printed from the 'print_data' function.These functions are called from the 'parse_packet' function. Protocols ARP, ICMPv4 and IGMPv1(v2) works under IPv4 and their src/dst IP addresses will be displayed as a 32 bits long and is expressed in dotted-decimal notation. For example, 192.168.0.1 is a common IPv4 address used in private networks. Protocols ICMPv6, MLDv2, NDP, IGMPv3  works under IPv6 and their src/dst IP addresses wil be displayed as a 128-bit number and is expressed in hexadecimal format, typically separated by colons into eight 16-bit blocks.
</p>

![tcp packet](/tests/example.png?raw=true)


<p align="center"><font size="2">Picture 1: Example of packet display</font></p>

## 3. Tests

### 3.1 Testing with Wireshark
<p>The output of the program was checked against the output of Wireshark (a network packet analysis program). ICMPv4 packet capturing was tested by using 'ping' command in cmd.</p>

![Wireshark example](/tests/wireshark_ex.png?raw=true)
<br>

![Program output](/tests/wireshark_ex_pr.png?raw=true)
<br>

<p align="center"><font size="2">Picture 2: Comparing of the Wireshark and program output</font></p>

### 3.2 Third-party programs for testing
To test for MLD, ICMPv6, IGMP and NDP packets we used the packet sending program from the [Github](https://github.com/turytsia/vut-ipk-packegen) repository of fellow student Oleksandr Turitsia (xturyt00). You will find the cloned contents of the repository in the 'tests' folder. The tests with capturing the packets of the above types were done on the NixOS system from the first project using the packet submitter. As argument to the packet submitter was passed inet6 address, that was got with the 'ifconfig' command from cmd. Also to the '--mode' option was passed type of the protocol to send packets of that type. 
 
![ICMPv6 test](/tests/third_party_test.png?raw=true)

<p align="center"><font size="2">Picture 3: Testing ICMPv6 packet capturing with the packet sender program by xturyt00</font></p>

## Bibliography
Used resources:
    <ul>
        <li>
            Free Software Foundation, Inc. <i>Declarations for getopt.</i> Available at: https://sites.uclouvain.be/SystInfo/usr/include/getopt.h.html
        </li>
        <li>
            The Tcpdump Group. <i>PCAP-FILTER MAN PAGE.</i> Available at: https://www.tcpdump.org/manpages/pcap-filter.7.html
        </li>
        <li>
            The Tcpdump Group. <i>PCAP(3PCAP) MAN PAGE.</i> Available at: https://www.tcpdump.org/manpages/pcap.3pcap.html
        </li>
        <li>
            Tim Carstens. <i>PROGRAMMING WITH PCAP. Copyright 2002.</i> Available at: https://www.tcpdump.org/pcap.html
        </li>
        <li>
            Wikipedia. <i>Internet Group Management Protocol.</i> Available at: https://en.wikipedia.org/wiki/Internet_Group_Management_Protocol
        </li>
        <li>
            Wikipedia. <i>ICMPv6.</i> Available at: https://en.wikipedia.org/wiki/ICMPv6
        </li>
        <li>
            Wikipedia. <i>EtherType.</i> Available at: https://en.wikipedia.org/wiki/EtherType
        </li>
        <li>
            Wikipedia. <i>Ethernet frame.</i> Available at: https://en.wikipedia.org/wiki/Ethernet_frame
        </li>
        <li>
            G. Klyne (Clearswift Corporation), C. Newman (Sun Microsystems). July 2002. <i>Date and Time on the Internet: Timestamps(RFC 3339).</i> Available at: https://www.ietf.org/rfc/rfc3339.txt
        </li>
    </ul>
<br>

### Author 
Kambulat Alakaev, email: xalaka00@stud.fit.vutbr.cz