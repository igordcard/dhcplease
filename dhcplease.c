#include <stdio.h>
#include <stdint.h>
#include <string.h>

/* Classic DHCP only, no IPv6! */

typedef struct
{
	uint8_t op;        /* 0x02. */
	uint8_t htype;     /* 0x01. */
	uint8_t hlen;
	uint8_t hops;
	uint32_t xid;
	uint16_t secs;
	uint16_t flags;
	unsigned char ciaddr[4];
	unsigned char yiaddr[4];
	unsigned char siaddr[4];
	unsigned char giaddr[4];
	unsigned char chaddr[16];   /* First 6 bytes used for MAC address of client. */
	unsigned char sname[64];    /* Server host name (BOOTP). */
	unsigned char file[128];    /* Boot file name (BOOTP). */
	unsigned char magic[4];     /* 0x63825363. */
} dhcp_msg;

typedef struct
{
	uint8_t type;
	uint8_t length;
	unsigned char value[256];
} dhcp_opt;

/* The lease stored in /var/lib/dhcpcd/ is usually a DHCP ACK message. */
struct DHCPlease
{
	dhcp_msg msg;
	dhcp_opt opts[256]; /* List of options from TLV encoding. */
	uint8_t optcount;
};

int bytes_to_int(unsigned char *bytes)
{
	return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | (bytes[3]);
}

int print_ip_address(char *title, unsigned char *bytes)
{
	char *end = ": %d.%d.%d.%d\n";
	int len = strlen(end)+strlen(title)+1;
	char format[len];
	strncpy(format, title, strlen(title)+1);
	strcat(format, end);
	printf(format, bytes[0], bytes[1], bytes[2], bytes[3]);
}

int print_mac_address(char *title, unsigned char *bytes)
{
	char *end = ": %x:%x:%x:%x:%x:%x\n";
	int len = strlen(end)+strlen(title)+1;
	char format[len];
	strncpy(format, title, strlen(title)+1);
	strcat(format, end);
	printf(format, bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
}

char* fix_ds_dots(char *ds, int len)
{
	int i;
	for(i = 0; i < len; i++) {
		if(ds[i] == 0x02) {
			ds[i] = 0x2e;
		}
	}
	return ds;
}

void print_usage()
{
	printf("Usage   : dhcplease <DHCP Lease File>\n");
	printf("Example : dhcplease /var/lib/dhcpcd/dhcpcd-wlp3s0.lease\n");
	printf("Also    : dhcplease --help\n");
	printf("Also    : dhcplease --version\n");
}

void print_version()
{
	printf("dhcplease (DHCP Lease Parser) 1.0.0\n");
}
 
int main(int argc, char **argv)
{
	/* Necessary variables. */
	FILE *file;
	struct DHCPlease dhcp_lease;
	int i, aux;
	
	/* Argument count check. */
	if(argc < 2) {
		printf("Not enough arguments.\n");
		print_usage();
		return 1;
	}
	
	/* --help argument. */
	if(strcmp(argv[1], "--help") == 0) {
		print_usage();
		return 0;
	}
	if(strcmp(argv[1], "--version") == 0) {
		print_version();
		return 0;
	}

	/* Lease file opening. */
	file = fopen(argv[1], "r");
	if(!file) {
		printf("Fail when opening file.\n");
		return 2;
	}

	/* Lease file reading and consequent storing. */
	fread(&dhcp_lease, sizeof(dhcp_msg), 1, file);
	for(i = 0; !feof(file); i++) {
		fread(&dhcp_lease.opts[i].type, 1, 1, file);
		fread(&dhcp_lease.opts[i].length, 1, 1, file);
		fread(&dhcp_lease.opts[i].value, dhcp_lease.opts[i].length, 1, file);		
	}
	dhcp_lease.optcount = i;
	fclose(file);
	
	/* Correctness and consistency checks (there's plenty of room for improvements here). */
	if(dhcp_lease.msg.op != 0x02 || dhcp_lease.msg.htype != 0x01 || bytes_to_int(dhcp_lease.msg.magic) != 0x63825363) {
		printf("Bad DHCP lease file format.\n");
		return 3;
	}
	
	/* Pretty print lease data. */
	print_ip_address("Your IP Address        ", dhcp_lease.msg.yiaddr);
	print_mac_address("Your MAC Address       ", dhcp_lease.msg.chaddr);
	for(i = 0; i < dhcp_lease.optcount; i++) {
		switch(dhcp_lease.opts[i].type) {
			case 1: /* Subnet Mask. */
				print_ip_address("Subnet Mask            ", dhcp_lease.opts[i].value);
				break;
			case 3: /* Router. */
				print_ip_address("Router IP Address      ", dhcp_lease.opts[i].value);
				break;
			case 6: /* Domain Name Server. */
				aux = 0;
				do {
					print_ip_address("DNS Server IP Address  ", dhcp_lease.opts[i].value+aux);
					aux+=4;
				} while(aux < dhcp_lease.opts[i].length);
				break;
			case 12: /* Host Name. */
				printf("Your Host Name         : %s\n", dhcp_lease.opts[i].value);
				break;
			case 15: /* Domain Name. */
				printf("Domain Name            : %s\n", dhcp_lease.opts[i].value);
				break;
			case 42: /* Network Time Protocol Servers. */
				aux = 0;
				do {
					print_ip_address("NTP Server IP Address  ", dhcp_lease.opts[i].value+aux);
					aux+=4;
				} while(aux < dhcp_lease.opts[i].length);
				break;
			case 51: /* IP Address Lease Time. */
				aux = bytes_to_int(dhcp_lease.opts[i].value);
				printf("IP Address Lease Time  : %d:%02d h (%d s)\n", aux/3600, (aux%3600)/60, aux);
				break;
			case 54: /* DHCP Server Identifier. */
				print_ip_address("DHCP Server IP Address ", dhcp_lease.opts[i].value);
				break;
			case 58: /* Renewal Time Value. */
				aux = bytes_to_int(dhcp_lease.opts[i].value);
				printf("Renewal Time Value     : %d:%02d h (%d s)\n", aux/3600, (aux%3600)/60, aux);
				break;
			case 59: /* Rebinding Time Value. */
				aux = bytes_to_int(dhcp_lease.opts[i].value);
				printf("Rebinding Time Value   : %d:%02d h (%d s)\n", aux/3600, (aux%3600)/60, aux);
				break;
			case 119: /* Domain Name. */
				fix_ds_dots(dhcp_lease.opts[i].value, dhcp_lease.opts[i].length);
				printf("Domain Search          : %s\n", dhcp_lease.opts[i].value);
				break;
			default:
				break;
		}
	}
	
	/* All okay, bye. */
	return 0;
}
