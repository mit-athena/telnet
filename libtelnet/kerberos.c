/*-
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static char sccsid[] = "@(#)kerberos.c	8.1 (Berkeley) 6/4/93";
#endif /* not lint */

/*
 * Copyright (C) 1990 by the Massachusetts Institute of Technology
 *
 * Export of this software from the United States of America is assumed
 * to require a specific license from the United States Government.
 * It is the responsibility of any person or organization contemplating
 * export to obtain such a license before exporting.
 *
 * WITHIN THAT CONSTRAINT, permission to use, copy, modify, and
 * distribute this software and its documentation for any purpose and
 * without fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright notice and
 * this permission notice appear in supporting documentation, and that
 * the name of M.I.T. not be used in advertising or publicity pertaining
 * to distribution of the software without specific, written prior
 * permission.  M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 */

#ifdef	KRB4
#include <sys/types.h>
#include <arpa/telnet.h>
#include <stdio.h>
#include <des.h>        /* BSD wont include this in krb.h, so we do it here */
#include <krb.h>
#ifdef	__STDC__
#include <stdlib.h>
#endif
#ifdef	NO_STRING_H
#include <strings.h>
#else
#include <string.h>
#endif
#ifdef ATHENA_LOGIN
#include <AL/AL.h>
#endif

#include "encrypt.h"
#include "auth.h"
#include "misc.h"

int kerberos4_cksum P((unsigned char *, int));
int krb_mk_req P((KTEXT, char *, char *, char *, u_long));
int krb_rd_req P((KTEXT, char *, char *, u_long, AUTH_DAT *, char *));
int krb_kntoln P((AUTH_DAT *, char *));
int krb_get_cred P((char *, char *, char *, CREDENTIALS *));
int krb_get_lrealm P((char *, int));
int kuserok P((AUTH_DAT *, char *));

extern auth_debug_mode;

static unsigned char str_data[1024] = { IAC, SB, TELOPT_AUTHENTICATION, 0,
			  		AUTHTYPE_KERBEROS_V4, };
static unsigned char str_name[1024] = { IAC, SB, TELOPT_AUTHENTICATION,
					TELQUAL_NAME, };

#define	KRB_AUTH	0		/* Authentication data follows */
#define	KRB_REJECT	1		/* Rejected (reason might follow) */
#define	KRB_ACCEPT	2		/* Accepted */
#define	KRB_CHALLENGE	3		/* Challenge for mutual auth. */
#define	KRB_RESPONSE	4		/* Response for mutual auth. */

#define KRB_SERVICE_NAME   "rcmd"

static	KTEXT_ST auth;
static	char name[ANAME_SZ];
static	AUTH_DAT adat = { 0 };
#ifdef	ENCRYPTION
static Block	session_key	= { 0 };
static Schedule sched;
static Block	challenge	= { 0 };
#endif	/* ENCRYPTION */

#ifdef ATHENA_LOGIN
static int DidStartUser=0;

int
AthenaUserOk(kdata, username)
     AUTH_DAT *kdata;
     char *username;
{
  long code;

  if (!DidStartUser)
    {
      code=ALsetUser(&AthenaLoginSession, username, ALisRemoteSession);
      if (code) com_err("telnetd", code, "(%s)",
			ALcontext(&AthenaLoginSession));
      if (ALisError(code)) return 1;

      code=ALstart(&AthenaLoginSession);
      if (code) com_err("telnetd", code, "(%s)",
			ALcontext(&AthenaLoginSession));
      if (ALisError(code)) return 1;

      if (ALisTrue(&AthenaLoginSession, ALdidCreateHomedir))
	setenv("TMPHOME", "", 1);
    }

  DidStartUser=1;
  return(kuserok(kdata, username));
}
#define kuserok(k, u) AthenaUserOk(k, u)

void
AthenaLoginCleanup()
{
  if (DidStartUser) ALend(&AthenaLoginSession);
  return;
}

#endif /* ATHENA_LOGIN */

	static int
Data(ap, type, d, c)
	Authenticator *ap;
	int type;
	void *d;
	int c;
{
        unsigned char *p = str_data + 4;
	unsigned char *cd = (unsigned char *)d;

	if (c == -1)
		c = strlen((char *)cd);

        if (auth_debug_mode) {
                printf("%s:%d: [%d] (%d)",
                        str_data[3] == TELQUAL_IS ? ">>>IS" : ">>>REPLY",
                        str_data[3],
                        type, c);
                printd(d, c);
                printf("\r\n");
        }
	*p++ = ap->type;
	*p++ = ap->way;
	*p++ = type;
        while (c-- > 0) {
                if ((*p++ = *cd++) == IAC)
                        *p++ = IAC;
        }
        *p++ = IAC;
        *p++ = SE;
	if (str_data[3] == TELQUAL_IS)
		printsub('>', &str_data[2], p - (&str_data[2]));
        return(net_write(str_data, p - str_data));
}

	int
