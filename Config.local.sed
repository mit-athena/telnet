athena_solaris:
	make -f Makefile.generic ${WHAT} \
		LIBS="-ltermlib ../libtelnet/libtelnet.a \
		  ${AUTH_LIB} -lsocket -lnsl -lbsd" \
		LIBPATH="/usr/ccs/lib/libtermlib.a ../libtelnet/libtelnet.a \
			${AUTH_LIBPATH} /usr/lib/libsocket.a \
			/usr/lib/libnsl.a /usr/athena/lib/libbsd.a" \
		DEST=${DESTDIR}/usr/athena/bin \
		DEFINES="-DFILIO_H -DUSE_TERMIO -DKLUDGELINEMODE \
			-DSTREAMS -DSTREAMSPTY -DDIAGNOSTICS -DSOLARIS \
			-DENV_HACK -DOLD_ENVIRON -DUTMPX \
	-DDEFAULT_IM='\"\r\nMIT Athena (%h/Solaris) (%t)\r\n\r\r\n\r\"' \
			-DLOGIN_ARGS ${AUTH_DEF}" \
		INCLUDES="-I..  ${AUTH_INC} -I/usr/ucbinclude" \
		LIB_OBJ="getent.o strerror.o setenv.o herror.o" \
		LIB_SRC="getent.c strerror.c setenv.c herror.c" \
		AR=ar ARFLAGS=cq RANLIB=NONE \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="ATHENA_LCCFLAGS"

athena_solaris.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod -lnsl -lsocket -lresolv" \
		AUTH_LIBPATH="/usr/athena/lib/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
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
		INCLUDES="-I..  ${AUTH_INC}" \
		LIB_OBJ="getent.o setenv.o" \
		LIB_SRC="getent.c setenv.c" \
		AR=ar ARFLAGS=cq RANLIB=ranlib \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="ATHENA_LCCFLAGS"

athena_aix.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod" \
		AUTH_LIBPATH="/usr/athena/lib/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
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
		INCLUDES="-I.. -I/usr/athena/include" \
		LIB_OBJ="getent.o strdup.o" \
		LIB_SRC="getent.c strdup.c" \
		AR=ar ARFLAGS=cq RANLIB=ranlib \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="ATHENA_LCCFLAGS"

athena_ultrix.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod" \
		AUTH_LIBPATH="/usr/athena/lib/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 -DDES_ENCRYPTION -DATHENA_LOGIN"

athena_hpux:
	make -f Makefile.generic ${WHAT} \
		LIBS="-lcurses ../libtelnet/libtelnet.a ${AUTH_LIB}" \
		LIBPATH="/lib/libc.a /lib/libcurses.sl \
				../libtelnet/libtelnet.a ${AUTH_LIBPATH}" \
		DEST=${DESTDIR}/usr/bin \
		DEFINES=${ODEFS}"-Dvfork=fork -DUSE_TERMIO \
	-DDEFAULT_IM='\"\r\nMIT Athena (%h/HP-UX) (%t)\r\n\r\r\n\r\"' \
			-DNO_LOGIN_F -DNO_LOGIN_P -DNO_LOGIN_H \
			-DDIAGNOSTICS -DLOGIN_ARGS ${AUTH_DEF}" \
		INCLUDES="-I.. -I/usr/athena/include" \
		LIB_OBJ="getent.o setenv.o" \
		LIB_SRC="getent.c setenv.c" \
		AR=ar ARFLAGS=cq RANLIB=NONE \
		LIBEXEC=${DESTDIR}/etc \
		CC="${CC}" LCCFLAGS="-O"

athena_hpux.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod" \
		AUTH_LIBPATH="/usr/athena/lib/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 -DDES_ENCRYPTION -DATHENA_LOGIN"

