
#ifndef __CC_BASE64_DECODE_H
#define __CC_BASE64_DECODE_H

#ifdef __cplusplus
extern "C" {
#endif


/** @file
 base64 helper functions
 */

/**
 * Decodes a 64base encoded memory. The decoded memory is
 * expected to be freed by the caller.
 *
 * returns the length of the out buffer
 *
 */
int base64Decode(unsigned char *in, unsigned int inLength, unsigned char **out);

#ifdef __cplusplus
}
#endif

#endif // __CC_BASE64_DECODE_H
