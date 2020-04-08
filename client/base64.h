/***********************************************************************
 * @ file base64.h
       __BASE64_H__
 * @ brief base64 enc/dec header file
 * @ history
 * Date        Version  Author     description
 * ==========  =======  =========  ====================================
 * 2020-04-07  V1.0     sleng      Create
 *
 * @ Copyright (C)  2020  Disthen  all right reserved
 ***********************************************************************/
#ifndef __BASE64_H__
#define __BASE64_H__

#ifdef __cplusplus
extern "C"
{
#endif


extern int base64_encode(const unsigned char *src, int srclen, unsigned char *out, int *outlen);
extern int base64_decode(const unsigned char *src, int srclen, unsigned char *out, int *outlen);


#ifdef __cplusplus
extern "C"
};
#endif

#endif //End of __BASE64_H__
