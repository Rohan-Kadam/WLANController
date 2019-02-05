#ifndef _BUFFER_MODIFIER_
#define _BUFFER_MODIFIER_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/xmlIO.h>
#include <libxml/xinclude.h>
#include <libxml/tree.h>

static void parse_buffer(xmlDocPtr doc, xmlNodePtr curr);
static void parse_init(xmlDocPtr doc, xmlNodePtr cur);
int buffer_main(unsigned char* bssid,unsigned char* dest_addr,unsigned char* source_addr);

#endif
