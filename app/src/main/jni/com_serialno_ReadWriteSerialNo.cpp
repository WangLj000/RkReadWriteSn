#include <jni.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "android/log.h"
#include <cstdint> // for uint8_t
#include <sstream>
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt, ##args)
#define DEBUG_LOG true
static const char *TAG = "serial_no";

typedef unsigned short uint16;
typedef unsigned long  uint32;
typedef unsigned char  uint8;

#define VENDOR_REQ_TAG      0x56524551
#define VENDOR_READ_IO      _IOW('v', 0x01, unsigned int)
#define VENDOR_WRITE_IO     _IOW('v', 0x02, unsigned int)

#define VENDOR_SN_ID        1
#define VENDOR_WIFI_MAC_ID  2
#define VENDOR_LAN_MAC_ID   3
#define VENDOR_BLUETOOTH_ID 4

#define RKNAND_SYS_STORGAE_DATA_LEN 512 /* max read length to read*/
#define SERIALNO_BUF_LEN 33

void hexToAscii(uint8_t *hexData, size_t dataSize, char *asciiBuffer);
int getNum(char arr[]);
struct rk_vendor_req {
    uint32 tag;
    uint16 id;
    uint16 len;
    uint8 data[512];
};
//十进制转十六进制
void hexToAscii(uint8_t *hexData, size_t dataSize, char *asciiBuffer) {

    for (size_t i = 0; i < dataSize; ++i) {
        sprintf(&asciiBuffer[i * 2], "%02X", hexData[i]);
    }
}
//十六进制转十进制
int getNum(char arr[]){
    int m=0;
    int sz=strlen(arr);
    int i=0;
    for(i=0;i<sz;i++)
    {
        if(arr[i]>='0'&&arr[i]<='9'){
            m=m*16+arr[i]-'0';
        }
        else
        {
            m=m*16+arr[i]-'A'+10;
        }
    }
    LOGD("m = %d", m);
    return m;
}

extern "C"
JNIEXPORT jstring JNICALL
Java_com_serialno_ReadWriteSerialNo_read(JNIEnv *env, jclass type,jint id){
    int fd;
    int ret = 0;
    int len = 0;
    char r_buf[RKNAND_SYS_STORGAE_DATA_LEN]={0};
    uint8 error[]="error";
	struct rk_vendor_req req;
    struct rk_vendor_req *p_req = &req;

    p_req->tag = VENDOR_REQ_TAG;
    if(id == 1)
        p_req->id = VENDOR_SN_ID;
    else if(id == 3)  p_req->id = VENDOR_LAN_MAC_ID;
    p_req->len = RKNAND_SYS_STORGAE_DATA_LEN;
    fd = open("/dev/vendor_storage", O_RDWR, 0);
    LOGD("open() fd = %d", fd);

    if (fd == -1) {
        LOGE("Cannot open port");
        ret = -1;
    }else{
        ret = ioctl(fd, VENDOR_READ_IO, p_req);
        close(fd);
    }

    if (!ret) {
       // if(id == 1)
            len =  strlen((char*)p_req->data);
       // else if(id == 3)
            //len =  strlen((char*)p_req->data);
        if(len <= 0){
            len = strlen((char*)error);
            memcpy(r_buf, error, len);
        }else{
        //sprintf(r_buf, "%d", p_req->data[0]);
         if(id == 1)
            memcpy(r_buf, p_req->data,len);
         else if(id == 3)
            hexToAscii(p_req->data,len,r_buf);
            //memcpy(r_buf, p_req->data,len);

            //sprintf(r_buf, "%s", r_buf);
            //LOGD("vendor read sn1 = %c  len = %c, \n",(char)p_req->data[0], (char)p_req->data[1]);
            //LOGD("vendor read sn1 = %s  len = %d, \n",r_buf, len);
        }
    }else{
        len = strlen((char*)error);
        memcpy(r_buf, error, len);
    }
    //convert(r_buf);

    LOGD("vendor read sn2 = %s  len = %d, \n", r_buf, len);

    jclass strClass = (env)->FindClass("java/lang/String");
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = (env)->NewByteArray(len);
    (env)->SetByteArrayRegion(bytes, 0, len, (jbyte*) r_buf);
    jstring encoding = (env)->NewStringUTF("utf-8");
    return (jstring) (env)->NewObject(strClass, ctorID, bytes,encoding);



}

extern "C"
JNIEXPORT jint JNICALL
Java_com_serialno_ReadWriteSerialNo_write(JNIEnv *env, jclass type,jint id,jstring sn){
    int fd;
    int ret = 0;
    int len = 0;
	struct rk_vendor_req req;
    struct rk_vendor_req *p_req = &req;

    jboolean iscopy;
    const char *sn_utf = env->GetStringUTFChars(sn, &iscopy);
    LOGD("write serial no = %s", sn_utf);

    fd = open("/dev/vendor_storage", O_RDWR, 0);
    LOGD("open() fd = %d", fd);

    if (fd == -1) {
        LOGE("Cannot open port");
        ret = -1;
    }

    p_req->tag = VENDOR_REQ_TAG;
    //p_req->id = VENDOR_SN_ID;
    if(id == 1)
       p_req->id = VENDOR_SN_ID;
    else if(id == 3)  p_req->id = VENDOR_LAN_MAC_ID;
    p_req->len = RKNAND_SYS_STORGAE_DATA_LEN;

    len = strlen(sn_utf);
    LOGD("p_req->len = %d", len);

    if(len > SERIALNO_BUF_LEN){
        len = SERIALNO_BUF_LEN;
    }
    if(id == 3){
        int k=0;
        for(int i=0;i<len-1;i+=2){
            char num[3] = {sn_utf[i],sn_utf[i+1],'\0'};
            LOGD("num[2] = %d", strlen(num));
            p_req->data[k++] = getNum(num);
        }
    }else if(id == 1){
        memset(p_req->data, 0, RKNAND_SYS_STORGAE_DATA_LEN);
        memcpy(p_req->data, sn_utf, len);
    }

    ret = ioctl(fd, VENDOR_WRITE_IO, p_req);
    close(fd);

    env->ReleaseStringUTFChars(sn, sn_utf);

    LOGD("vendor write sn = %s, ret = %d\n", p_req->data, ret);

    return ret;
}


