#include "jni.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "udns.h"

extern "C" jobjectArray Java_org_sipdroid_net_DnsSrv_getHostSRV(JNIEnv* env, jclass clazz,
        jstring nameStr, jstring srvStr, jstring protoStr, jstring dnsStr0, jstring dnsStr1) {
    if (nameStr == NULL || srvStr == NULL || protoStr == NULL || (dnsStr0 == NULL && dnsStr1 == NULL)) {
        return NULL;
    }
	
	jobjectArray ret = NULL;
	struct dns_ctx *nctx = NULL;
	
	if (dns_init(NULL, 0) < 0 || !(nctx = dns_new(NULL)))
		return NULL;

    const char* name = env->GetStringUTFChars(nameStr, NULL);
	const char* srv = env->GetStringUTFChars(srvStr, NULL);
	const char* proto = env->GetStringUTFChars(protoStr, NULL);

	int r = 0;
	unsigned short port = dns_set_opt(nctx, DNS_OPT_PORT, -1);

	dns_add_serv(nctx, NULL);

	if(dnsStr0 != NULL) {
		const char* dns = env->GetStringUTFChars(dnsStr0, NULL);
		union {
		  struct sockaddr_in addr;
		  struct sockaddr_in6 addr6;
		} a;
		memset(&a, 0, sizeof(struct sockaddr_in6));

		int iptype = 0;

		if (dns_pton(AF_INET, dns, &a.addr.sin_addr) > 0) {
			iptype = 1;
			a.addr.sin_family = AF_INET;
			a.addr.sin_port = htons(port);
		}
	#ifdef HAVE_IPv6
		else if (dns_pton(AF_INET6, dns, &a.addr6.sin6_addr) > 0) {
			iptype = 1;
			a.addr6.sin6_family = AF_INET6;
			a.addr6.sin6_port = htons(port);
		}
	#endif
		env->ReleaseStringUTFChars(dnsStr0, dns);

		if(iptype)
			r = dns_add_serv_s(nctx, (struct sockaddr *)&a);
	}
	
	if(dnsStr1 != NULL) {
		const char* dns = env->GetStringUTFChars(dnsStr1, NULL);
		union {
		  struct sockaddr_in addr;
		  struct sockaddr_in6 addr6;
		} a;
		int iptype = 0;

		if (dns_pton(AF_INET, dns, &a.addr.sin_addr) > 0) {
			iptype = 1;
			a.addr.sin_family = AF_INET;
			a.addr.sin_port = htons(port);
		}
	#ifdef HAVE_IPv6
		else if (dns_pton(AF_INET6, dns, &a.addr6.sin6_addr) > 0) {
			iptype = 1;
			a.addr6.sin6_family = AF_INET6;
			a.addr6.sin6_port = htons(port);
		}
	#endif
		env->ReleaseStringUTFChars(dnsStr1, dns);

		if(iptype)
			r = dns_add_serv_s(nctx, (struct sockaddr *)&a);
	}

	if(r > 0) {
		if (dns_open(nctx) < 0)
			return NULL;

		struct dns_rr_srv *rrsrv = dns_resolve_srv(nctx, name, srv, proto, 0);
		if(rrsrv) {
			ret = (jobjectArray)env->NewObjectArray(rrsrv->dnssrv_nrr, env->FindClass("java/lang/String"), env->NewStringUTF(""));
			char buf[DNS_MAXNAME*2];
			for (int i = 0; i < rrsrv->dnssrv_nrr; i++) {				
				struct dns_srv* s = &rrsrv->dnssrv_srv[i];
				sprintf(buf, "%s,%d,%d,%d", s->name, s->port, s->priority, s->weight);
				env->SetObjectArrayElement(ret, i, env->NewStringUTF(buf));
			}
			free(rrsrv);
		}
	}
	
	env->ReleaseStringUTFChars(nameStr, name);
	env->ReleaseStringUTFChars(srvStr, srv);
	env->ReleaseStringUTFChars(protoStr, proto);
	
	dns_free(nctx);
	
	return ret;
}
