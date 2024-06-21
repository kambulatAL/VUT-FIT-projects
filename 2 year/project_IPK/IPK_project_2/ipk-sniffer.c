/*
***********************************************************
*         IPK project 2 (packet sniffer)                  *
* Author: Kambulat Alakaev(xalaka00)                      *
***********************************************************
*  Some parts of the source code are adopted from the https://www.tcpdump.org/pcap.html by Tim Carstens
*/

#include <pcap.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ip_icmp.h>
#include <netinet/icmp6.h>
#include <arpa/inet.h>
#include <getopt.h>



// global due to interrupt signal processing
pcap_t *handle;


/** @brief  print the usage description
    @param argc      count of arguments from cmd
    @param argv      agruments from cmd
*/
void print_help(int argc, char *argv[])
{
    // '--help' text
    char *help_text = "NAME:\n"                                                            \
    "\tipk-sniffer - A network analyzer  \n"                                               \
    "USAGE:\n"                                                                             \
        "\t./ipk-sniffer [options] [arguments ...]\n"                                      \
    "OPTIONS:\n"                                                                           \
    "\t--help                       display usage information and exit\n"                  \
    "\t[--interface|-i] <value>     set the interface\n"                                   \
    "\t-p               <port>      set port number \n"                                    \
    "\t-n               <count>     specifies the number of packets to display\n"          \
    "\t[--tcp|-t]                   will show only TCP packets\n"                          \
    "\t[--udp|-u]                   will show only UCP packets\n"                          \
    "\t--icmp4                      will display only ICMPv4 packets\n"                    \
    "\t--icmp6                      will display only ICMPv6 echo request/response\n"      \
    "\t--arp                        will display only ARP frames\n"                        \
    "\t--ndp                        will display only ICMPv6 NDP packets\n"                \
    "\t--igmp                       will display only  IGMP packets\n"                     \
    "\t--mld                        will display only  MLD packets\n"                      \
    "AUTHORS:\n"                                                                           \
    "\tKambulat Alakaev email: xalaka00@stud.fit.vutbr.cz\n";

    // if only two parameters were passed
    if (argc == 2)
    {
        if (!strcmp(argv[1],"--help")){
            fprintf(stdout, "%s",help_text);
            exit(EXIT_SUCCESS);
        }
    }
    else {
        // check if the '--help' parameter was passed with others params
        int i = 1;
        while (i < argc){
            if (!strcmp(argv[i],"--help")){
                fprintf(stderr,"ERROR: you need to pass '--help' without other parameters, " 
                "if you want to see the program description\n");
                exit(EXIT_FAILURE);
            }
            i++;
        }
    }
}


/** @brief  displays all available interfaces
*/
void print_interfaces()
{
    pcap_if_t *alldevsp;    // pointer to the list of devices
    pcap_if_t *dev_counter; // counter to loop on the list above
    char errbuf[PCAP_ERRBUF_SIZE];  // buffer for the error message

    // get a pointer to the list of devices
    if (pcap_findalldevs(&alldevsp,errbuf) == PCAP_ERROR){ 
        fprintf(stderr,"ERROR: The pcap_findalldevs function has failed: %s\n",errbuf);
        exit(EXIT_FAILURE);
    }

    // if no devices were found  
    if (alldevsp == NULL){
        fprintf(stderr,"There are no devices to display\n");
    }
    else // display each device in a loop
    {
        for(dev_counter=alldevsp; dev_counter != NULL;dev_counter=dev_counter->next){
            fprintf(stdout,"%s\n",dev_counter->name);
        }
    }
    pcap_freealldevs(alldevsp);
    exit(EXIT_SUCCESS);
    
}


/** @brief  checks if interface is passed to an -i option
    @param optarg   option from cmd
*/
void check_inter_value(char *optarg)
{
    //if the interface option has no value (and its the last passed option) -> print all interfaces
    if (optarg == NULL){ print_interfaces(); } 
    // if the interface option has no value (and there is an another option after it, i.e. '-i -p')
    if (optarg[0] == '-'){ print_interfaces(); }
}


