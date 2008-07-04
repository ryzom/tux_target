
# Global variables
include ../Variables.mk

all:
	$(MAKE) -C src all

clean:
	-$(RM) core* *.log *~ .#* *.csv
	-$(MAKE) -C src clean

update:
	make -C src update
