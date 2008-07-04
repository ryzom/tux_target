
all:
	make -C client all
	make -C server all
#	make -C login_service all

clean:
	-rm *~
	-rm *.log
	-rm core*
	-make -C client clean
	-make -C server clean
	-make -C login_service clean

update:
	-make -C client update
	-make -C server update
