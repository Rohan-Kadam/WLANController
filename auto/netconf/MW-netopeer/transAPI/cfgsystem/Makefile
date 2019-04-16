PKGNAME = cfgsystem
TARGET = cfgsystem
MODULE = $(TARGET).la

# Various configurable paths (remember to edit Makefile.in, not Makefile)
prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
datadir = ${datarootdir}
bindir = ${exec_prefix}/bin
includedir = ${prefix}/include
libdir =  ${exec_prefix}/lib
mandir = ${datarootdir}/man
libtool = ./libtool
sysconfdir = ${prefix}/etc
NETOPEER_DIR = ${prefix}/etc/netopeer/

CC = gcc
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
LIBS = -lxml2 -lcrypt  -L/usr/local/lib -lnetconf -laugeas
CFLAGS = -Wall -Wno-unused-result -I/usr/include/libxml2  -O3 -DNDEBUG -fvisibility=hidden
CPPFLAGS = -I/usr/local/include -DPUBLIC=__attribute__\(\(visibility\(\"default\"\)\)\) -I/usr/local/include -I/usr/include/libxml2 -DHAVE_L64A -DNETOPEER_DIR=\"$(NETOPEER_DIR)\"
LIBTOOL = $(libtool) --tag=CC --quiet
NETOPEER_MANAGER = 

MODEL = model/ietf-system.yin \
	model/iana-crypt-hash.yin \
	model/ietf-system-config.rng \
	model/ietf-system-gdefs-config.rng \
	model/ietf-system-schematron.xsl

SRCS = $(TARGET).c \
	base/common.c \
	base/date_time.c \
	base/platform.c \
	base/dns_resolver.c \
	base/shutdown.c \
	base/encrypt.c \
	base/salt.c \
	base/local_users.c

OBJDIR = .obj
LOBJS = $(SRCS:%.c=$(OBJDIR)/%.lo)

all: $(MODULE) $(TARGET)-init

$(TARGET)-init: $(SRCS) $(TARGET)-init.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^ $(LIBS)

$(MODULE): $(LOBJS)
	$(LIBTOOL) --mode=link $(CC) $(CFLAGS) $(CPPFLAGS) $(LIBS) -avoid-version -module -shared -export-dynamic --mode=link -o $@ $^ -rpath $(libdir)

$(OBJDIR)/%.lo: %.c
	@[ -d $$(dirname $@) ] || \
		(mkdir -p $$(dirname $@))
	$(LIBTOOL) --mode=compile $(CC) $(CFLAGS) $(CPPFLAGS) -fPIC -shared -c $< -o $@

.PHONY: install
install: $(MODULE) $(TARGET)-init
	$(INSTALL) -m 775 -d $(DESTDIR)/$(libdir)
	$(LIBTOOL) --mode=install cp $(MODULE) $(DESTDIR)/$(libdir)/;
	$(INSTALL) -d $(NETOPEER_DIR)/ietf-system/
	@for i in $(MODEL); do \
		$(INSTALL_DATA) -m 600 $$i $(NETOPEER_DIR)/ietf-system/; \
	done
	if test -n "$(NETOPEER_MANAGER)"; then \
		if test -n "`$(NETOPEER_MANAGER) list | grep "^ietf-system ("`"; then \
			$(NETOPEER_MANAGER) rm --name ietf-system; \
		fi; \
		$(NETOPEER_MANAGER) add --name ietf-system \
			--model $(NETOPEER_DIR)/ietf-system/ietf-system.yin \
			--transapi $(DESTDIR)/$(libdir)/cfgsystem.so \
			--features timezone-name authentication local-users \
			--datastore $(NETOPEER_DIR)/ietf-system/datastore.xml; \
	fi; \
	./$(TARGET)-init $(NETOPEER_DIR)/ietf-system/datastore.xml timezone-name authentication local-users

.PHONY: uninstall
uninstall:
	$(LIBTOOL) --mode=uninstall rm -rf $(DESTDIR)/$(libdir)/$(MODULE);
	rm -rf $(NETOPEER_DIR)/ietf-system/
	if test -n "$(NETOPEER_MANAGER)"; then \
		if test -n "`$(NETOPEER_MANAGER) list | grep "^ietf-system ("`"; then \
			$(NETOPEER_MANAGER) rm --name ietf-system; \
		fi; \
	fi

.PHONY: clean
clean:
	$(LIBTOOL) --mode clean rm -f $(LOBJS)
	$(LIBTOOL) --mode clean rm -f $(MODULE)
	rm -rf $(MODULE) $(TARGET)-init $(OBJDIR)

