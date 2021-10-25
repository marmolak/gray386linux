/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2011
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

#ifndef CSR_WIFI_MSGCONV_H__
#define CSR_WIFI_MSGCONV_H__

#include "csr_prim_defs.h"
#include "csr_sched.h"

#ifdef __cplusplus
extern "C" {
#endif


void CsrUint16SerBigEndian(u8 *ptr, size_t *len, u16 v);
void CsrUint24SerBigEndian(u8 *ptr, size_t *len, u32 v);
void CsrUint32SerBigEndian(u8 *ptr, size_t *len, u32 v);

void CsrUint16DesBigEndian(u16 *v, u8 *buffer, size_t *offset);
void CsrUint24DesBigEndian(u32 *v, u8 *buffer, size_t *offset);
void CsrUint32DesBigEndian(u32 *v, u8 *buffer, size_t *offset);

void CsrUint24Ser(u8 *ptr, size_t *len, u32 v);
void CsrUint24Des(u32 *v, u8 *buffer, size_t *offset);


size_t CsrWifiEventSizeof(void *msg);
u8* CsrWifiEventSer(u8 *ptr, size_t *len, void *msg);
void* CsrWifiEventDes(u8 *buffer, size_t length);

size_t CsrWifiEventCsrUint8Sizeof(void *msg);
u8* CsrWifiEventCsrUint8Ser(u8 *ptr, size_t *len, void *msg);
void* CsrWifiEventCsrUint8Des(u8 *buffer, size_t length);

size_t CsrWifiEventCsrUint16Sizeof(void *msg);
u8* CsrWifiEventCsrUint16Ser(u8 *ptr, size_t *len, void *msg);
void* CsrWifiEventCsrUint16Des(u8 *buffer, size_t length);

size_t CsrWifiEventCsrUint32Sizeof(void *msg);
u8* CsrWifiEventCsrUint32Ser(u8 *ptr, size_t *len, void *msg);
void* CsrWifiEventCsrUint32Des(u8 *buffer, size_t length);

size_t CsrWifiEventCsrUint16CsrUint8Sizeof(void *msg);
u8* CsrWifiEventCsrUint16CsrUint8Ser(u8 *ptr, size_t *len, void *msg);
void* CsrWifiEventCsrUint16CsrUint8Des(u8 *buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* CSR_WIFI_MSGCONV_H__ */
