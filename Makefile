#!/usr/bin/make -f
#
# Build ioctlsave.
#
# Copyright � AARNet Pty Ltd (ACN 084 540 518).
# Australian Academic & Research Network, http://www.aarnet.edu.au/
# Licensed to the public under the GNU General Public License Version Two,
# see http://www.gnu.org/licenses/gpl.html. AARNet members may have
# additional license choices.
#
# Written by Glen Turner, <http://www.gdt.id.au/~gdt/>
# 
# $Id$

# Compile and link options for production
CFLAGS=-O2
LDFLAGS=-s
# and for debugging.
#  CFLAGS=-g
#  LDFLAGS=

# The manual page uses DocBook as its source format.
# OpenJade with JadeWrapper produces the manual page.
DB2MAN=jw
DB2MANFLAGS=--frontend docbook --backend man --nochunks

# Installation prefix
PREFIX=/usr/local
PREFIXBIN=$(PREFIX)/bin
PREFIXMAN=$(PREFIX)/man
PREFIXMAN1=$(PREFIXMAN)/man1

# Assuming BSD-like install
INSTALL=install
INSTALLFLAGS=-o bin -g bin
INSTALLFLAGSBIN=-m 0755
INSTALLFLAGSMAN=-m 0644


all: ioctlsave ioctlsave.1

ioctlsave: ioctlsave.o

ioctlsave.o: ioctlsave.c

ioctlsave.1: ioctlsave.1.sgml
	$(DB2MAN) $(DB2MANFLAGS) ioctlsave.1.sgml

.PHONY:	install
install: ioctlsave ioctlsave.1
	$(INSTALL) $(INSTALLFLAGS) $(INSTALLFLAGSBIN) ioctlsave $(PREFIXBIN)
	$(INSTALL) $(INSTALLFLAGS) $(INSTALLFLAGSMAN) ioctlsave.1 $(PREFIXMAN1)

.PHONY: clean
clean:
	$(RM) $(RMFLAGS) ioctlsave ioctlsave.o ioctlsave.1

# EOF
