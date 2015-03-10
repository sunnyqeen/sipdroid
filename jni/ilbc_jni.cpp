#include "jni.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
extern "C" {
#include "ilbc.h"
}

#define LOG_TAG "iLBC"


// the header length of the RTP frame (must skip when en/decoding)
#define	RTP_HDR_SIZE	12

const char *kInterfacePath = "org/sipdroid/pjlib/ilbc";

static int codec_open = 0;

static ilbc_encode_state_t Enc_Inst;
static ilbc_decode_state_t Dec_Inst;

const short MODE = 30;//30ms
const short BLOCK_LEN = 240;//MODE << 3;//240

extern "C"
JNIEXPORT jint JNICALL Java_org_sipdroid_codecs_iLBC_open
  (JNIEnv *env, jobject obj) {

  	if (codec_open++ != 0)
		return (jint)0;

    ilbc_encode_init(&Enc_Inst, MODE);
    ilbc_decode_init(&Dec_Inst, MODE, 0);
	
	return (jint)0;
}

extern "C"
JNIEXPORT void JNICALL Java_org_sipdroid_codecs_iLBC_close
    (JNIEnv *env, jobject obj) {

	if (--codec_open != 0)
		return;
}

extern "C"
JNIEXPORT jint JNICALL Java_org_sipdroid_codecs_iLBC_encode
    (JNIEnv *env, jobject obj, jshortArray lin, jint offset, jbyteArray encoded, jint size) {

	jshort pre_amp[BLOCK_LEN];	
	jbyte ilbc_data[ILBC_NO_OF_BYTES_MAX];

	int ret,i,frsz=BLOCK_LEN;

	unsigned int lin_pos = 0;
	
	if (!codec_open)
		return 0;

    for (i = 0; i < size; i+=BLOCK_LEN) {
		env->GetShortArrayRegion(lin, offset + i,frsz, pre_amp);
		
        ret = ilbc_encode(&Enc_Inst, (uint8_t*)ilbc_data, pre_amp, Enc_Inst.blockl);

        /* Write payload */		
		env->SetByteArrayRegion(encoded, RTP_HDR_SIZE+ lin_pos, ret, ilbc_data);
		lin_pos += ret;
    }

    return (jint)lin_pos;
}

extern "C"
JNIEXPORT jint JNICALL Java_org_sipdroid_codecs_iLBC_decode
    (JNIEnv *env, jobject obj, jbyteArray encoded, jshortArray lin, jint size) {

	jshort post_amp[BLOCK_LEN];
	jbyte ilbc_data[ILBC_NO_OF_BYTES_MAX];
	short speechType = 0;
	int len;

	if (!codec_open)
		return 0;

	env->GetByteArrayRegion(encoded, RTP_HDR_SIZE, size, ilbc_data);
	len = ilbc_decode(&Dec_Inst, post_amp, (uint8_t*)ilbc_data, size);

	env->SetShortArrayRegion(lin, 0, len,post_amp);
	return (jint)len;
}