athena_linux:
	make -f Makefile.generic ${WHAT} \
		LIBS="-ltermcap ../libtelnet/libtelnet.a -lbsd \
			${AUTH_LIB}" \
		LIBPATH="/usr/lib/libc.a /usr/lib/libtermcap.a \
			../libtelnet/libtelnet.a /usr/lib/libbsd.a \
			${AUTH_LIBPATH}" \
		DEST=${DESTDIR}/usr/athena/bin \
		DEFINES=${ODEFS}" -DTERMCAP -DUSE_TERMIO \
	-DDEFAULT_IM='\"\r\nMIT SIPB Linux-Athena (%h) (%t)\r\n\r\n\"' \
			-DKLUDGELINEMODE -DDIAGNOSTICS -DENV_HACK \
			-DOLD_ENVIRON ${AUTH_DEF}" \
		INCLUDES="-I.. ${AUTH_INC} -I/usr/ucbinclude" \
		LIB_OBJ="getent.o" \
		LIB_SRC="getent.c" \
		AR=ar ARFLAGS=cq RANLIB=ranlib \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="ATHENA_LCCFLAGS -g"

athena_linux.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L/usr/athena/lib -lAL -lkrb -ldes \
			-lcom_err -lhesiod" \
		AUTH_LIBPATH="/usr/athena/lib/libAL.a \
			/usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 \
			-DDES_ENCRYPTION -DATHENA_LOGIN"

athena_osf1:
	make -f Makefile.generic ${WHAT} \
		LIBS="-lutil -ltermcap ../libtelnet/libtelnet.a ${AUTH_LIB}" \
		LIBPATH="/usr/lib/libc.a /usr/lib/libtermcap.a \
				../libtelnet/libtelnet.a ${AUTH_LIBPATH}" \
		DEST=${DESTDIR}/usr/athena/bin \
		DEFINES=${ODEFS}"-DTERMCAP -DKLUDGELINEMODE \
		    -DDEFAULT_IM='\"\r\nMIT Athena (%h/OSF/1) (%t)\r\n\r\r\n\r\"' \
			-DUSE_TERMIO -DDIAGNOSTICS -DENV_HACK \
			-DOLD_ENVIRON ${AUTH_DEF}" \
		INCLUDES="-I.. ${AUTH_INC}"\
		LIB_OBJ="getent.o" \
		LIB_SRC="getent.c" \
		AR=ar ARFLAGS=cq RANLIB=ranlib \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="-O -std1 -Olimit 602 -Dunix"
# The -Dunix is for the -std1

athena_osf1.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod" \
		AUTH_LIBPATH="/usr/athena/lib/libAL.h /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_INC=-I/usr/athena/include \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 -DDES_ENCRYPTION -DATHENA_LOGIN"


athena_irix:
	make -f Makefile.generic ${WHAT} \
		LIBS="-ltermlib ../libtelnet/libtelnet.a ${AUTH_LIB}" \
		LIBPATH="/usr/lib/libtermlib.a \
			../libtelnet/libtelnet.a ${AUTH_LIBPATH}" \
		DEST=${DESTDIR}/usr/athena/bin \
		DEFINES=${ODEFS}"-Dvfork=fork -DUSE_TERMIO -DUTMPX \
		-DKLUDGELINEMODE -DSTREAMS -DDIAGNOSTICS \
		-DENV_HACK -DOLD_ENVIRON \
		    -DDEFAULT_IM='\"\r\nMIT Athena (%h/Irix) (%t)\r\n\r\r\n\r\"' \
			${AUTH_DEF}" \
		INCLUDES="-I.. ${AUTH_INC}" \
		LIB_OBJ="getent.o setenv.o" \
		LIB_SRC="getent.c setenv.c" \
		AR=ar ARFLAGS=cq RANLIB=NONE \
		LIBEXEC=${DESTDIR}/etc/athena \
		CC="${CC}" LCCFLAGS="-O2  -DSYSV -DPOSIX -DPOSIX_FLOCK"


athena_irix.auth:
	make -f ../Config.local `basename $@ .auth` WHAT=${WHAT} \
		AUTH_LIB="-L/usr/athena/lib -lAL -lkrb -ldes -lcom_err -lhesiod" \
		AUTH_LIBPATH="/usr/athena/lib/libAL.a /usr/athena/lib/libkrb.a /usr/athena/lib/libdes.a" \
		AUTH_INC=-I/usr/athena/include \
		AUTH_DEF="-DAUTHENTICATION -DENCRYPTION -DKRB4 -DDES_ENCRYPTION -DATHENA_LOGIN"
