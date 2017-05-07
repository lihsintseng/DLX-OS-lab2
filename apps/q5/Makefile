default:
	cd makeprocs; make
	cd nReady; make
	cd makeN2; make
	cd oReady; make
	cd makeO2; make

clean:
	cd makeprocs; make clean
	cd nReady; make clean
	cd makeN2; make clean
	cd oReady; make clean
	cd makeO2; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u krypton.dlx.obj 5 13 7; ee469_fixterminal