/** @brief  checks if string is a positive number
    @param arg  value of the option from cmd
    @return boolean value
*/
bool is_number(char *arg)
{
    for(int i=0; i< strlen(arg); i++){
        if (!isdigit(arg[i])) return false;
    }
    if (atoi(arg) < 0) { return false;}
    return true;
}


/** @brief  function for cmd arguments parsing
    @param argc           count of arguments from cmd
    @param argv           agruments from cmd
    @param interface      string containing an interface
    @param port           string containing a port
    @param pack_to_disp   string containing a number of packets to display
    @param tcp            boolean value, contains true if tcp flag was set
    @param udp            boolean value, contains true if udp flag was set
    @param icmp4          boolean value, contains true if tcp icmp was set
    @param icmp6          boolean value, contains true if icmp6 flag was set
    @param arp            boolean value, contains true if arp flag was set
    @param ndp            boolean value, contains true if ndp flag was set
    @param igmp           boolean value, contains true if igmp flag was set
    @param mld            boolean value, contains true if mld flag was set
*/
void parse_cmd_args(int argc, char *argv[], char *interface, char *port, char *pack_to_disp,
                    bool *tcp, bool *udp, bool *icmp4, bool *icmp6, bool *arp, bool *ndp,
                    bool *igmp, bool *mld)
{
    int ch = 0;
    opterr = 0; //disable getopt errors 
    
    // stucture contains long options
    struct option long_opts[] =
        {
            {"interface", optional_argument, NULL, 'i'},
            {"tcp", no_argument, NULL, 't'},
            {"udp", no_argument, NULL, 'u'},
            {"icmp4", no_argument, NULL, '4'},
            {"icmp6", no_argument, NULL, '6'},
            {"arp", no_argument, NULL, 'a'},
            {"ndp", no_argument, NULL, 'd'},
            {"igmp", no_argument, NULL, 'g'},
            {"mld", no_argument, NULL, 'l'},
            {NULL, 0, NULL, 0}
        };

    // parse each parameter and set it value(if it was passed)
    while ((ch = getopt_long(argc, argv, "utn:p:i:", long_opts, NULL)) != -1)
    {
        switch (ch){
        case 'i':
            if (optarg == NULL && optind < argc && argv[optind][0] != '-'){
                // use the next argument as the value of --interface
                strcpy(interface, argv[optind]);
                optind++; // skip over the value argument
            }
            else{
                check_inter_value(optarg);
                strcpy(interface, optarg);
            }
            break;

        case 'n':
            if (optarg != NULL) {
                if (!is_number(optarg)){
                    fprintf(stderr,"ERROR: Wrong argument was passed for the '-n' option \n");
                    exit(EXIT_FAILURE);
                }
                strcpy(pack_to_disp, optarg); 
            }
            break;
        case 'p':
            if (optarg != NULL){ 
                if (!is_number(optarg)){
                    fprintf(stderr,"ERROR: Wrong argument was passed for the '-p' option \n");
                    exit(EXIT_FAILURE);
                }
                strcpy(port, optarg); 
            }
            break;
        case 't': // if the tcp parameter was passed
            *tcp = true;
            break;
        case 'u': // if the udp parameter was passed
            *udp = true;
            break;
        case '4': // if the icmp4 parameter was passed
            *icmp4 = true;
            break;
        case '6': // if the icmp6 parameter was passed
            *icmp6 = true;
            break;
        case 'a': // if the arp parameter was passed
            *arp = true;
            break;
        case 'd': // if the ndp parameter was passed
            *ndp = true;
            break;
        case 'g': // if the igmp parameter was passed
            *igmp = true;
            break;
        case 'l': // if the mld parameter was passed
            *mld = true;
            break;
        case '?':
            // if the '-i' option didnt get a value 
            if (optopt == 'i') { print_interfaces(); }            
            
            else if (optopt == 'p') {   // if '-p' has no value
                fprintf(stderr,"ERROR: Parameter '-p' requires an argument\n");
                exit(EXIT_FAILURE);
            }
            else if (optopt == 'n') {
                fprintf(stderr,"ERROR: Parameter '-n' requires an argument\n");
                exit(EXIT_FAILURE);
            }
            else {
                fprintf(stderr,"ERROR: unknown parameter was passed to the program \n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            fprintf(stderr,"ERROR: unknown parameter was passed to the program  %s\n",optarg);
            exit(EXIT_FAILURE);
        }
    }
    
    // if the --n parameter has no value-> set 1
    if (strlen(pack_to_disp) == 0){strcpy(pack_to_disp, "1"); }
}



/** @brief         inserts tcp/udp proto qualifiers to the filter string
    @param filter  string value, containing filter with flags(protocols)
    @param tcp     boolean value, contains true if tcp flag was set
    @param udp     boolean value, contains true if udp flag was set
    @param port    string containing a port
*/
void add_tcp_udp_filter(char *filter,bool tcp, bool udp, char *port)
{
    if (tcp){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        strcat(filter,"(tcp");
        // if port is specified
        if (strlen(port) != 0){
            strcat(filter," and port ");
            strcat(filter,port);  
        }
        // remove a redundant space in the end of filter
        if (filter[strlen(filter)-1] == ' '){ filter[strlen(filter)-1] = '\0'; }
        strcat(filter,") ");
    }

    if (udp){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        strcat(filter,"(udp");
        
        // if port is specified
        if (strlen(port) != 0){
            strcat(filter," and port ");
            strcat(filter,port);
        }
        // remove a redundant space in the end of filter
        if (filter[strlen(filter)-1] == ' '){ filter[strlen(filter)-1] = '\0'; }
        strcat(filter,") ");
    }
}


/** @brief         inserts icmp/icmp6 protocols to the filter string
    @param filter  string value, containing filter with flags(protocols)
    @param icmp4   boolean value, contains true if icmp flag was set
    @param icmp6   boolean value, contains true if icmp6 flag was set
*/
void add_icmp_filter(char *filter,bool icmp4, bool icmp6)
{
    if (icmp4){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        strcat(filter,"icmp ");
    }

    if (icmp6){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        strcat(filter,"(icmp6 and (icmp6[0] > 127 and icmp6[0] < 130)) ");
    }
}


/** @brief         inserts arp/ndp protocols to the filter string
    @param filter  string value, containing filter with flags(protocols)
    @param arp     boolean value, contains true if arp flag was set
    @param ndp     boolean value, contains true if ndp flag was set
*/
void add_arp_ndp_filter(char *filter, bool arp, bool ndp)
{
    if (arp){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        strcat(filter,"arp ");
    }

    if (ndp){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        // filter for the NDP(ICMPv6 NDP) packets
        strcat(filter,"(icmp6 and (icmp6[0] > 132 and icmp6[0] < 138)) "); 
    }
}


/** @brief         inserts igmp/mld protocols to the filter string
    @param filter  string value, containing filter with flags(protocols)
    @param igmp    boolean value, contains true if igmp flag was set
    @param mld     boolean value, contains true if mld flag was set
*/
void add_igmp_mld(char *filter, bool igmp, bool mld)
{
    if (igmp){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        strcat(filter,"igmp ");
    }

    if (mld){
        // if it isn't the only one parameter
        if (filter[strlen(filter)-1] != '(')
        {
            strcat(filter,"or ");
        }
        // filter for the MLD packets
        strcat(filter,"(icmp6 and (icmp6[0] > 129 and icmp6[0] < 133)) "); 
    }
}



/** @brief         configures a row, that contains filter for the sniffing
    @param interface      string containing an interface
    @param port           string containing a port
    @param tcp            boolean value, contains true if tcp flag was set
    @param udp            boolean value, contains true if udp flag was set
    @param icmp4          boolean value, contains true if tcp icmp was set
    @param icmp6          boolean value, contains true if icmp6 flag was set
    @param arp            boolean value, contains true if arp flag was set
    @param ndp            boolean value, contains true if ndp flag was set
    @param igmp           boolean value, contains true if igmp flag was set
    @param mld            boolean value, contains true if mld flag was set

    @return               string, containing a filter 
*/
char *filter_configure(char *interface, char *port, bool tcp, bool udp, bool icmp4, 
    bool icmp6, bool arp, bool ndp, bool igmp, bool mld)
{   
    char *filter = malloc(BUFSIZ); // contains the result filter
    memset(filter,'\0',BUFSIZ);
    char *default_filter = malloc(BUFSIZ); // contains the default filter (with all flags)
    
    strcat(default_filter,"(");
    add_icmp_filter(default_filter,true,true);
    add_arp_ndp_filter(default_filter,true,true);
    add_igmp_mld(default_filter,true,true);
    
    // if there is no passed protocol
    if (!tcp && !udp && !icmp4 && !icmp6 && !arp && !ndp && !igmp && !mld){
        // if port isnt specified
        if (strlen(port) == 0){
            strcat(default_filter, "or tcp");
            strcat(default_filter," or udp");            
            strcat(default_filter,")");
            free(filter);
            return default_filter; 
            }
        else {
            strcat(default_filter, "or (tcp and port ");
            strcat(default_filter, port);
            strcat(default_filter,")");
            strcat(default_filter, " or (udp and port ");
            strcat(default_filter, port);
            strcat(default_filter,")");

            strcat(default_filter,")");
            free(filter);
            return default_filter;
        }
    }

    strcat(filter,"("); // add open bracket to the filter

    if (strlen(port)!= 0 && !(tcp || udp)){
        if (icmp4 || icmp6 || arp || ndp || igmp || mld){
            strcat(filter, "port ");
            strcat(filter, port);
            strcat(filter, " and ");
        }
    }
    // if tcp or udp is passed -> write it to the filter
    if (tcp || udp){ add_tcp_udp_filter(filter,tcp,udp,port); }
    // if icmp4 or icmp6 is passed -> write it to the filter
    if (icmp4 || icmp6){ add_icmp_filter(filter,icmp4,icmp6); }
    
    // if arp or ndp is passed -> write it to the filter
    if (arp || ndp) { add_arp_ndp_filter(filter,arp,ndp); }

    // if igmp or mld is passed -> write it to the filter
    if (igmp || mld) { add_igmp_mld(filter,igmp,mld); }
    
    // remove a redundant space in the end of filter
    if (filter[strlen(filter)-1] == ' '){ filter[strlen(filter)-1] = '\0'; }

    strcat(filter,")"); // add closing bracket to the filter

    return filter;
}


/** @brief                creates a packet capture handler to receive packets described by the filter.
    @param filter         string value, containing filter with flags(protocols)
    @param interface      string containing an interface

    @return handler
*/
pcap_t* configure_pcap_handler(char *interface, char*filter)
{
    char errbuf[PCAP_ERRBUF_SIZE];	// string contains error
    struct bpf_program fp;		    // contains the compiled filter expression 
    bpf_u_int32 mask;		        //  The netmask of the sniffing device 
    bpf_u_int32 net;		        // The IP of the sniffing device 

    if (pcap_lookupnet(interface, &net, &mask, errbuf) == PCAP_ERROR) {
        fprintf(stderr, "ERROR from pcap_lookupnet: %s\n", errbuf);
        exit(EXIT_FAILURE);
    }

    handle = pcap_open_live(interface, BUFSIZ, 1, 1000, errbuf);
    
    if (handle == NULL) {
        fprintf(stderr, "ERROR: Couldn't open interface %s: %s\n", interface, errbuf);
        exit(EXIT_FAILURE);
    }
    if (pcap_compile(handle, &fp, filter, 0, net) == PCAP_ERROR) {
        fprintf(stderr, "ERROR: Couldn't parse filter: %s\n",pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }
    if (pcap_setfilter(handle, &fp) == PCAP_ERROR) {
        fprintf(stderr, "ERROR: Couldn't install filter: %s\n",pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    return handle;
}


/** @brief                prints NDP src/dst IP
    @param packet         contains captured packet
    @param ipv6_header    containing IPv6 header
*/
void print_ipv6_type(const u_char *packet, const struct ip6_hdr *ipv6_header)
{

     // Check if the protocol is ICMPv6
    if (ipv6_header->ip6_nxt == IPPROTO_ICMPV6) {
        struct icmp6_hdr *icmp6_header = (struct icmp6_hdr *)(packet + ETHER_HDR_LEN + sizeof(struct ip6_hdr));

        if (icmp6_header->icmp6_type == 133){
            printf("Type: Router Solicitation (NDP)\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 134){
            printf("Type: Router Advertisement (NDP)\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 135) {
            printf("Type: Neighbor Solicitation (NDP)\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 136) {
           printf("Type: Neighbor Advertisement (NDP)\n");
           return;
        }
        
        else if (icmp6_header->icmp6_type == 137){
            printf("Type: Redirect Message (NDP)\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 130){
            printf("Type: Multicast Listener Query (MLD)\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 131){
            printf("Type: Multicast Listener Report (MLD)\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 132){
            printf("Type: Multicast Listener Done (MLD)\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 128){
            printf("Type: ICMPv6 Echo Request\n");
            return;
        }
        else if (icmp6_header->icmp6_type == 129){
            printf("Type: ICMPv6 Echo Reply\n");
            return;
        }
    }
    else if (ipv6_header->ip6_nxt == 131)
    {
        printf("Type: IGMPv3\n");
        return;
    }
    printf("Type: another IPV6 protocol\n");
}


/** @brief                prints ipv4 protocol
    @param ip_hdr         contains IPv4 header
*/
void print_ipv4_type(const struct ip *ip_hdr)
{
    if (ip_hdr->ip_p == IPPROTO_ICMP){
        printf("Type: ICMPv4\n");
    }
    else if (ip_hdr->ip_p == IPPROTO_IGMP){
        printf("Type: IGMP\n");
    }
    else { printf("Type: another IPV4 protocol\n"); }
}


/** @brief                print ip src/dst IP addresses
    @param packet         contains captured packet
*/
void print_ip(const u_char *packet)
{
     // Get the IP header
    struct ip *ip_hdr = (struct ip*)(packet + ETHER_HDR_LEN);

    // Check if this is an IPv4 packet
    if (ip_hdr->ip_v == 4) {
        // Get the source and destination IPv4 addresses
        char src_ip[INET_ADDRSTRLEN];
        char dst_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_hdr->ip_src), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_hdr->ip_dst), dst_ip, INET_ADDRSTRLEN);
        
        printf("src IP: %s\n", src_ip);
        printf("dst IP: %s\n",dst_ip);

    }
    // Check if this is an IPv6 packet
    else if (ip_hdr->ip_v == 6) {
        // Get the IPv6 header
        struct ip6_hdr *ipv6_header = (struct ip6_hdr*)ip_hdr;

        // Get the source and destination IPv6 addresses
        char src_ip[INET6_ADDRSTRLEN];
        char dst_ip[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &(ipv6_header->ip6_src), src_ip, INET6_ADDRSTRLEN);
        inet_ntop(AF_INET6, &(ipv6_header->ip6_dst), dst_ip, INET6_ADDRSTRLEN);
        
        printf("src IP: %s\n", src_ip);
        printf("dst IP: %s\n",dst_ip);
    }
}


/** @brief                prints IPv4 ports for TCP and UDP
    @param packet         contains captured packet
    @param ip_hdr         contains IPv4 header
*/
void print_ipv4_ports(const u_char *packet,const struct ip *ip_hdr)
{   
    // determine size of IPv4 header
    int size_ipv4 = sizeof(struct ip);
    u_int16_t src_port = 0, dst_port = 0;
    
    // Check if this is a TCP or UDP packet
    if (ip_hdr->ip_p == IPPROTO_TCP) {
        printf("Type: TCP\n");
        // Extract the TCP header from the packet
        const struct tcphdr *tcp_hdr = (const struct tcphdr *)(packet + ETHER_HDR_LEN + size_ipv4);
        src_port = ntohs(tcp_hdr->th_sport);
        dst_port = ntohs(tcp_hdr->th_dport);
        printf("src port: %d\n",src_port);
        printf("dst port: %d\n",dst_port);
    } 
    else if (ip_hdr->ip_p == IPPROTO_UDP) {
        printf("Type: UDP\n");
        // Extract the UDP header from the packet
        const struct udphdr *udp_hdr = (const struct udphdr *)(packet + ETHER_HDR_LEN + size_ipv4);
        src_port = ntohs(udp_hdr->uh_sport);
        dst_port = ntohs(udp_hdr->uh_dport);
        printf("src port: %d\n",src_port);
        printf("dst port: %d\n",dst_port);
    }
    else { print_ipv4_type(ip_hdr); }
}


/** @brief                prints src/dst IPv4 for ARP
    @param packet         contains captured packet
*/
void print_arp_ip(const u_char *packet)
{
    struct ether_arp *arp_hdr = (struct ether_arp *)(packet + ETHER_HDR_LEN);
    // Convert the source IP address to a string and print it
    char src_ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, arp_hdr->arp_spa, src_ip_str, INET_ADDRSTRLEN);
    printf("src IP: %s\n", src_ip_str);

    // Convert the destination IP address to a string and print it
    char dst_ip_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, arp_hdr->arp_tpa, dst_ip_str, INET_ADDRSTRLEN);
    printf("dst IP: %s\n", dst_ip_str);
}


/** @brief               prints IPv6 ports for TCP and UDP
    @param packet        contains captured packet
    @param ip6_header    IPV6 header             
*/
void print_ipv6_ports(const u_char *packet,const struct ip6_hdr *ip6_header)
{   
    // determine size of IPv6 header
    int size_ipv6 = sizeof(struct ip6_hdr);
    u_int16_t src_port = 0, dst_port = 0;

    // check if the packet contains TCP protocol
    if (ip6_header->ip6_nxt == IPPROTO_TCP) {
        printf("Type: TCP\n");
        // get TCP header
        const struct tcphdr *tcp_hdr = (const struct tcphdr *) (packet + ETHER_HDR_LEN + size_ipv6);

        // get source and destination ports
        src_port = ntohs(tcp_hdr->th_sport);
        dst_port = ntohs(tcp_hdr->th_dport);

        printf("src port: %d\n",src_port);
        printf("dst port: %d\n",dst_port);
    }
    // check if the packet contains UDP protocol
    else if (ip6_header->ip6_nxt == IPPROTO_UDP) {
        printf("Type: UDP\n");
        // get UDP header
        const struct udphdr *udp_hdr = (const struct udphdr *) (packet + ETHER_HDR_LEN + size_ipv6);

        // get source and destination ports
        src_port = ntohs(udp_hdr->uh_sport);
        dst_port = ntohs(udp_hdr->uh_dport);

        printf("src port: %d\n",src_port);
        printf("dst port: %d\n",dst_port);
    }
    else{ print_ipv6_type(packet,ip6_header); }
}


/** @brief                prints ip and ports(for the protocols, that work with ports)
    @param packet         contains captured packet
*/
void print_ip_ports(const u_char *packet)
{
    const struct ether_header *ether_hdr = (struct ether_header *) packet;
    uint16_t eth_type = ntohs(ether_hdr->ether_type);

    switch(eth_type){
        // If packet has ARP protocol
        case ETHERTYPE_ARP:
            print_arp_ip(packet);
            printf("Type: ARP\n");
            break;
        // If packet has IPv4 protocol
        case ETHERTYPE_IP:
            print_ip(packet);
            const struct ip *ip_hdr = (const struct ip*)(packet + ETHER_HDR_LEN);
            print_ipv4_ports(packet,ip_hdr);
            break;
        // If packet has IPv6 protocol
        case ETHERTYPE_IPV6:
            print_ip(packet);
            const struct ip6_hdr *ip6_header = (const struct ip6_hdr *)(packet + ETHER_HDR_LEN);
            print_ipv6_ports(packet, ip6_header);
            break;
    }
}


/** @brief                prints packet data
    @param packet         contains captured packet
    @param packet_len     frame length              
*/
void print_data(const u_char *packet, int packet_len) {
    for (int i = 0; i < packet_len; i++) {
        if (i % 16 == 0) {
            printf("0x%04x: ", i); // print fist left symb
        }
        printf("%02x ", packet[i]); // print hex numbers
        if (i % 16 == 15 || i == packet_len - 1) {
            int j;
            for (j = 0; j < 15 - (i % 16); j++) { printf("   "); }
           
            for (j = i - (i % 16); j <= i; j++) {
                if (packet[j] >= 32 && packet[j] <= 126) {
                    printf("%c", packet[j]); // convert hex numbers to char and print
                }
                else { printf("."); }
            }
            printf("\n");
        }
    }
}


/** @brief               parses packets
    @param packet        contains captured packet
    @param header        packet header             
*/
void parse_packet(u_char *args, const struct pcap_pkthdr *header,const u_char *packet)
{
    time_t ts_sec = header->ts.tv_sec;
    suseconds_t ts_usec = header->ts.tv_usec;
    struct tm *tm_info = localtime(&ts_sec);
    char timestamp[30];
    long ts_msec = ts_usec / 1000;

    // get timestamp and print it 
    strftime(timestamp, 30, "%Y-%m-%dT%H:%M:%S", tm_info);
    printf("timestamp: %s.%03ld%+03ld:%02ld\n", timestamp, ts_msec, tm_info->tm_gmtoff / 3600, 
            labs(tm_info->tm_gmtoff % 3600) / 60);

    //get src/dst MAC adresses 
    struct ether_header *eth_hdr = (struct ether_header *) packet;
    // Extract the source MAC address
    const unsigned char *src_mac = eth_hdr->ether_shost;
    // Extract the destination MAC address
    const unsigned char *dst_mac = eth_hdr->ether_dhost;
    
    printf("src MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",src_mac[0], src_mac[1], src_mac[2], 
            src_mac[3], src_mac[4], src_mac[5]);
    printf("dst MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",dst_mac[0], dst_mac[1], dst_mac[2], 
            dst_mac[3], dst_mac[4], dst_mac[5]);
    printf("frame length: %d bytes\n",header->caplen);
    print_ip_ports(packet);
    printf("\n");
    print_data(packet,header->caplen);
    printf("\n");
}


/** @brief                set handler for the  interrupt signal processing
    @param signum         numeric value of the signal
*/
void sigint_handler(int signum)
{
    pcap_close(handle);
    exit(EXIT_SUCCESS);
}


/** @brief              main function
    @param argc           count of arguments from cmd
    @param argv           agruments from cmd            
*/
int main(int argc, char *argv[])
{
    char interface[50];    // contains an interface
    char port[10];         // contains the port number
    char pack_to_disp[20]; // contains number of packets to print
    
    print_help(argc,argv); // check if '--help' was passed

    memset(interface,'\0',sizeof(interface));
    memset(port,'\0',sizeof(port));
    memset(pack_to_disp,'\0',sizeof(pack_to_disp));


    // below are variables, that indicate if a specific protocol was passed to the program
    bool got_tcp = false;       
    bool got_udp = false;       
    bool got_icmp4 = false;
    bool got_icmp6 = false;
    bool got_arp = false;
    bool got_ndp = false;
    bool got_igmp = false;
    bool got_mld = false;

    // get parameters from cmd 
    parse_cmd_args(argc, argv, interface, port, pack_to_disp, &got_tcp, &got_udp, &got_icmp4, 
    &got_icmp6, &got_arp, &got_ndp, &got_igmp, &got_mld);
    
    // if there wasnt even set the '--interface' parameter
    if (strlen(interface) == 0) { print_interfaces(); }

    // configure a filter
    char *filter = filter_configure(interface,port,got_tcp,got_udp,got_icmp4,
                                    got_icmp6,got_arp,got_ndp,got_igmp,got_mld);
    
    // configure packet capture handle.
    handle = configure_pcap_handler(interface,filter);

    // check if datalink layer type is correct
    if (pcap_datalink(handle) == PCAP_ERROR){
        fprintf(stderr,"ERROR: pcap_datalink(): %s\n", pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }
    // set function to process the Interrupt signal
    signal(SIGINT,sigint_handler);

    // reformat value of the '-n' option to int
    int n_count = atoi(pack_to_disp);

    // start sniffing 
    if (pcap_loop(handle, n_count, parse_packet, (u_char*)NULL) < 0) {
    	fprintf(stderr, "ERROR: pcap_loop failed: %s\n", pcap_geterr(handle));
	    return -1;
    }

    free(filter);
	pcap_close(handle);
    return 0;
}
