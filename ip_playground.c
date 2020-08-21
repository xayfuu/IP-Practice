#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* * * * * * * * * * * * * * * * * * * * *
 *                                       *
 * $ gcc ip_playground.c -lws2_32        *
 *                                       *
 * * * * * * * * * * * * * * * * * * * * */

/*
 * Small program that prints the computer's Local IP, the Netmask,
 * the Network's IP, and the Broadcast IP. IPv4 only.
 * 
 * Written for practice in a Networking Lecture at Uni.
 */

// ipv4 has 4 numbers
const unsigned int ipv4_size = 4;


// define generics for array printing
#define print_array(arr, size, newline)                 \
    _Generic((*arr),                                    \
        int: print_int_arr,                             \
        char: print_char_arr,                           \
        char*: print_char_marr)(arr, size, newline)     \

void print_int_arr(int *arr, int size, int newline)
{
    for(int i = 0; i < size; i++)
        printf("%d ", arr[i]);
        
    if(newline == 1)
        printf("\n");
}

void print_char_arr(char *arr, int size, int newline)
{
    for(int i = 0; i < size; i++)
        printf("%s ", arr[i]);
    
    if(newline == 1)
        printf("\n");
}

void print_char_marr(char **arr, int size, int newline)
{
    for(int i = 0; i < size; i++)
        printf("%s ", arr[i]);
    
    if(newline == 1)
        printf("\n");
}

// split a string into an array on a delimiter
char** split(char *str, const char *delim) 
{
    char *token, *token_c, *str_c, **result_arr;
    int  result_size = 0;
    int  append_counter = 0;
    
    str_c = strdup(str);
    token_c = strtok(str_c, delim);
    
    while(token_c != NULL)
    {
        token_c = strtok(NULL, delim);
        result_size += 1;
    }
    free(str_c);

    if(result_size != ipv4_size)
    {
        printf("Fatal: Bad IPv4 String");
        exit(0);
    }
    append_counter = result_size;

    result_arr = (char**) malloc(result_size * sizeof(char**));
    token = strtok(str, delim);

    while(token != NULL)
    {
        result_arr[result_size - append_counter] = token;
        append_counter -= 1;
        token = strtok(NULL, delim);
    }
    
    return result_arr;
}

// convert string array to array of ints
int* to_int_array(char **str_arr, int size)         
{
    int *ret = malloc(sizeof(int) * size);
    
    for(int c = 0; c < size; c++)
        ret[c] = strtol(str_arr[c], NULL, 10);
        
    return ret;
}

// retrieve local ipv4 address through winsock2 lib
char* get_IPv4() 
{
    char ac[32];

    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) 
    {
        printf("Error: Unable to get local hostname");
        exit(0);
    }
    
    struct hostent *host_entity = gethostbyname(ac);
    if (host_entity == 0) 
    {
        printf("Error: Bad hostname lookup");
        exit(0);
    }

    struct in_addr ipv4;
    char* addr = (*host_entity).h_addr_list[0];
    memcpy(&ipv4, addr, sizeof(struct in_addr));
    
    return inet_ntoa(ipv4);
}

// using the local ip and the netmask, calculate the network address
int* get_network_address(int *local, int *netmask, int size)
{
    int *network_ip = malloc(sizeof(int) * size);
    for(int i = 0; i < size; i++)
        network_ip[i] = local[i] & netmask[i];

    return network_ip;
}

// using the local ip and the netmask, calculate the broadcast address
int* get_broadcast_address(int *local, int *netmask, int size)
{
    int *broadcast_ip = malloc(sizeof(int) * size);
    for(int i = 0; i < size; i++)
        broadcast_ip[i] = local[i] | (netmask[i] ^ 255);
        
    return broadcast_ip;
}

int main(int argc, char *argv[])
{
    WSADATA wsaData;
    
    if (WSAStartup(MAKEWORD(1,1), &wsaData) != 0)
    {
        printf("WSAStartup failed"); 
        return 1;
    }
    
    char netmask[] = "255.255.255.0";
    char **ip_local_s, **ip_netmask_s;
    int  *ip_local_i, *ip_netmask_i;
    int  *network_address, *broadcast_address;
    
    ip_local_s = split(get_IPv4(), ".");
    ip_local_i = to_int_array(ip_local_s, ipv4_size);
    
    printf("Local IP:\t");
    print_array(ip_local_s, ipv4_size, 1);
    free(ip_local_s);

    ip_netmask_s = split(netmask, ".");
    ip_netmask_i = to_int_array(ip_netmask_s, ipv4_size);

    printf("Netmask:\t");
    print_array(ip_netmask_s, ipv4_size, 1);
    free(ip_netmask_s);
    
    network_address = get_network_address(ip_local_i, ip_netmask_i, ipv4_size);
    
    printf("Network IP:\t");
    print_array(network_address, ipv4_size, 1);
    free(network_address);
    
    broadcast_address = get_broadcast_address(ip_local_i, ip_netmask_i, ipv4_size);

    printf("Broadcast IP:\t");
    print_array(broadcast_address, ipv4_size, 1);
    free(broadcast_address);

    return 0;
}
    
