athena_solaris:
	make -f Makefile.generic ${WHAT} \
		LIBS="-ltermlib ../libtelnet/libtelnet.a \
			/usr/athena/lib/libresolv.a -lsocket -lnsl -lc \
			/usr/ucblib/libucb.a -lelf -ldl ${AUTH_LIB}" \
		LIBPATH="/usr/ccs/lib/libtermlib.a ../libtelnet/libtelnet.a \
			/usr/lib/libc.a /usr/ucblib/libucb.a \
			/usr/lib/libsocket.a /usr/lib/libnsl.a ${AUTH_LIBPATH}" \
		DEST=${DESTDIR}/usr/athena/bin \
		DEFINES="-DFILIO_H -DUSE_TERMIO -DKLUDGELINEMODE \
			-DSTREAMS -DSTREAMSPTY -DDIAGNOSTICS -DSOLARIS \
			-DENV_HACK -DOLD_ENVIRON -DNO_LOGIN_P -DUTMPX \
	-DDEFAULT_IM='\"\r\nMIT Athena (%h/Solaris) (%t)\r\n\r\r\n\r\"' \
			-DLOGIN_ARGS ${AUTH_DEF}" \
		INCLUDES="-I..  -I../.. ${AUTH_INC} -I/usr/ucbinclude" \
		LIB_OBJ="getent.o strerror.o setenv.o herror.o" \
		LIB_SRC="getent.c strerror.c setenv.c herror.c" \
		AR=ar ARFLAGS=cq RANLIB=NONE \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="ATHENA_LCCFLAGS"

athena_solaris.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L../../AL -L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod -lnsl -lsocket -lresolv" \
		AUTH_LIBPATH="../../AL/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_INC=-I/usr/athena/include \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 -DDES_ENCRYPTION -DATHENA_LOGIN"

athena_aix:
	make -f Makefile.generic ${WHAT} \
		LIBS="../libtelnet/libtelnet.a -lbsd\
			-lcurses ${AUTH_LIB}" \
		LIBPATH="../libtelnet/libtelnet.a /usr/lib/libbsd.a \
			 /usr/lib/libcurses.a ${AUTH_LIBPATH}" \
		DEST=${DESTDIR}/usr/athena/bin \
		DEFINES="-DUSE_TERMIO -DKLUDGELINEMODE \
			-DSTREAMS -DDIAGNOSTICS \
			-DOLD_ENVIRON -Dunix \
	-DDEFAULT_IM='\"\r\nMIT Athena (%h/AIX) (%t)\r\n\r\r\n\r\"' \
			-DLOGIN_ARGS ${AUTH_DEF}" \
		INCLUDES="-I..  -I../.. ${AUTH_INC}" \
		LIB_OBJ="getent.o setenv.o" \
		LIB_SRC="getent.c setenv.c" \
		AR=ar ARFLAGS=cq RANLIB=ranlib \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="ATHENA_LCCFLAGS"

athena_aix.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L../../AL -L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod" \
		AUTH_LIBPATH="../../AL/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_INC=-I/usr/athena/include \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 -DDES_ENCRYPTION -DATHENA_LOGIN"

athena_ultrix:
	make -f Makefile.generic ${WHAT} \
		LIBS="-ltermlib -lcursesX ../libtelnet/libtelnet.a ${AUTH_LIB}" \
		LIBPATH="/lib/libc.a /usr/lib/libtermlib.a /usr/lib/libcursesX.a \
			../libtelnet/libtelnet.a ${AUTH_LIBPATH}" \
		DEST=${DESTDIR}/usr/athena/bin \
		DEFINES=${ODEFS}" \
	-DDEFAULT_IM='\"\r\nMIT Athena (%h/Ultrix) (%t)\r\n\r\r\n\r\"' \
			-DKLUDGELINEMODE -DDIAGNOSTICS \
			-DENV_HACK -DOLD_ENVIRON ${AUTH_DEF}" \
		INCLUDES="-I.. -I../.. -I/usr/athena/include" \
		LIB_OBJ="getent.o strdup.o" \
		LIB_SRC="getent.c strdup.c" \
		AR=ar ARFLAGS=cq RANLIB=ranlib \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="ATHENA_LCCFLAGS"

athena_ultrix.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L../../AL -L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod" \
		AUTH_LIBPATH="../../AL/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 -DDES_ENCRYPTION -DATHENA_LOGIN"