kerberos4_init(ap, server)
	Authenticator *ap;
	int server;
{
	FILE *fp;

	if (server) {
		str_data[3] = TELQUAL_REPLY;
		if ((fp = fopen(KEYFILE, "r")) == NULL)
			return(0);
		fclose(fp);
	} else {
		str_data[3] = TELQUAL_IS;
	}
	return(1);
}

char dst_realm_buf[REALM_SZ], *dest_realm = NULL;
int dst_realm_sz = REALM_SZ;

	int
kerberos4_send(ap)
	Authenticator *ap;
{
	KTEXT_ST auth;
#ifdef	ENCRYPTION
	Block enckey;
#endif	/* ENCRYPTION */
	char instance[INST_SZ];
	char *realm;
	char *krb_realmofhost();
	char *krb_get_phost();
	CREDENTIALS cred;
	int r;

	printf("[ Trying KERBEROS4 ... ]\n");	
	if (!UserNameRequested) {
		if (auth_debug_mode) {
			printf("Kerberos V4: no user name supplied\r\n");
		}
		return(0);
	}

	bzero(instance, sizeof(instance));

	if (realm = krb_get_phost(RemoteHostName))
		strncpy(instance, realm, sizeof(instance));

	instance[sizeof(instance)-1] = '\0';

	realm = dest_realm ? dest_realm : krb_realmofhost(RemoteHostName);

	if (!realm) {
		printf("Kerberos V4: no realm for %s\r\n", RemoteHostName);
		return(0);
	}
	if (r = krb_mk_req(&auth, KRB_SERVICE_NAME, instance, realm, 0L)) {
		printf("mk_req failed for %s.%s@%s: %s\r\n",
		       KRB_SERVICE_NAME, instance, realm, krb_err_txt[r]);
		return(0);
	}
	if (r = krb_get_cred(KRB_SERVICE_NAME, instance, realm, &cred)) {
		printf("get_cred failed for %s.%s@%s: %s\r\n",
		       KRB_SERVICE_NAME, instance, realm, krb_err_txt[r]);
		return(0);
	}
	if (!auth_sendname(UserNameRequested, strlen(UserNameRequested))) {
		if (auth_debug_mode)
			printf("Not enough room for user name\r\n");
		return(0);
	}
	if (auth_debug_mode)
		printf("Sent %d bytes of authentication data\r\n", auth.length);
	if (!Data(ap, KRB_AUTH, (void *)auth.dat, auth.length)) {
		if (auth_debug_mode)
			printf("Not enough room for authentication data\r\n");
		return(0);
	}
#ifdef	ENCRYPTION
	/*
	 * If we are doing mutual authentication, get set up to send
	 * the challenge, and verify it when the response comes back.
	 */
	if ((ap->way & AUTH_HOW_MASK) == AUTH_HOW_MUTUAL) {
		register int i;

		des_key_sched(cred.session, sched);
		des_set_random_generator_seed(cred.session);
		des_new_random_key(challenge);
		des_ecb_encrypt(challenge, session_key, sched, 1);
		/*
		 * Increment the challenge by 1, and encrypt it for
		 * later comparison.
		 */
		for (i = 7; i >= 0; --i) {
			register int x;
			x = (unsigned int)challenge[i] + 1;
			challenge[i] = x;	/* ignore overflow */
			if (x < 256)		/* if no overflow, all done */
				break;
		}
		des_ecb_encrypt(challenge, challenge, sched, 1);
	}
#endif	/* ENCRYPTION */
	
	if (auth_debug_mode) {
		printf("CK: %d:", kerberos4_cksum(auth.dat, auth.length));
		printd(auth.dat, auth.length);
		printf("\r\n");
		printf("Sent Kerberos V4 credentials to server\r\n");
	}
	return(1);
}

	void
