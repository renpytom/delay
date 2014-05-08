AUTOMAKE_OPTIONS=foreign
YFLAGS=-d
SUFFIXES=.1 .pod

%.1: %.pod
	pod2man --center="OneGeek Software" --release="Delay $(VERSION)" $+ > $@ 2>/dev/null

bin_PROGRAMS = delay
delay_SOURCES = delay.c curses.c parsetimey.y parsetimel.l


man_MANS = delay.1
EXTRA_DIST = INSTALL.autoconf delay.1 parsetime.h parsetimey.h ChangeLog \
	delay.pod 

# debian/changelog debian/control debian/copyright \
#	debian/dirs debian/rules

# Debian is smart about packaging. Just call 'debuild' to take care of
# it.
# dpkg: distdir
#	cd $(distdir) && debuild -us -uc 
#	-rm -rf $(distdir)

# Rpm wants to know what platform you're on. Since it's not smart enough
# to create the appropriate directory on its own, we have to hardcode
# it here.
#RPM_PLATFORM=i386

#rpm: dist
#	mkdir -p redhat
#	mkdir -p redhat/BUILD
#	mkdir -p redhat/RPMS/$(RPM_PLATFORM)
#	mkdir -p redhat/SOURCES
#	mkdir -p redhat/SPECS
#	mkdir -p redhat/SRPMS
#	cp delay-$(VERSION).tar.gz redhat/SOURCES/
#	cp delay.spec redhat/SPECS/
#	rpm --define "_topdir `pwd`/redhat" -ba redhat/SPECS/delay.spec
#	cp redhat/RPMS/$(RPM_PLATFORM)/* .
#	cp redhat/SRPMS/* .
#	-rm -rf redhat

#clean-local:
#	-rm delay-$(VERSION)*
#	-rm delay_$(VERSION)*