kerberos4_is(ap, data, cnt)
	Authenticator *ap;
	unsigned char *data;
	int cnt;
{
#ifdef	ENCRYPTION
	Session_Key skey;
	Block datablock;
#endif	/* ENCRYPTION */
	char realm[REALM_SZ];
	char instance[INST_SZ];
	int r;

	if (cnt-- < 1)
		return;
	switch (*data++) {
	case KRB_AUTH:
		if (krb_get_lrealm(realm, 1) != KSUCCESS) {
			Data(ap, KRB_REJECT, (void *)"No local V4 Realm.", -1);
			auth_finished(ap, AUTH_REJECT);
			if (auth_debug_mode)
				printf("No local realm\r\n");
			return;
		}
		bcopy((void *)data, (void *)auth.dat, auth.length = cnt);
		if (auth_debug_mode) {
			printf("Got %d bytes of authentication data\r\n", cnt);
			printf("CK: %d:", kerberos4_cksum(auth.dat, auth.length));
			printd(auth.dat, auth.length);
			printf("\r\n");
		}
		instance[0] = '*'; instance[1] = 0;
		if (r = krb_rd_req(&auth, KRB_SERVICE_NAME,
				   instance, 0, &adat, "")) {
			if (auth_debug_mode)
				printf("Kerberos failed him as %s\r\n", name);
			Data(ap, KRB_REJECT, (void *)krb_err_txt[r], -1);
			auth_finished(ap, AUTH_REJECT);
			return;
		}
#ifdef	ENCRYPTION
		bcopy((void *)adat.session, (void *)session_key, sizeof(Block));
#endif	/* ENCRYPTION */
		krb_kntoln(&adat, name);

		if (UserNameRequested && !kuserok(&adat, UserNameRequested))
			Data(ap, KRB_ACCEPT, (void *)0, 0);
		else
			Data(ap, KRB_REJECT,
				(void *)"user is not authorized", -1);
		auth_finished(ap, AUTH_USER);
		break;

	case KRB_CHALLENGE:
#ifndef	ENCRYPTION
		Data(ap, KRB_RESPONSE, (void *)0, 0);
#else	/* ENCRYPTION */
		if (!VALIDKEY(session_key)) {
			/*
			 * We don't have a valid session key, so just
			 * send back a response with an empty session
			 * key.
			 */
			Data(ap, KRB_RESPONSE, (void *)0, 0);
			break;
		}

		des_key_sched(session_key, sched);
		bcopy((void *)data, (void *)datablock, sizeof(Block));
		/*
		 * Take the received encrypted challenge, and encrypt
		 * it again to get a unique session_key for the
		 * ENCRYPT option.
		 */
		des_ecb_encrypt(datablock, session_key, sched, 1);
		skey.type = SK_DES;
		skey.length = 8;
		skey.data = session_key;
		encrypt_session_key(&skey, 1);
		/*
		 * Now decrypt the received encrypted challenge,
		 * increment by one, re-encrypt it and send it back.
		 */
		des_ecb_encrypt(datablock, challenge, sched, 0);
		for (r = 7; r >= 0; r++) {
			register int t;
			t = (unsigned int)challenge[r] + 1;
			challenge[r] = t;	/* ignore overflow */
			if (t < 256)		/* if no overflow, all done */
				break;
		}
		des_ecb_encrypt(challenge, challenge, sched, 1);
		Data(ap, KRB_RESPONSE, (void *)challenge, sizeof(challenge));
#endif	/* ENCRYPTION */
		break;

	default:
		if (auth_debug_mode)
			printf("Unknown Kerberos option %d\r\n", data[-1]);
		Data(ap, KRB_REJECT, 0, 0);
		break;
	}
}

	void
kerberos4_reply(ap, data, cnt)
	Authenticator *ap;
	unsigned char *data;
	int cnt;
{
#ifdef	ENCRYPTION
	Session_Key skey;
#endif	/* ENCRYPTION */

	if (cnt-- < 1)
		return;
	switch (*data++) {
	case KRB_REJECT:
		if (cnt > 0) {
			printf("[ Kerberos V4 refuses authentication because %.*s ]\r\n",
				cnt, data);
		} else
			printf("[ Kerberos V4 refuses authentication ]\r\n");
		auth_send_retry();
		return;
	case KRB_ACCEPT:
		printf("[ Kerberos V4 accepts you ]\n");
		if ((ap->way & AUTH_HOW_MASK) == AUTH_HOW_MUTUAL) {
			/*
			 * Send over the encrypted challenge.
		 	 */
#ifndef	ENCRYPTION
			Data(ap, KRB_CHALLENGE, (void *)0, 0);
#else	/* ENCRYPTION */
			Data(ap, KRB_CHALLENGE, (void *)session_key,
						sizeof(session_key));
			des_ecb_encrypt(session_key, session_key, sched, 1);
			skey.type = SK_DES;
			skey.length = 8;
			skey.data = session_key;
			encrypt_session_key(&skey, 0);
#endif	/* ENCRYPTION */
			return;
		}
		auth_finished(ap, AUTH_USER);
		return;
	case KRB_RESPONSE:
#ifdef	ENCRYPTION
		/*
		 * Verify that the response to the challenge is correct.
		 */
		if ((cnt != sizeof(Block)) ||
		    (0 != memcmp((void *)data, (void *)challenge,
						sizeof(challenge))))
		{
#endif	/* ENCRYPTION */
			printf("[ Kerberos V4 challenge failed!!! ]\r\n");
			auth_send_retry();
			return;
#ifdef	ENCRYPTION
		}
		printf("[ Kerberos V4 challenge successful ]\r\n");
		auth_finished(ap, AUTH_USER);
#endif	/* ENCRYPTION */
		break;
	default:
		if (auth_debug_mode)
			printf("Unknown Kerberos option %d\r\n", data[-1]);
		return;
	}
}

	int
kerberos4_status(ap, name, level)
	Authenticator *ap;
	char *name;
	int level;
{
	if (level < AUTH_USER)
		return(level);

	if (UserNameRequested && !kuserok(&adat, UserNameRequested)) {
		strcpy(name, UserNameRequested);
		return(AUTH_VALID);
	} else
		return(AUTH_USER);
}

#define	BUMP(buf, len)		while (*(buf)) {++(buf), --(len);}
#define	ADDC(buf, len, c)	if ((len) > 0) {*(buf)++ = (c); --(len);}

	void
kerberos4_printsub(data, cnt, buf, buflen)
	unsigned char *data, *buf;
	int cnt, buflen;
{
	char lbuf[32];
	register int i;

	buf[buflen-1] = '\0';		/* make sure its NULL terminated */
	buflen -= 1;

	switch(data[3]) {
	case KRB_REJECT:		/* Rejected (reason might follow) */
		strncpy((char *)buf, " REJECT ", buflen);
		goto common;

	case KRB_ACCEPT:		/* Accepted (name might follow) */
		strncpy((char *)buf, " ACCEPT ", buflen);
	common:
		BUMP(buf, buflen);
		if (cnt <= 4)
			break;
		ADDC(buf, buflen, '"');
		for (i = 4; i < cnt; i++)
			ADDC(buf, buflen, data[i]);
		ADDC(buf, buflen, '"');
		ADDC(buf, buflen, '\0');
		break;

	case KRB_AUTH:			/* Authentication data follows */
		strncpy((char *)buf, " AUTH", buflen);
		goto common2;

	case KRB_CHALLENGE:
		strncpy((char *)buf, " CHALLENGE", buflen);
		goto common2;

	case KRB_RESPONSE:
		strncpy((char *)buf, " RESPONSE", buflen);
		goto common2;

	default:
		sprintf(lbuf, " %d (unknown)", data[3]);
		strncpy((char *)buf, lbuf, buflen);
	common2:
		BUMP(buf, buflen);
		for (i = 4; i < cnt; i++) {
			sprintf(lbuf, " %d", data[i]);
			strncpy((char *)buf, lbuf, buflen);
			BUMP(buf, buflen);
		}
		break;
	}
}

	int
kerberos4_cksum(d, n)
	unsigned char *d;
	int n;
{
	int ck = 0;

	/*
	 * A comment is probably needed here for those not
	 * well versed in the "C" language.  Yes, this is
	 * supposed to be a "switch" with the body of the
	 * "switch" being a "while" statement.  The whole
	 * purpose of the switch is to allow us to jump into
	 * the middle of the while() loop, and then not have
	 * to do any more switch()s.
	 *
	 * Some compilers will spit out a warning message
	 * about the loop not being entered at the top.
	 */
	switch (n&03)
	while (n > 0) {
	case 0:
		ck ^= (int)*d++ << 24;
		--n;
	case 3:
		ck ^= (int)*d++ << 16;
		--n;
	case 2:
		ck ^= (int)*d++ << 8;
		--n;
	case 1:
		ck ^= (int)*d++;
		--n;
	}
	return(ck);
}
#endif

#ifdef notdef

prkey(msg, key)
	char *msg;
	unsigned char *key;
{
	register int i;
	printf("%s:", msg);
	for (i = 0; i < 8; i++)
		printf(" %3d", key[i]);
	printf("\r\n");
}
#endif
