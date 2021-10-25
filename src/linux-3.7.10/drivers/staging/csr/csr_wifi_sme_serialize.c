/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/* Note: this is an auto-generated file. */
#include <linux/string.h>
#include <linux/slab.h>
#include "csr_msgconv.h"
#include "csr_wifi_sme_prim.h"
#include "csr_wifi_sme_serialize.h"

void CsrWifiSmePfree(void *ptr)
{
    kfree(ptr);
}


size_t CsrWifiSmeAdhocConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 11) */
    bufferSize += 2; /* u16 primitive->adHocConfig.atimWindowTu */
    bufferSize += 2; /* u16 primitive->adHocConfig.beaconPeriodTu */
    bufferSize += 2; /* u16 primitive->adHocConfig.joinOnlyAttempts */
    bufferSize += 2; /* u16 primitive->adHocConfig.joinAttemptIntervalMs */
    return bufferSize;
}


u8* CsrWifiSmeAdhocConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeAdhocConfigSetReq *primitive = (CsrWifiSmeAdhocConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.atimWindowTu);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.beaconPeriodTu);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.joinOnlyAttempts);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.joinAttemptIntervalMs);
    return(ptr);
}


void* CsrWifiSmeAdhocConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeAdhocConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeAdhocConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.atimWindowTu, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.beaconPeriodTu, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.joinOnlyAttempts, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.joinAttemptIntervalMs, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeBlacklistReqSizeof(void *msg)
{
    CsrWifiSmeBlacklistReq *primitive = (CsrWifiSmeBlacklistReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* u8 primitive->setAddressCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setAddressCount; i1++)
        {
            bufferSize += 6; /* u8 primitive->setAddresses[i1].a[6] */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeBlacklistReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeBlacklistReq *primitive = (CsrWifiSmeBlacklistReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->setAddressCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setAddressCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->setAddresses[i1].a, ((u16) (6)));
        }
    }
    return(ptr);
}


void* CsrWifiSmeBlacklistReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeBlacklistReq *primitive = kmalloc(sizeof(CsrWifiSmeBlacklistReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->setAddressCount, buffer, &offset);
    primitive->setAddresses = NULL;
    if (primitive->setAddressCount)
    {
        primitive->setAddresses = kmalloc(sizeof(CsrWifiMacAddress) * primitive->setAddressCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setAddressCount; i1++)
        {
            CsrMemCpyDes(primitive->setAddresses[i1].a, buffer, &offset, ((u16) (6)));
        }
    }

    return primitive;
}


void CsrWifiSmeBlacklistReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeBlacklistReq *primitive = (CsrWifiSmeBlacklistReq *) voidPrimitivePointer;
    kfree(primitive->setAddresses);
    kfree(primitive);
}


size_t CsrWifiSmeCalibrationDataSetReqSizeof(void *msg)
{
    CsrWifiSmeCalibrationDataSetReq *primitive = (CsrWifiSmeCalibrationDataSetReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 6) */
    bufferSize += 2;                                /* u16 primitive->calibrationDataLength */
    bufferSize += primitive->calibrationDataLength; /* u8 primitive->calibrationData */
    return bufferSize;
}


u8* CsrWifiSmeCalibrationDataSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCalibrationDataSetReq *primitive = (CsrWifiSmeCalibrationDataSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->calibrationDataLength);
    if (primitive->calibrationDataLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->calibrationData, ((u16) (primitive->calibrationDataLength)));
    }
    return(ptr);
}


void* CsrWifiSmeCalibrationDataSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCalibrationDataSetReq *primitive = kmalloc(sizeof(CsrWifiSmeCalibrationDataSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->calibrationDataLength, buffer, &offset);
    if (primitive->calibrationDataLength)
    {
        primitive->calibrationData = kmalloc(primitive->calibrationDataLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->calibrationData, buffer, &offset, ((u16) (primitive->calibrationDataLength)));
    }
    else
    {
        primitive->calibrationData = NULL;
    }

    return primitive;
}


void CsrWifiSmeCalibrationDataSetReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeCalibrationDataSetReq *primitive = (CsrWifiSmeCalibrationDataSetReq *) voidPrimitivePointer;
    kfree(primitive->calibrationData);
    kfree(primitive);
}


size_t CsrWifiSmeCcxConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 9) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* u8 primitive->ccxConfig.keepAliveTimeMs */
    bufferSize += 1; /* u8 primitive->ccxConfig.apRoamingEnabled */
    bufferSize += 1; /* u8 primitive->ccxConfig.measurementsMask */
    bufferSize += 1; /* u8 primitive->ccxConfig.ccxRadioMgtEnabled */
    return bufferSize;
}


u8* CsrWifiSmeCcxConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCcxConfigSetReq *primitive = (CsrWifiSmeCcxConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.keepAliveTimeMs);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.apRoamingEnabled);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.measurementsMask);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.ccxRadioMgtEnabled);
    return(ptr);
}


void* CsrWifiSmeCcxConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCcxConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeCcxConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.keepAliveTimeMs, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.apRoamingEnabled, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.measurementsMask, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.ccxRadioMgtEnabled, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeCoexConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 29) */
    bufferSize += 1; /* u8 primitive->coexConfig.coexEnableSchemeManagement */
    bufferSize += 1; /* u8 primitive->coexConfig.coexPeriodicWakeHost */
    bufferSize += 2; /* u16 primitive->coexConfig.coexTrafficBurstyLatencyMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexTrafficContinuousLatencyMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexObexBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexObexBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpBrBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpBrBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpEdrBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpEdrBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexPagingBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexPagingBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexInquiryBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexInquiryBlackoutPeriodMs */
    return bufferSize;
}


u8* CsrWifiSmeCoexConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCoexConfigSetReq *primitive = (CsrWifiSmeCoexConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint8Ser(ptr, len, (u8) primitive->coexConfig.coexEnableSchemeManagement);
    CsrUint8Ser(ptr, len, (u8) primitive->coexConfig.coexPeriodicWakeHost);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexTrafficBurstyLatencyMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexTrafficContinuousLatencyMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexObexBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexObexBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpBrBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpBrBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpEdrBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpEdrBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexPagingBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexPagingBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexInquiryBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexInquiryBlackoutPeriodMs);
    return(ptr);
}


void* CsrWifiSmeCoexConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCoexConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeCoexConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexConfig.coexEnableSchemeManagement, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexConfig.coexPeriodicWakeHost, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexTrafficBurstyLatencyMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexTrafficContinuousLatencyMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexObexBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexObexBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpBrBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpBrBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpEdrBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpEdrBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexPagingBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexPagingBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexInquiryBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexInquiryBlackoutPeriodMs, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeConnectReqSizeof(void *msg)
{
    CsrWifiSmeConnectReq *primitive = (CsrWifiSmeConnectReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 57) */
    bufferSize += 2;                                                                     /* u16 primitive->interfaceTag */
    bufferSize += 32;                                                                    /* u8 primitive->connectionConfig.ssid.ssid[32] */
    bufferSize += 1;                                                                     /* u8 primitive->connectionConfig.ssid.length */
    bufferSize += 6;                                                                     /* u8 primitive->connectionConfig.bssid.a[6] */
    bufferSize += 1;                                                                     /* CsrWifiSmeBssType primitive->connectionConfig.bssType */
    bufferSize += 1;                                                                     /* CsrWifiSmeRadioIF primitive->connectionConfig.ifIndex */
    bufferSize += 1;                                                                     /* CsrWifiSme80211PrivacyMode primitive->connectionConfig.privacyMode */
    bufferSize += 2;                                                                     /* CsrWifiSmeAuthModeMask primitive->connectionConfig.authModeMask */
    bufferSize += 2;                                                                     /* CsrWifiSmeEncryptionMask primitive->connectionConfig.encryptionModeMask */
    bufferSize += 2;                                                                     /* u16 primitive->connectionConfig.mlmeAssociateReqInformationElementsLength */
    bufferSize += primitive->connectionConfig.mlmeAssociateReqInformationElementsLength; /* u8 primitive->connectionConfig.mlmeAssociateReqInformationElements */
    bufferSize += 1;                                                                     /* CsrWifiSmeWmmQosInfoMask primitive->connectionConfig.wmmQosInfo */
    bufferSize += 1;                                                                     /* u8 primitive->connectionConfig.adhocJoinOnly */
    bufferSize += 1;                                                                     /* u8 primitive->connectionConfig.adhocChannel */
    return bufferSize;
}


u8* CsrWifiSmeConnectReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeConnectReq *primitive = (CsrWifiSmeConnectReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionConfig.ssid.ssid, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.ssid.length);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionConfig.bssid.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.bssType);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.ifIndex);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.privacyMode);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionConfig.authModeMask);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionConfig.encryptionModeMask);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionConfig.mlmeAssociateReqInformationElementsLength);
    if (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionConfig.mlmeAssociateReqInformationElements, ((u16) (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)));
    }
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.wmmQosInfo);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.adhocJoinOnly);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.adhocChannel);
    return(ptr);
}


void* CsrWifiSmeConnectReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeConnectReq *primitive = kmalloc(sizeof(CsrWifiSmeConnectReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrMemCpyDes(primitive->connectionConfig.ssid.ssid, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->connectionConfig.ssid.length, buffer, &offset);
    CsrMemCpyDes(primitive->connectionConfig.bssid.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->connectionConfig.bssType, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.ifIndex, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.privacyMode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionConfig.authModeMask, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionConfig.encryptionModeMask, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionConfig.mlmeAssociateReqInformationElementsLength, buffer, &offset);
    if (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)
    {
        primitive->connectionConfig.mlmeAssociateReqInformationElements = kmalloc(primitive->connectionConfig.mlmeAssociateReqInformationElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionConfig.mlmeAssociateReqInformationElements, buffer, &offset, ((u16) (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)));
    }
    else
    {
        primitive->connectionConfig.mlmeAssociateReqInformationElements = NULL;
    }
    CsrUint8Des((u8 *) &primitive->connectionConfig.wmmQosInfo, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.adhocJoinOnly, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.adhocChannel, buffer, &offset);

    return primitive;
}


void CsrWifiSmeConnectReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeConnectReq *primitive = (CsrWifiSmeConnectReq *) voidPrimitivePointer;
    kfree(primitive->connectionConfig.mlmeAssociateReqInformationElements);
    kfree(primitive);
}


size_t CsrWifiSmeHostConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* CsrWifiSmeHostPowerMode primitive->hostConfig.powerMode */
    bufferSize += 2; /* u16 primitive->hostConfig.applicationDataPeriodMs */
    return bufferSize;
}


u8* CsrWifiSmeHostConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeHostConfigSetReq *primitive = (CsrWifiSmeHostConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->hostConfig.powerMode);
    CsrUint16Ser(ptr, len, (u16) primitive->hostConfig.applicationDataPeriodMs);
    return(ptr);
}


void* CsrWifiSmeHostConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeHostConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeHostConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->hostConfig.powerMode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->hostConfig.applicationDataPeriodMs, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeKeyReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 65) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* CsrWifiSmeKeyType primitive->key.keyType */
    bufferSize += 1; /* u8 primitive->key.keyIndex */
    bufferSize += 1; /* u8 primitive->key.wepTxKey */
    {
        u16 i2;
        for (i2 = 0; i2 < 8; i2++)
        {
            bufferSize += 2; /* u16 primitive->key.keyRsc[8] */
        }
    }
    bufferSize += 1;         /* u8 primitive->key.authenticator */
    bufferSize += 6;         /* u8 primitive->key.address.a[6] */
    bufferSize += 1;         /* u8 primitive->key.keyLength */
    bufferSize += 32;        /* u8 primitive->key.key[32] */
    return bufferSize;
}


u8* CsrWifiSmeKeyReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeKeyReq *primitive = (CsrWifiSmeKeyReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->key.keyType);
    CsrUint8Ser(ptr, len, (u8) primitive->key.keyIndex);
    CsrUint8Ser(ptr, len, (u8) primitive->key.wepTxKey);
    {
        u16 i2;
        for (i2 = 0; i2 < 8; i2++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->key.keyRsc[i2]);
        }
    }
    CsrUint8Ser(ptr, len, (u8) primitive->key.authenticator);
    CsrMemCpySer(ptr, len, (const void *) primitive->key.address.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->key.keyLength);
    CsrMemCpySer(ptr, len, (const void *) primitive->key.key, ((u16) (32)));
    return(ptr);
}


void* CsrWifiSmeKeyReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeKeyReq *primitive = kmalloc(sizeof(CsrWifiSmeKeyReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->key.keyType, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->key.keyIndex, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->key.wepTxKey, buffer, &offset);
    {
        u16 i2;
        for (i2 = 0; i2 < 8; i2++)
        {
            CsrUint16Des((u16 *) &primitive->key.keyRsc[i2], buffer, &offset);
        }
    }
    CsrUint8Des((u8 *) &primitive->key.authenticator, buffer, &offset);
    CsrMemCpyDes(primitive->key.address.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->key.keyLength, buffer, &offset);
    CsrMemCpyDes(primitive->key.key, buffer, &offset, ((u16) (32)));

    return primitive;
}


size_t CsrWifiSmeMibConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 11) */
    bufferSize += 1; /* u8 primitive->mibConfig.unifiFixMaxTxDataRate */
    bufferSize += 1; /* u8 primitive->mibConfig.unifiFixTxDataRate */
    bufferSize += 2; /* u16 primitive->mibConfig.dot11RtsThreshold */
    bufferSize += 2; /* u16 primitive->mibConfig.dot11FragmentationThreshold */
    bufferSize += 2; /* u16 primitive->mibConfig.dot11CurrentTxPowerLevel */
    return bufferSize;
}


u8* CsrWifiSmeMibConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMibConfigSetReq *primitive = (CsrWifiSmeMibConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint8Ser(ptr, len, (u8) primitive->mibConfig.unifiFixMaxTxDataRate);
    CsrUint8Ser(ptr, len, (u8) primitive->mibConfig.unifiFixTxDataRate);
    CsrUint16Ser(ptr, len, (u16) primitive->mibConfig.dot11RtsThreshold);
    CsrUint16Ser(ptr, len, (u16) primitive->mibConfig.dot11FragmentationThreshold);
    CsrUint16Ser(ptr, len, (u16) primitive->mibConfig.dot11CurrentTxPowerLevel);
    return(ptr);
}


void* CsrWifiSmeMibConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMibConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeMibConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->mibConfig.unifiFixMaxTxDataRate, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->mibConfig.unifiFixTxDataRate, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibConfig.dot11RtsThreshold, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibConfig.dot11FragmentationThreshold, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibConfig.dot11CurrentTxPowerLevel, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeMibGetNextReqSizeof(void *msg)
{
    CsrWifiSmeMibGetNextReq *primitive = (CsrWifiSmeMibGetNextReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 6) */
    bufferSize += 2;                             /* u16 primitive->mibAttributeLength */
    bufferSize += primitive->mibAttributeLength; /* u8 primitive->mibAttribute */
    return bufferSize;
}


u8* CsrWifiSmeMibGetNextReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMibGetNextReq *primitive = (CsrWifiSmeMibGetNextReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->mibAttributeLength);
    if (primitive->mibAttributeLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->mibAttribute, ((u16) (primitive->mibAttributeLength)));
    }
    return(ptr);
}


void* CsrWifiSmeMibGetNextReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMibGetNextReq *primitive = kmalloc(sizeof(CsrWifiSmeMibGetNextReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibAttributeLength, buffer, &offset);
    if (primitive->mibAttributeLength)
    {
        primitive->mibAttribute = kmalloc(primitive->mibAttributeLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->mibAttribute, buffer, &offset, ((u16) (primitive->mibAttributeLength)));
    }
    else
    {
        primitive->mibAttribute = NULL;
    }

    return primitive;
}


void CsrWifiSmeMibGetNextReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMibGetNextReq *primitive = (CsrWifiSmeMibGetNextReq *) voidPrimitivePointer;
    kfree(primitive->mibAttribute);
    kfree(primitive);
}


size_t CsrWifiSmeMibGetReqSizeof(void *msg)
{
    CsrWifiSmeMibGetReq *primitive = (CsrWifiSmeMibGetReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 6) */
    bufferSize += 2;                             /* u16 primitive->mibAttributeLength */
    bufferSize += primitive->mibAttributeLength; /* u8 primitive->mibAttribute */
    return bufferSize;
}


u8* CsrWifiSmeMibGetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMibGetReq *primitive = (CsrWifiSmeMibGetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->mibAttributeLength);
    if (primitive->mibAttributeLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->mibAttribute, ((u16) (primitive->mibAttributeLength)));
    }
    return(ptr);
}


void* CsrWifiSmeMibGetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMibGetReq *primitive = kmalloc(sizeof(CsrWifiSmeMibGetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibAttributeLength, buffer, &offset);
    if (primitive->mibAttributeLength)
    {
        primitive->mibAttribute = kmalloc(primitive->mibAttributeLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->mibAttribute, buffer, &offset, ((u16) (primitive->mibAttributeLength)));
    }
    else
    {
        primitive->mibAttribute = NULL;
    }

    return primitive;
}


void CsrWifiSmeMibGetReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMibGetReq *primitive = (CsrWifiSmeMibGetReq *) voidPrimitivePointer;
    kfree(primitive->mibAttribute);
    kfree(primitive);
}


size_t CsrWifiSmeMibSetReqSizeof(void *msg)
{
    CsrWifiSmeMibSetReq *primitive = (CsrWifiSmeMibSetReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 6) */
    bufferSize += 2;                             /* u16 primitive->mibAttributeLength */
    bufferSize += primitive->mibAttributeLength; /* u8 primitive->mibAttribute */
    return bufferSize;
}


u8* CsrWifiSmeMibSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMibSetReq *primitive = (CsrWifiSmeMibSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->mibAttributeLength);
    if (primitive->mibAttributeLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->mibAttribute, ((u16) (primitive->mibAttributeLength)));
    }
    return(ptr);
}


void* CsrWifiSmeMibSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMibSetReq *primitive = kmalloc(sizeof(CsrWifiSmeMibSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibAttributeLength, buffer, &offset);
    if (primitive->mibAttributeLength)
    {
        primitive->mibAttribute = kmalloc(primitive->mibAttributeLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->mibAttribute, buffer, &offset, ((u16) (primitive->mibAttributeLength)));
    }
    else
    {
        primitive->mibAttribute = NULL;
    }

    return primitive;
}


void CsrWifiSmeMibSetReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMibSetReq *primitive = (CsrWifiSmeMibSetReq *) voidPrimitivePointer;
    kfree(primitive->mibAttribute);
    kfree(primitive);
}


size_t CsrWifiSmeMulticastAddressReqSizeof(void *msg)
{
    CsrWifiSmeMulticastAddressReq *primitive = (CsrWifiSmeMulticastAddressReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* u8 primitive->setAddressesCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setAddressesCount; i1++)
        {
            bufferSize += 6; /* u8 primitive->setAddresses[i1].a[6] */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeMulticastAddressReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMulticastAddressReq *primitive = (CsrWifiSmeMulticastAddressReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->setAddressesCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setAddressesCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->setAddresses[i1].a, ((u16) (6)));
        }
    }
    return(ptr);
}


void* CsrWifiSmeMulticastAddressReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMulticastAddressReq *primitive = kmalloc(sizeof(CsrWifiSmeMulticastAddressReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->setAddressesCount, buffer, &offset);
    primitive->setAddresses = NULL;
    if (primitive->setAddressesCount)
    {
        primitive->setAddresses = kmalloc(sizeof(CsrWifiMacAddress) * primitive->setAddressesCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setAddressesCount; i1++)
        {
            CsrMemCpyDes(primitive->setAddresses[i1].a, buffer, &offset, ((u16) (6)));
        }
    }

    return primitive;
}


void CsrWifiSmeMulticastAddressReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMulticastAddressReq *primitive = (CsrWifiSmeMulticastAddressReq *) voidPrimitivePointer;
    kfree(primitive->setAddresses);
    kfree(primitive);
}


size_t CsrWifiSmePacketFilterSetReqSizeof(void *msg)
{
    CsrWifiSmePacketFilterSetReq *primitive = (CsrWifiSmePacketFilterSetReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2;                       /* u16 primitive->interfaceTag */
    bufferSize += 2;                       /* u16 primitive->filterLength */
    bufferSize += primitive->filterLength; /* u8 primitive->filter */
    bufferSize += 1;                       /* CsrWifiSmePacketFilterMode primitive->mode */
    bufferSize += 4;                       /* u8 primitive->arpFilterAddress.a[4] */
    return bufferSize;
}


u8* CsrWifiSmePacketFilterSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePacketFilterSetReq *primitive = (CsrWifiSmePacketFilterSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->filterLength);
    if (primitive->filterLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->filter, ((u16) (primitive->filterLength)));
    }
    CsrUint8Ser(ptr, len, (u8) primitive->mode);
    CsrMemCpySer(ptr, len, (const void *) primitive->arpFilterAddress.a, ((u16) (4)));
    return(ptr);
}


void* CsrWifiSmePacketFilterSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmePacketFilterSetReq *primitive = kmalloc(sizeof(CsrWifiSmePacketFilterSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->filterLength, buffer, &offset);
    if (primitive->filterLength)
    {
        primitive->filter = kmalloc(primitive->filterLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->filter, buffer, &offset, ((u16) (primitive->filterLength)));
    }
    else
    {
        primitive->filter = NULL;
    }
    CsrUint8Des((u8 *) &primitive->mode, buffer, &offset);
    CsrMemCpyDes(primitive->arpFilterAddress.a, buffer, &offset, ((u16) (4)));

    return primitive;
}


void CsrWifiSmePacketFilterSetReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmePacketFilterSetReq *primitive = (CsrWifiSmePacketFilterSetReq *) voidPrimitivePointer;
    kfree(primitive->filter);
    kfree(primitive);
}


size_t CsrWifiSmePmkidReqSizeof(void *msg)
{
    CsrWifiSmePmkidReq *primitive = (CsrWifiSmePmkidReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 29) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* u8 primitive->setPmkidsCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setPmkidsCount; i1++)
        {
            bufferSize += 6;  /* u8 primitive->setPmkids[i1].bssid.a[6] */
            bufferSize += 16; /* u8 primitive->setPmkids[i1].pmkid[16] */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmePmkidReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePmkidReq *primitive = (CsrWifiSmePmkidReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->setPmkidsCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setPmkidsCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->setPmkids[i1].bssid.a, ((u16) (6)));
            CsrMemCpySer(ptr, len, (const void *) primitive->setPmkids[i1].pmkid, ((u16) (16)));
        }
    }
    return(ptr);
}


void* CsrWifiSmePmkidReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmePmkidReq *primitive = kmalloc(sizeof(CsrWifiSmePmkidReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->setPmkidsCount, buffer, &offset);
    primitive->setPmkids = NULL;
    if (primitive->setPmkidsCount)
    {
        primitive->setPmkids = kmalloc(sizeof(CsrWifiSmePmkid) * primitive->setPmkidsCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->setPmkidsCount; i1++)
        {
            CsrMemCpyDes(primitive->setPmkids[i1].bssid.a, buffer, &offset, ((u16) (6)));
            CsrMemCpyDes(primitive->setPmkids[i1].pmkid, buffer, &offset, ((u16) (16)));
        }
    }

    return primitive;
}


void CsrWifiSmePmkidReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmePmkidReq *primitive = (CsrWifiSmePmkidReq *) voidPrimitivePointer;
    kfree(primitive->setPmkids);
    kfree(primitive);
}


size_t CsrWifiSmePowerConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 11) */
    bufferSize += 1; /* CsrWifiSmePowerSaveLevel primitive->powerConfig.powerSaveLevel */
    bufferSize += 2; /* u16 primitive->powerConfig.listenIntervalTu */
    bufferSize += 1; /* u8 primitive->powerConfig.rxDtims */
    bufferSize += 1; /* CsrWifiSmeD3AutoScanMode primitive->powerConfig.d3AutoScanMode */
    bufferSize += 1; /* u8 primitive->powerConfig.clientTrafficWindow */
    bufferSize += 1; /* u8 primitive->powerConfig.opportunisticPowerSave */
    bufferSize += 1; /* u8 primitive->powerConfig.noticeOfAbsence */
    return bufferSize;
}


u8* CsrWifiSmePowerConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePowerConfigSetReq *primitive = (CsrWifiSmePowerConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.powerSaveLevel);
    CsrUint16Ser(ptr, len, (u16) primitive->powerConfig.listenIntervalTu);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.rxDtims);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.d3AutoScanMode);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.clientTrafficWindow);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.opportunisticPowerSave);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.noticeOfAbsence);
    return(ptr);
}


void* CsrWifiSmePowerConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmePowerConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmePowerConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.powerSaveLevel, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->powerConfig.listenIntervalTu, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.rxDtims, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.d3AutoScanMode, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.clientTrafficWindow, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.opportunisticPowerSave, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.noticeOfAbsence, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeRoamingConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 70) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].rssiHighThreshold */
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].rssiLowThreshold */
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].snrHighThreshold */
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].snrLowThreshold */
        }
    }
    bufferSize += 1;         /* u8 primitive->roamingConfig.disableSmoothRoaming */
    bufferSize += 1;         /* u8 primitive->roamingConfig.disableRoamScans */
    bufferSize += 1;         /* u8 primitive->roamingConfig.reconnectLimit */
    bufferSize += 2;         /* u16 primitive->roamingConfig.reconnectLimitIntervalMs */
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].intervalSeconds */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].validitySeconds */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].minActiveChannelTimeTu */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].maxActiveChannelTimeTu */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].minPassiveChannelTimeTu */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].maxPassiveChannelTimeTu */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeRoamingConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeRoamingConfigSetReq *primitive = (CsrWifiSmeRoamingConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].rssiHighThreshold);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].rssiLowThreshold);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].snrHighThreshold);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].snrLowThreshold);
        }
    }
    CsrUint8Ser(ptr, len, (u8) primitive->roamingConfig.disableSmoothRoaming);
    CsrUint8Ser(ptr, len, (u8) primitive->roamingConfig.disableRoamScans);
    CsrUint8Ser(ptr, len, (u8) primitive->roamingConfig.reconnectLimit);
    CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.reconnectLimitIntervalMs);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].intervalSeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].validitySeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].minActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].maxActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].minPassiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].maxPassiveChannelTimeTu);
        }
    }
    return(ptr);
}


void* CsrWifiSmeRoamingConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeRoamingConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeRoamingConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].rssiHighThreshold, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].rssiLowThreshold, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].snrHighThreshold, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].snrLowThreshold, buffer, &offset);
        }
    }
    CsrUint8Des((u8 *) &primitive->roamingConfig.disableSmoothRoaming, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->roamingConfig.disableRoamScans, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->roamingConfig.reconnectLimit, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->roamingConfig.reconnectLimitIntervalMs, buffer, &offset);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].intervalSeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].validitySeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].minActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].maxActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].minPassiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].maxPassiveChannelTimeTu, buffer, &offset);
        }
    }

    return primitive;
}


size_t CsrWifiSmeScanConfigSetReqSizeof(void *msg)
{
    CsrWifiSmeScanConfigSetReq *primitive = (CsrWifiSmeScanConfigSetReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 63) */
    {
        u16 i2;
        for (i2 = 0; i2 < 4; i2++)
        {
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].intervalSeconds */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].validitySeconds */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].minActiveChannelTimeTu */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].maxActiveChannelTimeTu */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].minPassiveChannelTimeTu */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].maxPassiveChannelTimeTu */
        }
    }
    bufferSize += 1;                                             /* u8 primitive->scanConfig.disableAutonomousScans */
    bufferSize += 2;                                             /* u16 primitive->scanConfig.maxResults */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.highRssiThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.lowRssiThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.deltaRssiThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.highSnrThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.lowSnrThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.deltaSnrThreshold */
    bufferSize += 2;                                             /* u16 primitive->scanConfig.passiveChannelListCount */
    bufferSize += primitive->scanConfig.passiveChannelListCount; /* u8 primitive->scanConfig.passiveChannelList */
    return bufferSize;
}


u8* CsrWifiSmeScanConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeScanConfigSetReq *primitive = (CsrWifiSmeScanConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    {
        u16 i2;
        for (i2 = 0; i2 < 4; i2++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].intervalSeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].validitySeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].minActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].maxActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].minPassiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].maxPassiveChannelTimeTu);
        }
    }
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.disableAutonomousScans);
    CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.maxResults);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.highRssiThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.lowRssiThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.deltaRssiThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.highSnrThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.lowSnrThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.deltaSnrThreshold);
    CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.passiveChannelListCount);
    if (primitive->scanConfig.passiveChannelListCount)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->scanConfig.passiveChannelList, ((u16) (primitive->scanConfig.passiveChannelListCount)));
    }
    return(ptr);
}


void* CsrWifiSmeScanConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeScanConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeScanConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    {
        u16 i2;
        for (i2 = 0; i2 < 4; i2++)
        {
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].intervalSeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].validitySeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].minActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].maxActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].minPassiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].maxPassiveChannelTimeTu, buffer, &offset);
        }
    }
    CsrUint8Des((u8 *) &primitive->scanConfig.disableAutonomousScans, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->scanConfig.maxResults, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.highRssiThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.lowRssiThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.deltaRssiThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.highSnrThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.lowSnrThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.deltaSnrThreshold, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->scanConfig.passiveChannelListCount, buffer, &offset);
    if (primitive->scanConfig.passiveChannelListCount)
    {
        primitive->scanConfig.passiveChannelList = kmalloc(primitive->scanConfig.passiveChannelListCount, GFP_KERNEL);
        CsrMemCpyDes(primitive->scanConfig.passiveChannelList, buffer, &offset, ((u16) (primitive->scanConfig.passiveChannelListCount)));
    }
    else
    {
        primitive->scanConfig.passiveChannelList = NULL;
    }

    return primitive;
}


void CsrWifiSmeScanConfigSetReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeScanConfigSetReq *primitive = (CsrWifiSmeScanConfigSetReq *) voidPrimitivePointer;
    kfree(primitive->scanConfig.passiveChannelList);
    kfree(primitive);
}


size_t CsrWifiSmeScanFullReqSizeof(void *msg)
{
    CsrWifiSmeScanFullReq *primitive = (CsrWifiSmeScanFullReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 52) */
    bufferSize += 1; /* u8 primitive->ssidCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->ssidCount; i1++)
        {
            bufferSize += 32;                  /* u8 primitive->ssid[i1].ssid[32] */
            bufferSize += 1;                   /* u8 primitive->ssid[i1].length */
        }
    }
    bufferSize += 6;                           /* u8 primitive->bssid.a[6] */
    bufferSize += 1;                           /* u8 primitive->forceScan */
    bufferSize += 1;                           /* CsrWifiSmeBssType primitive->bssType */
    bufferSize += 1;                           /* CsrWifiSmeScanType primitive->scanType */
    bufferSize += 2;                           /* u16 primitive->channelListCount */
    bufferSize += primitive->channelListCount; /* u8 primitive->channelList */
    bufferSize += 2;                           /* u16 primitive->probeIeLength */
    bufferSize += primitive->probeIeLength;    /* u8 primitive->probeIe */
    return bufferSize;
}


u8* CsrWifiSmeScanFullReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeScanFullReq *primitive = (CsrWifiSmeScanFullReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint8Ser(ptr, len, (u8) primitive->ssidCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->ssidCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->ssid[i1].ssid, ((u16) (32)));
            CsrUint8Ser(ptr, len, (u8) primitive->ssid[i1].length);
        }
    }
    CsrMemCpySer(ptr, len, (const void *) primitive->bssid.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->forceScan);
    CsrUint8Ser(ptr, len, (u8) primitive->bssType);
    CsrUint8Ser(ptr, len, (u8) primitive->scanType);
    CsrUint16Ser(ptr, len, (u16) primitive->channelListCount);
    if (primitive->channelListCount)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->channelList, ((u16) (primitive->channelListCount)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->probeIeLength);
    if (primitive->probeIeLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->probeIe, ((u16) (primitive->probeIeLength)));
    }
    return(ptr);
}


void* CsrWifiSmeScanFullReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeScanFullReq *primitive = kmalloc(sizeof(CsrWifiSmeScanFullReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ssidCount, buffer, &offset);
    primitive->ssid = NULL;
    if (primitive->ssidCount)
    {
        primitive->ssid = kmalloc(sizeof(CsrWifiSsid) * primitive->ssidCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->ssidCount; i1++)
        {
            CsrMemCpyDes(primitive->ssid[i1].ssid, buffer, &offset, ((u16) (32)));
            CsrUint8Des((u8 *) &primitive->ssid[i1].length, buffer, &offset);
        }
    }
    CsrMemCpyDes(primitive->bssid.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->forceScan, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->bssType, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanType, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->channelListCount, buffer, &offset);
    if (primitive->channelListCount)
    {
        primitive->channelList = kmalloc(primitive->channelListCount, GFP_KERNEL);
        CsrMemCpyDes(primitive->channelList, buffer, &offset, ((u16) (primitive->channelListCount)));
    }
    else
    {
        primitive->channelList = NULL;
    }
    CsrUint16Des((u16 *) &primitive->probeIeLength, buffer, &offset);
    if (primitive->probeIeLength)
    {
        primitive->probeIe = kmalloc(primitive->probeIeLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->probeIe, buffer, &offset, ((u16) (primitive->probeIeLength)));
    }
    else
    {
        primitive->probeIe = NULL;
    }

    return primitive;
}


void CsrWifiSmeScanFullReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeScanFullReq *primitive = (CsrWifiSmeScanFullReq *) voidPrimitivePointer;
    kfree(primitive->ssid);
    kfree(primitive->channelList);
    kfree(primitive->probeIe);
    kfree(primitive);
}


size_t CsrWifiSmeSmeStaConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 11) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* u8 primitive->smeConfig.connectionQualityRssiChangeTrigger */
    bufferSize += 1; /* u8 primitive->smeConfig.connectionQualitySnrChangeTrigger */
    bufferSize += 1; /* CsrWifiSmeWmmModeMask primitive->smeConfig.wmmModeMask */
    bufferSize += 1; /* CsrWifiSmeRadioIF primitive->smeConfig.ifIndex */
    bufferSize += 1; /* u8 primitive->smeConfig.allowUnicastUseGroupCipher */
    bufferSize += 1; /* u8 primitive->smeConfig.enableOpportunisticKeyCaching */
    return bufferSize;
}


u8* CsrWifiSmeSmeStaConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeSmeStaConfigSetReq *primitive = (CsrWifiSmeSmeStaConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.connectionQualityRssiChangeTrigger);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.connectionQualitySnrChangeTrigger);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.wmmModeMask);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.ifIndex);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.allowUnicastUseGroupCipher);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.enableOpportunisticKeyCaching);
    return(ptr);
}


void* CsrWifiSmeSmeStaConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeSmeStaConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeSmeStaConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.connectionQualityRssiChangeTrigger, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.connectionQualitySnrChangeTrigger, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.wmmModeMask, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.ifIndex, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.allowUnicastUseGroupCipher, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.enableOpportunisticKeyCaching, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeTspecReqSizeof(void *msg)
{
    CsrWifiSmeTspecReq *primitive = (CsrWifiSmeTspecReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 18) */
    bufferSize += 2;                      /* u16 primitive->interfaceTag */
    bufferSize += 1;                      /* CsrWifiSmeListAction primitive->action */
    bufferSize += 4;                      /* u32 primitive->transactionId */
    bufferSize += 1;                      /* u8 primitive->strict */
    bufferSize += 1;                      /* CsrWifiSmeTspecCtrlMask primitive->ctrlMask */
    bufferSize += 2;                      /* u16 primitive->tspecLength */
    bufferSize += primitive->tspecLength; /* u8 primitive->tspec */
    bufferSize += 2;                      /* u16 primitive->tclasLength */
    bufferSize += primitive->tclasLength; /* u8 primitive->tclas */
    return bufferSize;
}


u8* CsrWifiSmeTspecReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeTspecReq *primitive = (CsrWifiSmeTspecReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint32Ser(ptr, len, (u32) primitive->transactionId);
    CsrUint8Ser(ptr, len, (u8) primitive->strict);
    CsrUint8Ser(ptr, len, (u8) primitive->ctrlMask);
    CsrUint16Ser(ptr, len, (u16) primitive->tspecLength);
    if (primitive->tspecLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->tspec, ((u16) (primitive->tspecLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->tclasLength);
    if (primitive->tclasLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->tclas, ((u16) (primitive->tclasLength)));
    }
    return(ptr);
}


void* CsrWifiSmeTspecReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeTspecReq *primitive = kmalloc(sizeof(CsrWifiSmeTspecReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->transactionId, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->strict, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ctrlMask, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->tspecLength, buffer, &offset);
    if (primitive->tspecLength)
    {
        primitive->tspec = kmalloc(primitive->tspecLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->tspec, buffer, &offset, ((u16) (primitive->tspecLength)));
    }
    else
    {
        primitive->tspec = NULL;
    }
    CsrUint16Des((u16 *) &primitive->tclasLength, buffer, &offset);
    if (primitive->tclasLength)
    {
        primitive->tclas = kmalloc(primitive->tclasLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->tclas, buffer, &offset, ((u16) (primitive->tclasLength)));
    }
    else
    {
        primitive->tclas = NULL;
    }

    return primitive;
}


void CsrWifiSmeTspecReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeTspecReq *primitive = (CsrWifiSmeTspecReq *) voidPrimitivePointer;
    kfree(primitive->tspec);
    kfree(primitive->tclas);
    kfree(primitive);
}


size_t CsrWifiSmeWifiFlightmodeReqSizeof(void *msg)
{
    CsrWifiSmeWifiFlightmodeReq *primitive = (CsrWifiSmeWifiFlightmodeReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 14) */
    bufferSize += 6; /* u8 primitive->address.a[6] */
    bufferSize += 2; /* u16 primitive->mibFilesCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            bufferSize += 2;                              /* u16 primitive->mibFiles[i1].length */
            bufferSize += primitive->mibFiles[i1].length; /* u8 primitive->mibFiles[i1].data */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeWifiFlightmodeReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeWifiFlightmodeReq *primitive = (CsrWifiSmeWifiFlightmodeReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrMemCpySer(ptr, len, (const void *) primitive->address.a, ((u16) (6)));
    CsrUint16Ser(ptr, len, (u16) primitive->mibFilesCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->mibFiles[i1].length);
            if (primitive->mibFiles[i1].length)
            {
                CsrMemCpySer(ptr, len, (const void *) primitive->mibFiles[i1].data, ((u16) (primitive->mibFiles[i1].length)));
            }
        }
    }
    return(ptr);
}


void* CsrWifiSmeWifiFlightmodeReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeWifiFlightmodeReq *primitive = kmalloc(sizeof(CsrWifiSmeWifiFlightmodeReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrMemCpyDes(primitive->address.a, buffer, &offset, ((u16) (6)));
    CsrUint16Des((u16 *) &primitive->mibFilesCount, buffer, &offset);
    primitive->mibFiles = NULL;
    if (primitive->mibFilesCount)
    {
        primitive->mibFiles = kmalloc(sizeof(CsrWifiSmeDataBlock) * primitive->mibFilesCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            CsrUint16Des((u16 *) &primitive->mibFiles[i1].length, buffer, &offset);
            if (primitive->mibFiles[i1].length)
            {
                primitive->mibFiles[i1].data = kmalloc(primitive->mibFiles[i1].length, GFP_KERNEL);
                CsrMemCpyDes(primitive->mibFiles[i1].data, buffer, &offset, ((u16) (primitive->mibFiles[i1].length)));
            }
            else
            {
                primitive->mibFiles[i1].data = NULL;
            }
        }
    }

    return primitive;
}


void CsrWifiSmeWifiFlightmodeReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeWifiFlightmodeReq *primitive = (CsrWifiSmeWifiFlightmodeReq *) voidPrimitivePointer;
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            kfree(primitive->mibFiles[i1].data);
        }
    }
    kfree(primitive->mibFiles);
    kfree(primitive);
}


size_t CsrWifiSmeWifiOnReqSizeof(void *msg)
{
    CsrWifiSmeWifiOnReq *primitive = (CsrWifiSmeWifiOnReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 14) */
    bufferSize += 6; /* u8 primitive->address.a[6] */
    bufferSize += 2; /* u16 primitive->mibFilesCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            bufferSize += 2;                              /* u16 primitive->mibFiles[i1].length */
            bufferSize += primitive->mibFiles[i1].length; /* u8 primitive->mibFiles[i1].data */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeWifiOnReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeWifiOnReq *primitive = (CsrWifiSmeWifiOnReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrMemCpySer(ptr, len, (const void *) primitive->address.a, ((u16) (6)));
    CsrUint16Ser(ptr, len, (u16) primitive->mibFilesCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->mibFiles[i1].length);
            if (primitive->mibFiles[i1].length)
            {
                CsrMemCpySer(ptr, len, (const void *) primitive->mibFiles[i1].data, ((u16) (primitive->mibFiles[i1].length)));
            }
        }
    }
    return(ptr);
}


void* CsrWifiSmeWifiOnReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeWifiOnReq *primitive = kmalloc(sizeof(CsrWifiSmeWifiOnReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrMemCpyDes(primitive->address.a, buffer, &offset, ((u16) (6)));
    CsrUint16Des((u16 *) &primitive->mibFilesCount, buffer, &offset);
    primitive->mibFiles = NULL;
    if (primitive->mibFilesCount)
    {
        primitive->mibFiles = kmalloc(sizeof(CsrWifiSmeDataBlock) * primitive->mibFilesCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            CsrUint16Des((u16 *) &primitive->mibFiles[i1].length, buffer, &offset);
            if (primitive->mibFiles[i1].length)
            {
                primitive->mibFiles[i1].data = kmalloc(primitive->mibFiles[i1].length, GFP_KERNEL);
                CsrMemCpyDes(primitive->mibFiles[i1].data, buffer, &offset, ((u16) (primitive->mibFiles[i1].length)));
            }
            else
            {
                primitive->mibFiles[i1].data = NULL;
            }
        }
    }

    return primitive;
}


void CsrWifiSmeWifiOnReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeWifiOnReq *primitive = (CsrWifiSmeWifiOnReq *) voidPrimitivePointer;
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->mibFilesCount; i1++)
        {
            kfree(primitive->mibFiles[i1].data);
        }
    }
    kfree(primitive->mibFiles);
    kfree(primitive);
}


size_t CsrWifiSmeCloakedSsidsSetReqSizeof(void *msg)
{
    CsrWifiSmeCloakedSsidsSetReq *primitive = (CsrWifiSmeCloakedSsidsSetReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 37) */
    bufferSize += 1; /* u8 primitive->cloakedSsids.cloakedSsidsCount */
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->cloakedSsids.cloakedSsidsCount; i2++)
        {
            bufferSize += 32; /* u8 primitive->cloakedSsids.cloakedSsids[i2].ssid[32] */
            bufferSize += 1;  /* u8 primitive->cloakedSsids.cloakedSsids[i2].length */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeCloakedSsidsSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCloakedSsidsSetReq *primitive = (CsrWifiSmeCloakedSsidsSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint8Ser(ptr, len, (u8) primitive->cloakedSsids.cloakedSsidsCount);
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->cloakedSsids.cloakedSsidsCount; i2++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->cloakedSsids.cloakedSsids[i2].ssid, ((u16) (32)));
            CsrUint8Ser(ptr, len, (u8) primitive->cloakedSsids.cloakedSsids[i2].length);
        }
    }
    return(ptr);
}


void* CsrWifiSmeCloakedSsidsSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCloakedSsidsSetReq *primitive = kmalloc(sizeof(CsrWifiSmeCloakedSsidsSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->cloakedSsids.cloakedSsidsCount, buffer, &offset);
    primitive->cloakedSsids.cloakedSsids = NULL;
    if (primitive->cloakedSsids.cloakedSsidsCount)
    {
        primitive->cloakedSsids.cloakedSsids = kmalloc(sizeof(CsrWifiSsid) * primitive->cloakedSsids.cloakedSsidsCount, GFP_KERNEL);
    }
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->cloakedSsids.cloakedSsidsCount; i2++)
        {
            CsrMemCpyDes(primitive->cloakedSsids.cloakedSsids[i2].ssid, buffer, &offset, ((u16) (32)));
            CsrUint8Des((u8 *) &primitive->cloakedSsids.cloakedSsids[i2].length, buffer, &offset);
        }
    }

    return primitive;
}


void CsrWifiSmeCloakedSsidsSetReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeCloakedSsidsSetReq *primitive = (CsrWifiSmeCloakedSsidsSetReq *) voidPrimitivePointer;
    kfree(primitive->cloakedSsids.cloakedSsids);
    kfree(primitive);
}


size_t CsrWifiSmeSmeCommonConfigSetReqSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 1; /* CsrWifiSme80211dTrustLevel primitive->deviceConfig.trustLevel */
    bufferSize += 2; /* u8 primitive->deviceConfig.countryCode[2] */
    bufferSize += 1; /* CsrWifiSmeFirmwareDriverInterface primitive->deviceConfig.firmwareDriverInterface */
    bufferSize += 1; /* u8 primitive->deviceConfig.enableStrictDraftN */
    return bufferSize;
}


u8* CsrWifiSmeSmeCommonConfigSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeSmeCommonConfigSetReq *primitive = (CsrWifiSmeSmeCommonConfigSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint8Ser(ptr, len, (u8) primitive->deviceConfig.trustLevel);
    CsrMemCpySer(ptr, len, (const void *) primitive->deviceConfig.countryCode, ((u16) (2)));
    CsrUint8Ser(ptr, len, (u8) primitive->deviceConfig.firmwareDriverInterface);
    CsrUint8Ser(ptr, len, (u8) primitive->deviceConfig.enableStrictDraftN);
    return(ptr);
}


void* CsrWifiSmeSmeCommonConfigSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeSmeCommonConfigSetReq *primitive = kmalloc(sizeof(CsrWifiSmeSmeCommonConfigSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->deviceConfig.trustLevel, buffer, &offset);
    CsrMemCpyDes(primitive->deviceConfig.countryCode, buffer, &offset, ((u16) (2)));
    CsrUint8Des((u8 *) &primitive->deviceConfig.firmwareDriverInterface, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->deviceConfig.enableStrictDraftN, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeWpsConfigurationReqSizeof(void *msg)
{
    CsrWifiSmeWpsConfigurationReq *primitive = (CsrWifiSmeWpsConfigurationReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 240) */
    bufferSize += 1;  /* u8 primitive->wpsConfig.wpsVersion */
    bufferSize += 16; /* u8 primitive->wpsConfig.uuid[16] */
    bufferSize += 32; /* u8 primitive->wpsConfig.deviceName[32] */
    bufferSize += 1;  /* u8 primitive->wpsConfig.deviceNameLength */
    bufferSize += 64; /* u8 primitive->wpsConfig.manufacturer[64] */
    bufferSize += 1;  /* u8 primitive->wpsConfig.manufacturerLength */
    bufferSize += 32; /* u8 primitive->wpsConfig.modelName[32] */
    bufferSize += 1;  /* u8 primitive->wpsConfig.modelNameLength */
    bufferSize += 32; /* u8 primitive->wpsConfig.modelNumber[32] */
    bufferSize += 1;  /* u8 primitive->wpsConfig.modelNumberLength */
    bufferSize += 32; /* u8 primitive->wpsConfig.serialNumber[32] */
    bufferSize += 8;  /* u8 primitive->wpsConfig.primDeviceType.deviceDetails[8] */
    bufferSize += 1;  /* u8 primitive->wpsConfig.secondaryDeviceTypeCount */
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->wpsConfig.secondaryDeviceTypeCount; i2++)
        {
            bufferSize += 8; /* u8 primitive->wpsConfig.secondaryDeviceType[i2].deviceDetails[8] */
        }
    }
    bufferSize += 2;         /* CsrWifiSmeWpsConfigTypeMask primitive->wpsConfig.configMethods */
    bufferSize += 1;         /* u8 primitive->wpsConfig.rfBands */
    bufferSize += 4;         /* u8 primitive->wpsConfig.osVersion[4] */
    return bufferSize;
}


u8* CsrWifiSmeWpsConfigurationReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeWpsConfigurationReq *primitive = (CsrWifiSmeWpsConfigurationReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint8Ser(ptr, len, (u8) primitive->wpsConfig.wpsVersion);
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.uuid, ((u16) (16)));
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.deviceName, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->wpsConfig.deviceNameLength);
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.manufacturer, ((u16) (64)));
    CsrUint8Ser(ptr, len, (u8) primitive->wpsConfig.manufacturerLength);
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.modelName, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->wpsConfig.modelNameLength);
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.modelNumber, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->wpsConfig.modelNumberLength);
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.serialNumber, ((u16) (32)));
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.primDeviceType.deviceDetails, ((u16) (8)));
    CsrUint8Ser(ptr, len, (u8) primitive->wpsConfig.secondaryDeviceTypeCount);
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->wpsConfig.secondaryDeviceTypeCount; i2++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.secondaryDeviceType[i2].deviceDetails, ((u16) (8)));
        }
    }
    CsrUint16Ser(ptr, len, (u16) primitive->wpsConfig.configMethods);
    CsrUint8Ser(ptr, len, (u8) primitive->wpsConfig.rfBands);
    CsrMemCpySer(ptr, len, (const void *) primitive->wpsConfig.osVersion, ((u16) (4)));
    return(ptr);
}


void* CsrWifiSmeWpsConfigurationReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeWpsConfigurationReq *primitive = kmalloc(sizeof(CsrWifiSmeWpsConfigurationReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->wpsConfig.wpsVersion, buffer, &offset);
    CsrMemCpyDes(primitive->wpsConfig.uuid, buffer, &offset, ((u16) (16)));
    CsrMemCpyDes(primitive->wpsConfig.deviceName, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->wpsConfig.deviceNameLength, buffer, &offset);
    CsrMemCpyDes(primitive->wpsConfig.manufacturer, buffer, &offset, ((u16) (64)));
    CsrUint8Des((u8 *) &primitive->wpsConfig.manufacturerLength, buffer, &offset);
    CsrMemCpyDes(primitive->wpsConfig.modelName, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->wpsConfig.modelNameLength, buffer, &offset);
    CsrMemCpyDes(primitive->wpsConfig.modelNumber, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->wpsConfig.modelNumberLength, buffer, &offset);
    CsrMemCpyDes(primitive->wpsConfig.serialNumber, buffer, &offset, ((u16) (32)));
    CsrMemCpyDes(primitive->wpsConfig.primDeviceType.deviceDetails, buffer, &offset, ((u16) (8)));
    CsrUint8Des((u8 *) &primitive->wpsConfig.secondaryDeviceTypeCount, buffer, &offset);
    primitive->wpsConfig.secondaryDeviceType = NULL;
    if (primitive->wpsConfig.secondaryDeviceTypeCount)
    {
        primitive->wpsConfig.secondaryDeviceType = kmalloc(sizeof(CsrWifiSmeWpsDeviceType) * primitive->wpsConfig.secondaryDeviceTypeCount, GFP_KERNEL);
    }
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->wpsConfig.secondaryDeviceTypeCount; i2++)
        {
            CsrMemCpyDes(primitive->wpsConfig.secondaryDeviceType[i2].deviceDetails, buffer, &offset, ((u16) (8)));
        }
    }
    CsrUint16Des((u16 *) &primitive->wpsConfig.configMethods, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->wpsConfig.rfBands, buffer, &offset);
    CsrMemCpyDes(primitive->wpsConfig.osVersion, buffer, &offset, ((u16) (4)));

    return primitive;
}


void CsrWifiSmeWpsConfigurationReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeWpsConfigurationReq *primitive = (CsrWifiSmeWpsConfigurationReq *) voidPrimitivePointer;
    kfree(primitive->wpsConfig.secondaryDeviceType);
    kfree(primitive);
}


size_t CsrWifiSmeSetReqSizeof(void *msg)
{
    CsrWifiSmeSetReq *primitive = (CsrWifiSmeSetReq *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 4;                     /* u32 primitive->dataLength */
    bufferSize += primitive->dataLength; /* u8 primitive->data */
    return bufferSize;
}


u8* CsrWifiSmeSetReqSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeSetReq *primitive = (CsrWifiSmeSetReq *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint32Ser(ptr, len, (u32) primitive->dataLength);
    if (primitive->dataLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->data, ((u16) (primitive->dataLength)));
    }
    return(ptr);
}


void* CsrWifiSmeSetReqDes(u8 *buffer, size_t length)
{
    CsrWifiSmeSetReq *primitive = kmalloc(sizeof(CsrWifiSmeSetReq), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->dataLength, buffer, &offset);
    if (primitive->dataLength)
    {
        primitive->data = kmalloc(primitive->dataLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->data, buffer, &offset, ((u16) (primitive->dataLength)));
    }
    else
    {
        primitive->data = NULL;
    }

    return primitive;
}


void CsrWifiSmeSetReqSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeSetReq *primitive = (CsrWifiSmeSetReq *) voidPrimitivePointer;
    kfree(primitive->data);
    kfree(primitive);
}


size_t CsrWifiSmeAdhocConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 2; /* u16 primitive->adHocConfig.atimWindowTu */
    bufferSize += 2; /* u16 primitive->adHocConfig.beaconPeriodTu */
    bufferSize += 2; /* u16 primitive->adHocConfig.joinOnlyAttempts */
    bufferSize += 2; /* u16 primitive->adHocConfig.joinAttemptIntervalMs */
    return bufferSize;
}


u8* CsrWifiSmeAdhocConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeAdhocConfigGetCfm *primitive = (CsrWifiSmeAdhocConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.atimWindowTu);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.beaconPeriodTu);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.joinOnlyAttempts);
    CsrUint16Ser(ptr, len, (u16) primitive->adHocConfig.joinAttemptIntervalMs);
    return(ptr);
}


void* CsrWifiSmeAdhocConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeAdhocConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeAdhocConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.atimWindowTu, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.beaconPeriodTu, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.joinOnlyAttempts, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->adHocConfig.joinAttemptIntervalMs, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeAssociationCompleteIndSizeof(void *msg)
{
    CsrWifiSmeAssociationCompleteInd *primitive = (CsrWifiSmeAssociationCompleteInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 98) */
    bufferSize += 2;                                                     /* u16 primitive->interfaceTag */
    bufferSize += 2;                                                     /* CsrResult primitive->status */
    bufferSize += 32;                                                    /* u8 primitive->connectionInfo.ssid.ssid[32] */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.ssid.length */
    bufferSize += 6;                                                     /* u8 primitive->connectionInfo.bssid.a[6] */
    bufferSize += 1;                                                     /* CsrWifiSme80211NetworkType primitive->connectionInfo.networkType80211 */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.channelNumber */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.channelFrequency */
    bufferSize += 2;                                                     /* CsrWifiSmeAuthMode primitive->connectionInfo.authMode */
    bufferSize += 2;                                                     /* CsrWifiSmeEncryption primitive->connectionInfo.pairwiseCipher */
    bufferSize += 2;                                                     /* CsrWifiSmeEncryption primitive->connectionInfo.groupCipher */
    bufferSize += 1;                                                     /* CsrWifiSmeRadioIF primitive->connectionInfo.ifIndex */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.atimWindowTu */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.beaconPeriodTu */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.reassociation */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.beaconFrameLength */
    bufferSize += primitive->connectionInfo.beaconFrameLength;           /* u8 primitive->connectionInfo.beaconFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.associationReqFrameLength */
    bufferSize += primitive->connectionInfo.associationReqFrameLength;   /* u8 primitive->connectionInfo.associationReqFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.associationRspFrameLength */
    bufferSize += primitive->connectionInfo.associationRspFrameLength;   /* u8 primitive->connectionInfo.associationRspFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocScanInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocScanInfoElementsLength; /* u8 primitive->connectionInfo.assocScanInfoElements */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqCapabilities */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqListenIntervalTu */
    bufferSize += 6;                                                     /* u8 primitive->connectionInfo.assocReqApAddress.a[6] */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocReqInfoElementsLength;  /* u8 primitive->connectionInfo.assocReqInfoElements */
    bufferSize += 2;                                                     /* CsrWifiSmeIEEE80211Result primitive->connectionInfo.assocRspResult */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspCapabilityInfo */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspAssociationId */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocRspInfoElementsLength;  /* u8 primitive->connectionInfo.assocRspInfoElements */
    bufferSize += 2;                                                     /* CsrWifiSmeIEEE80211Reason primitive->deauthReason */
    return bufferSize;
}


u8* CsrWifiSmeAssociationCompleteIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeAssociationCompleteInd *primitive = (CsrWifiSmeAssociationCompleteInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.ssid.ssid, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.ssid.length);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.bssid.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.networkType80211);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.channelNumber);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.channelFrequency);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.authMode);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.pairwiseCipher);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.groupCipher);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.ifIndex);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.atimWindowTu);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.beaconPeriodTu);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.reassociation);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.beaconFrameLength);
    if (primitive->connectionInfo.beaconFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.beaconFrame, ((u16) (primitive->connectionInfo.beaconFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.associationReqFrameLength);
    if (primitive->connectionInfo.associationReqFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.associationReqFrame, ((u16) (primitive->connectionInfo.associationReqFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.associationRspFrameLength);
    if (primitive->connectionInfo.associationRspFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.associationRspFrame, ((u16) (primitive->connectionInfo.associationRspFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocScanInfoElementsLength);
    if (primitive->connectionInfo.assocScanInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocScanInfoElements, ((u16) (primitive->connectionInfo.assocScanInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqCapabilities);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqListenIntervalTu);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocReqApAddress.a, ((u16) (6)));
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqInfoElementsLength);
    if (primitive->connectionInfo.assocReqInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocReqInfoElements, ((u16) (primitive->connectionInfo.assocReqInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspResult);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspCapabilityInfo);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspAssociationId);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspInfoElementsLength);
    if (primitive->connectionInfo.assocRspInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocRspInfoElements, ((u16) (primitive->connectionInfo.assocRspInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->deauthReason);
    return(ptr);
}


void* CsrWifiSmeAssociationCompleteIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeAssociationCompleteInd *primitive = kmalloc(sizeof(CsrWifiSmeAssociationCompleteInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.ssid.ssid, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->connectionInfo.ssid.length, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.bssid.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->connectionInfo.networkType80211, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.channelNumber, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.channelFrequency, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.authMode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.pairwiseCipher, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.groupCipher, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.ifIndex, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.atimWindowTu, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.beaconPeriodTu, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.reassociation, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.beaconFrameLength, buffer, &offset);
    if (primitive->connectionInfo.beaconFrameLength)
    {
        primitive->connectionInfo.beaconFrame = kmalloc(primitive->connectionInfo.beaconFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.beaconFrame, buffer, &offset, ((u16) (primitive->connectionInfo.beaconFrameLength)));
    }
    else
    {
        primitive->connectionInfo.beaconFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.associationReqFrameLength, buffer, &offset);
    if (primitive->connectionInfo.associationReqFrameLength)
    {
        primitive->connectionInfo.associationReqFrame = kmalloc(primitive->connectionInfo.associationReqFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.associationReqFrame, buffer, &offset, ((u16) (primitive->connectionInfo.associationReqFrameLength)));
    }
    else
    {
        primitive->connectionInfo.associationReqFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.associationRspFrameLength, buffer, &offset);
    if (primitive->connectionInfo.associationRspFrameLength)
    {
        primitive->connectionInfo.associationRspFrame = kmalloc(primitive->connectionInfo.associationRspFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.associationRspFrame, buffer, &offset, ((u16) (primitive->connectionInfo.associationRspFrameLength)));
    }
    else
    {
        primitive->connectionInfo.associationRspFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocScanInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocScanInfoElementsLength)
    {
        primitive->connectionInfo.assocScanInfoElements = kmalloc(primitive->connectionInfo.assocScanInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocScanInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocScanInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocScanInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqCapabilities, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqListenIntervalTu, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.assocReqApAddress.a, buffer, &offset, ((u16) (6)));
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocReqInfoElementsLength)
    {
        primitive->connectionInfo.assocReqInfoElements = kmalloc(primitive->connectionInfo.assocReqInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocReqInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocReqInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocReqInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspResult, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspCapabilityInfo, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspAssociationId, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocRspInfoElementsLength)
    {
        primitive->connectionInfo.assocRspInfoElements = kmalloc(primitive->connectionInfo.assocRspInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocRspInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocRspInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocRspInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->deauthReason, buffer, &offset);

    return primitive;
}


void CsrWifiSmeAssociationCompleteIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeAssociationCompleteInd *primitive = (CsrWifiSmeAssociationCompleteInd *) voidPrimitivePointer;
    kfree(primitive->connectionInfo.beaconFrame);
    kfree(primitive->connectionInfo.associationReqFrame);
    kfree(primitive->connectionInfo.associationRspFrame);
    kfree(primitive->connectionInfo.assocScanInfoElements);
    kfree(primitive->connectionInfo.assocReqInfoElements);
    kfree(primitive->connectionInfo.assocRspInfoElements);
    kfree(primitive);
}


size_t CsrWifiSmeAssociationStartIndSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 44) */
    bufferSize += 2;  /* u16 primitive->interfaceTag */
    bufferSize += 6;  /* u8 primitive->address.a[6] */
    bufferSize += 32; /* u8 primitive->ssid.ssid[32] */
    bufferSize += 1;  /* u8 primitive->ssid.length */
    return bufferSize;
}


u8* CsrWifiSmeAssociationStartIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeAssociationStartInd *primitive = (CsrWifiSmeAssociationStartInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrMemCpySer(ptr, len, (const void *) primitive->address.a, ((u16) (6)));
    CsrMemCpySer(ptr, len, (const void *) primitive->ssid.ssid, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->ssid.length);
    return(ptr);
}


void* CsrWifiSmeAssociationStartIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeAssociationStartInd *primitive = kmalloc(sizeof(CsrWifiSmeAssociationStartInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrMemCpyDes(primitive->address.a, buffer, &offset, ((u16) (6)));
    CsrMemCpyDes(primitive->ssid.ssid, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->ssid.length, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeBlacklistCfmSizeof(void *msg)
{
    CsrWifiSmeBlacklistCfm *primitive = (CsrWifiSmeBlacklistCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 15) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* u8 primitive->getAddressCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getAddressCount; i1++)
        {
            bufferSize += 6; /* u8 primitive->getAddresses[i1].a[6] */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeBlacklistCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeBlacklistCfm *primitive = (CsrWifiSmeBlacklistCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->getAddressCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getAddressCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->getAddresses[i1].a, ((u16) (6)));
        }
    }
    return(ptr);
}


void* CsrWifiSmeBlacklistCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeBlacklistCfm *primitive = kmalloc(sizeof(CsrWifiSmeBlacklistCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->getAddressCount, buffer, &offset);
    primitive->getAddresses = NULL;
    if (primitive->getAddressCount)
    {
        primitive->getAddresses = kmalloc(sizeof(CsrWifiMacAddress) * primitive->getAddressCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getAddressCount; i1++)
        {
            CsrMemCpyDes(primitive->getAddresses[i1].a, buffer, &offset, ((u16) (6)));
        }
    }

    return primitive;
}


void CsrWifiSmeBlacklistCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeBlacklistCfm *primitive = (CsrWifiSmeBlacklistCfm *) voidPrimitivePointer;
    kfree(primitive->getAddresses);
    kfree(primitive);
}


size_t CsrWifiSmeCalibrationDataGetCfmSizeof(void *msg)
{
    CsrWifiSmeCalibrationDataGetCfm *primitive = (CsrWifiSmeCalibrationDataGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 2;                                /* CsrResult primitive->status */
    bufferSize += 2;                                /* u16 primitive->calibrationDataLength */
    bufferSize += primitive->calibrationDataLength; /* u8 primitive->calibrationData */
    return bufferSize;
}


u8* CsrWifiSmeCalibrationDataGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCalibrationDataGetCfm *primitive = (CsrWifiSmeCalibrationDataGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint16Ser(ptr, len, (u16) primitive->calibrationDataLength);
    if (primitive->calibrationDataLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->calibrationData, ((u16) (primitive->calibrationDataLength)));
    }
    return(ptr);
}


void* CsrWifiSmeCalibrationDataGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCalibrationDataGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeCalibrationDataGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->calibrationDataLength, buffer, &offset);
    if (primitive->calibrationDataLength)
    {
        primitive->calibrationData = kmalloc(primitive->calibrationDataLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->calibrationData, buffer, &offset, ((u16) (primitive->calibrationDataLength)));
    }
    else
    {
        primitive->calibrationData = NULL;
    }

    return primitive;
}


void CsrWifiSmeCalibrationDataGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeCalibrationDataGetCfm *primitive = (CsrWifiSmeCalibrationDataGetCfm *) voidPrimitivePointer;
    kfree(primitive->calibrationData);
    kfree(primitive);
}


size_t CsrWifiSmeCcxConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 11) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->ccxConfig.keepAliveTimeMs */
    bufferSize += 1; /* u8 primitive->ccxConfig.apRoamingEnabled */
    bufferSize += 1; /* u8 primitive->ccxConfig.measurementsMask */
    bufferSize += 1; /* u8 primitive->ccxConfig.ccxRadioMgtEnabled */
    return bufferSize;
}


u8* CsrWifiSmeCcxConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCcxConfigGetCfm *primitive = (CsrWifiSmeCcxConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.keepAliveTimeMs);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.apRoamingEnabled);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.measurementsMask);
    CsrUint8Ser(ptr, len, (u8) primitive->ccxConfig.ccxRadioMgtEnabled);
    return(ptr);
}


void* CsrWifiSmeCcxConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCcxConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeCcxConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.keepAliveTimeMs, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.apRoamingEnabled, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.measurementsMask, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->ccxConfig.ccxRadioMgtEnabled, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeCcxConfigSetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmeCcxConfigSetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCcxConfigSetCfm *primitive = (CsrWifiSmeCcxConfigSetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmeCcxConfigSetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCcxConfigSetCfm *primitive = kmalloc(sizeof(CsrWifiSmeCcxConfigSetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeCoexConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 31) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->coexConfig.coexEnableSchemeManagement */
    bufferSize += 1; /* u8 primitive->coexConfig.coexPeriodicWakeHost */
    bufferSize += 2; /* u16 primitive->coexConfig.coexTrafficBurstyLatencyMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexTrafficContinuousLatencyMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexObexBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexObexBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpBrBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpBrBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpEdrBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexA2dpEdrBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexPagingBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexPagingBlackoutPeriodMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexInquiryBlackoutDurationMs */
    bufferSize += 2; /* u16 primitive->coexConfig.coexInquiryBlackoutPeriodMs */
    return bufferSize;
}


u8* CsrWifiSmeCoexConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCoexConfigGetCfm *primitive = (CsrWifiSmeCoexConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->coexConfig.coexEnableSchemeManagement);
    CsrUint8Ser(ptr, len, (u8) primitive->coexConfig.coexPeriodicWakeHost);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexTrafficBurstyLatencyMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexTrafficContinuousLatencyMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexObexBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexObexBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpBrBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpBrBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpEdrBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexA2dpEdrBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexPagingBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexPagingBlackoutPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexInquiryBlackoutDurationMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexConfig.coexInquiryBlackoutPeriodMs);
    return(ptr);
}


void* CsrWifiSmeCoexConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCoexConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeCoexConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexConfig.coexEnableSchemeManagement, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexConfig.coexPeriodicWakeHost, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexTrafficBurstyLatencyMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexTrafficContinuousLatencyMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexObexBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexObexBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpBrBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpBrBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpEdrBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexA2dpEdrBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexPagingBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexPagingBlackoutPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexInquiryBlackoutDurationMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexConfig.coexInquiryBlackoutPeriodMs, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeCoexInfoGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 24) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->coexInfo.hasTrafficData */
    bufferSize += 1; /* CsrWifiSmeTrafficType primitive->coexInfo.currentTrafficType */
    bufferSize += 2; /* u16 primitive->coexInfo.currentPeriodMs */
    bufferSize += 1; /* CsrWifiSmePowerSaveLevel primitive->coexInfo.currentPowerSave */
    bufferSize += 2; /* u16 primitive->coexInfo.currentCoexPeriodMs */
    bufferSize += 2; /* u16 primitive->coexInfo.currentCoexLatencyMs */
    bufferSize += 1; /* u8 primitive->coexInfo.hasBtDevice */
    bufferSize += 4; /* u32 primitive->coexInfo.currentBlackoutDurationUs */
    bufferSize += 4; /* u32 primitive->coexInfo.currentBlackoutPeriodUs */
    bufferSize += 1; /* CsrWifiSmeCoexScheme primitive->coexInfo.currentCoexScheme */
    return bufferSize;
}


u8* CsrWifiSmeCoexInfoGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCoexInfoGetCfm *primitive = (CsrWifiSmeCoexInfoGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->coexInfo.hasTrafficData);
    CsrUint8Ser(ptr, len, (u8) primitive->coexInfo.currentTrafficType);
    CsrUint16Ser(ptr, len, (u16) primitive->coexInfo.currentPeriodMs);
    CsrUint8Ser(ptr, len, (u8) primitive->coexInfo.currentPowerSave);
    CsrUint16Ser(ptr, len, (u16) primitive->coexInfo.currentCoexPeriodMs);
    CsrUint16Ser(ptr, len, (u16) primitive->coexInfo.currentCoexLatencyMs);
    CsrUint8Ser(ptr, len, (u8) primitive->coexInfo.hasBtDevice);
    CsrUint32Ser(ptr, len, (u32) primitive->coexInfo.currentBlackoutDurationUs);
    CsrUint32Ser(ptr, len, (u32) primitive->coexInfo.currentBlackoutPeriodUs);
    CsrUint8Ser(ptr, len, (u8) primitive->coexInfo.currentCoexScheme);
    return(ptr);
}


void* CsrWifiSmeCoexInfoGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCoexInfoGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeCoexInfoGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexInfo.hasTrafficData, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexInfo.currentTrafficType, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexInfo.currentPeriodMs, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexInfo.currentPowerSave, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexInfo.currentCoexPeriodMs, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->coexInfo.currentCoexLatencyMs, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexInfo.hasBtDevice, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->coexInfo.currentBlackoutDurationUs, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->coexInfo.currentBlackoutPeriodUs, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->coexInfo.currentCoexScheme, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeConnectCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmeConnectCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeConnectCfm *primitive = (CsrWifiSmeConnectCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmeConnectCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeConnectCfm *primitive = kmalloc(sizeof(CsrWifiSmeConnectCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeConnectionConfigGetCfmSizeof(void *msg)
{
    CsrWifiSmeConnectionConfigGetCfm *primitive = (CsrWifiSmeConnectionConfigGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 59) */
    bufferSize += 2;                                                                     /* u16 primitive->interfaceTag */
    bufferSize += 2;                                                                     /* CsrResult primitive->status */
    bufferSize += 32;                                                                    /* u8 primitive->connectionConfig.ssid.ssid[32] */
    bufferSize += 1;                                                                     /* u8 primitive->connectionConfig.ssid.length */
    bufferSize += 6;                                                                     /* u8 primitive->connectionConfig.bssid.a[6] */
    bufferSize += 1;                                                                     /* CsrWifiSmeBssType primitive->connectionConfig.bssType */
    bufferSize += 1;                                                                     /* CsrWifiSmeRadioIF primitive->connectionConfig.ifIndex */
    bufferSize += 1;                                                                     /* CsrWifiSme80211PrivacyMode primitive->connectionConfig.privacyMode */
    bufferSize += 2;                                                                     /* CsrWifiSmeAuthModeMask primitive->connectionConfig.authModeMask */
    bufferSize += 2;                                                                     /* CsrWifiSmeEncryptionMask primitive->connectionConfig.encryptionModeMask */
    bufferSize += 2;                                                                     /* u16 primitive->connectionConfig.mlmeAssociateReqInformationElementsLength */
    bufferSize += primitive->connectionConfig.mlmeAssociateReqInformationElementsLength; /* u8 primitive->connectionConfig.mlmeAssociateReqInformationElements */
    bufferSize += 1;                                                                     /* CsrWifiSmeWmmQosInfoMask primitive->connectionConfig.wmmQosInfo */
    bufferSize += 1;                                                                     /* u8 primitive->connectionConfig.adhocJoinOnly */
    bufferSize += 1;                                                                     /* u8 primitive->connectionConfig.adhocChannel */
    return bufferSize;
}


u8* CsrWifiSmeConnectionConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeConnectionConfigGetCfm *primitive = (CsrWifiSmeConnectionConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionConfig.ssid.ssid, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.ssid.length);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionConfig.bssid.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.bssType);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.ifIndex);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.privacyMode);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionConfig.authModeMask);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionConfig.encryptionModeMask);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionConfig.mlmeAssociateReqInformationElementsLength);
    if (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionConfig.mlmeAssociateReqInformationElements, ((u16) (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)));
    }
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.wmmQosInfo);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.adhocJoinOnly);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionConfig.adhocChannel);
    return(ptr);
}


void* CsrWifiSmeConnectionConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeConnectionConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeConnectionConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrMemCpyDes(primitive->connectionConfig.ssid.ssid, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->connectionConfig.ssid.length, buffer, &offset);
    CsrMemCpyDes(primitive->connectionConfig.bssid.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->connectionConfig.bssType, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.ifIndex, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.privacyMode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionConfig.authModeMask, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionConfig.encryptionModeMask, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionConfig.mlmeAssociateReqInformationElementsLength, buffer, &offset);
    if (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)
    {
        primitive->connectionConfig.mlmeAssociateReqInformationElements = kmalloc(primitive->connectionConfig.mlmeAssociateReqInformationElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionConfig.mlmeAssociateReqInformationElements, buffer, &offset, ((u16) (primitive->connectionConfig.mlmeAssociateReqInformationElementsLength)));
    }
    else
    {
        primitive->connectionConfig.mlmeAssociateReqInformationElements = NULL;
    }
    CsrUint8Des((u8 *) &primitive->connectionConfig.wmmQosInfo, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.adhocJoinOnly, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionConfig.adhocChannel, buffer, &offset);

    return primitive;
}


void CsrWifiSmeConnectionConfigGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeConnectionConfigGetCfm *primitive = (CsrWifiSmeConnectionConfigGetCfm *) voidPrimitivePointer;
    kfree(primitive->connectionConfig.mlmeAssociateReqInformationElements);
    kfree(primitive);
}


size_t CsrWifiSmeConnectionInfoGetCfmSizeof(void *msg)
{
    CsrWifiSmeConnectionInfoGetCfm *primitive = (CsrWifiSmeConnectionInfoGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 96) */
    bufferSize += 2;                                                     /* u16 primitive->interfaceTag */
    bufferSize += 2;                                                     /* CsrResult primitive->status */
    bufferSize += 32;                                                    /* u8 primitive->connectionInfo.ssid.ssid[32] */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.ssid.length */
    bufferSize += 6;                                                     /* u8 primitive->connectionInfo.bssid.a[6] */
    bufferSize += 1;                                                     /* CsrWifiSme80211NetworkType primitive->connectionInfo.networkType80211 */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.channelNumber */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.channelFrequency */
    bufferSize += 2;                                                     /* CsrWifiSmeAuthMode primitive->connectionInfo.authMode */
    bufferSize += 2;                                                     /* CsrWifiSmeEncryption primitive->connectionInfo.pairwiseCipher */
    bufferSize += 2;                                                     /* CsrWifiSmeEncryption primitive->connectionInfo.groupCipher */
    bufferSize += 1;                                                     /* CsrWifiSmeRadioIF primitive->connectionInfo.ifIndex */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.atimWindowTu */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.beaconPeriodTu */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.reassociation */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.beaconFrameLength */
    bufferSize += primitive->connectionInfo.beaconFrameLength;           /* u8 primitive->connectionInfo.beaconFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.associationReqFrameLength */
    bufferSize += primitive->connectionInfo.associationReqFrameLength;   /* u8 primitive->connectionInfo.associationReqFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.associationRspFrameLength */
    bufferSize += primitive->connectionInfo.associationRspFrameLength;   /* u8 primitive->connectionInfo.associationRspFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocScanInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocScanInfoElementsLength; /* u8 primitive->connectionInfo.assocScanInfoElements */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqCapabilities */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqListenIntervalTu */
    bufferSize += 6;                                                     /* u8 primitive->connectionInfo.assocReqApAddress.a[6] */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocReqInfoElementsLength;  /* u8 primitive->connectionInfo.assocReqInfoElements */
    bufferSize += 2;                                                     /* CsrWifiSmeIEEE80211Result primitive->connectionInfo.assocRspResult */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspCapabilityInfo */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspAssociationId */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocRspInfoElementsLength;  /* u8 primitive->connectionInfo.assocRspInfoElements */
    return bufferSize;
}


u8* CsrWifiSmeConnectionInfoGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeConnectionInfoGetCfm *primitive = (CsrWifiSmeConnectionInfoGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.ssid.ssid, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.ssid.length);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.bssid.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.networkType80211);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.channelNumber);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.channelFrequency);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.authMode);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.pairwiseCipher);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.groupCipher);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.ifIndex);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.atimWindowTu);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.beaconPeriodTu);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.reassociation);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.beaconFrameLength);
    if (primitive->connectionInfo.beaconFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.beaconFrame, ((u16) (primitive->connectionInfo.beaconFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.associationReqFrameLength);
    if (primitive->connectionInfo.associationReqFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.associationReqFrame, ((u16) (primitive->connectionInfo.associationReqFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.associationRspFrameLength);
    if (primitive->connectionInfo.associationRspFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.associationRspFrame, ((u16) (primitive->connectionInfo.associationRspFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocScanInfoElementsLength);
    if (primitive->connectionInfo.assocScanInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocScanInfoElements, ((u16) (primitive->connectionInfo.assocScanInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqCapabilities);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqListenIntervalTu);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocReqApAddress.a, ((u16) (6)));
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqInfoElementsLength);
    if (primitive->connectionInfo.assocReqInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocReqInfoElements, ((u16) (primitive->connectionInfo.assocReqInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspResult);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspCapabilityInfo);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspAssociationId);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspInfoElementsLength);
    if (primitive->connectionInfo.assocRspInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocRspInfoElements, ((u16) (primitive->connectionInfo.assocRspInfoElementsLength)));
    }
    return(ptr);
}


void* CsrWifiSmeConnectionInfoGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeConnectionInfoGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeConnectionInfoGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.ssid.ssid, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->connectionInfo.ssid.length, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.bssid.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->connectionInfo.networkType80211, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.channelNumber, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.channelFrequency, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.authMode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.pairwiseCipher, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.groupCipher, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.ifIndex, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.atimWindowTu, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.beaconPeriodTu, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.reassociation, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.beaconFrameLength, buffer, &offset);
    if (primitive->connectionInfo.beaconFrameLength)
    {
        primitive->connectionInfo.beaconFrame = kmalloc(primitive->connectionInfo.beaconFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.beaconFrame, buffer, &offset, ((u16) (primitive->connectionInfo.beaconFrameLength)));
    }
    else
    {
        primitive->connectionInfo.beaconFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.associationReqFrameLength, buffer, &offset);
    if (primitive->connectionInfo.associationReqFrameLength)
    {
        primitive->connectionInfo.associationReqFrame = kmalloc(primitive->connectionInfo.associationReqFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.associationReqFrame, buffer, &offset, ((u16) (primitive->connectionInfo.associationReqFrameLength)));
    }
    else
    {
        primitive->connectionInfo.associationReqFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.associationRspFrameLength, buffer, &offset);
    if (primitive->connectionInfo.associationRspFrameLength)
    {
        primitive->connectionInfo.associationRspFrame = kmalloc(primitive->connectionInfo.associationRspFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.associationRspFrame, buffer, &offset, ((u16) (primitive->connectionInfo.associationRspFrameLength)));
    }
    else
    {
        primitive->connectionInfo.associationRspFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocScanInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocScanInfoElementsLength)
    {
        primitive->connectionInfo.assocScanInfoElements = kmalloc(primitive->connectionInfo.assocScanInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocScanInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocScanInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocScanInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqCapabilities, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqListenIntervalTu, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.assocReqApAddress.a, buffer, &offset, ((u16) (6)));
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocReqInfoElementsLength)
    {
        primitive->connectionInfo.assocReqInfoElements = kmalloc(primitive->connectionInfo.assocReqInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocReqInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocReqInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocReqInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspResult, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspCapabilityInfo, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspAssociationId, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocRspInfoElementsLength)
    {
        primitive->connectionInfo.assocRspInfoElements = kmalloc(primitive->connectionInfo.assocRspInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocRspInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocRspInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocRspInfoElements = NULL;
    }

    return primitive;
}


void CsrWifiSmeConnectionInfoGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeConnectionInfoGetCfm *primitive = (CsrWifiSmeConnectionInfoGetCfm *) voidPrimitivePointer;
    kfree(primitive->connectionInfo.beaconFrame);
    kfree(primitive->connectionInfo.associationReqFrame);
    kfree(primitive->connectionInfo.associationRspFrame);
    kfree(primitive->connectionInfo.assocScanInfoElements);
    kfree(primitive->connectionInfo.assocReqInfoElements);
    kfree(primitive->connectionInfo.assocRspInfoElements);
    kfree(primitive);
}


size_t CsrWifiSmeConnectionQualityIndSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 9) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* s16 primitive->linkQuality.unifiRssi */
    bufferSize += 2; /* s16 primitive->linkQuality.unifiSnr */
    return bufferSize;
}


u8* CsrWifiSmeConnectionQualityIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeConnectionQualityInd *primitive = (CsrWifiSmeConnectionQualityInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->linkQuality.unifiRssi);
    CsrUint16Ser(ptr, len, (u16) primitive->linkQuality.unifiSnr);
    return(ptr);
}


void* CsrWifiSmeConnectionQualityIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeConnectionQualityInd *primitive = kmalloc(sizeof(CsrWifiSmeConnectionQualityInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->linkQuality.unifiRssi, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->linkQuality.unifiSnr, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeConnectionStatsGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 101) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->connectionStats.unifiTxDataRate */
    bufferSize += 1; /* u8 primitive->connectionStats.unifiRxDataRate */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RetryCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11MultipleRetryCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11AckFailureCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11FrameDuplicateCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11FcsErrorCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RtsSuccessCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RtsFailureCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11FailedCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11TransmittedFragmentCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11TransmittedFrameCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11WepExcludedCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11WepIcvErrorCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11WepUndecryptableCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11MulticastReceivedFrameCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11MulticastTransmittedFrameCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11ReceivedFragmentCount */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11Rsna4WayHandshakeFailures */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RsnaTkipCounterMeasuresInvoked */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RsnaStatsTkipLocalMicFailures */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RsnaStatsTkipReplays */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RsnaStatsTkipIcvErrors */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RsnaStatsCcmpReplays */
    bufferSize += 4; /* u32 primitive->connectionStats.dot11RsnaStatsCcmpDecryptErrors */
    return bufferSize;
}


u8* CsrWifiSmeConnectionStatsGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeConnectionStatsGetCfm *primitive = (CsrWifiSmeConnectionStatsGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionStats.unifiTxDataRate);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionStats.unifiRxDataRate);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RetryCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11MultipleRetryCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11AckFailureCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11FrameDuplicateCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11FcsErrorCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RtsSuccessCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RtsFailureCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11FailedCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11TransmittedFragmentCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11TransmittedFrameCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11WepExcludedCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11WepIcvErrorCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11WepUndecryptableCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11MulticastReceivedFrameCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11MulticastTransmittedFrameCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11ReceivedFragmentCount);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11Rsna4WayHandshakeFailures);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RsnaTkipCounterMeasuresInvoked);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RsnaStatsTkipLocalMicFailures);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RsnaStatsTkipReplays);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RsnaStatsTkipIcvErrors);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RsnaStatsCcmpReplays);
    CsrUint32Ser(ptr, len, (u32) primitive->connectionStats.dot11RsnaStatsCcmpDecryptErrors);
    return(ptr);
}


void* CsrWifiSmeConnectionStatsGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeConnectionStatsGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeConnectionStatsGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionStats.unifiTxDataRate, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionStats.unifiRxDataRate, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RetryCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11MultipleRetryCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11AckFailureCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11FrameDuplicateCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11FcsErrorCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RtsSuccessCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RtsFailureCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11FailedCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11TransmittedFragmentCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11TransmittedFrameCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11WepExcludedCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11WepIcvErrorCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11WepUndecryptableCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11MulticastReceivedFrameCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11MulticastTransmittedFrameCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11ReceivedFragmentCount, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11Rsna4WayHandshakeFailures, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RsnaTkipCounterMeasuresInvoked, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RsnaStatsTkipLocalMicFailures, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RsnaStatsTkipReplays, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RsnaStatsTkipIcvErrors, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RsnaStatsCcmpReplays, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->connectionStats.dot11RsnaStatsCcmpDecryptErrors, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeDisconnectCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmeDisconnectCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeDisconnectCfm *primitive = (CsrWifiSmeDisconnectCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmeDisconnectCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeDisconnectCfm *primitive = kmalloc(sizeof(CsrWifiSmeDisconnectCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeHostConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 10) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* CsrWifiSmeHostPowerMode primitive->hostConfig.powerMode */
    bufferSize += 2; /* u16 primitive->hostConfig.applicationDataPeriodMs */
    return bufferSize;
}


u8* CsrWifiSmeHostConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeHostConfigGetCfm *primitive = (CsrWifiSmeHostConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->hostConfig.powerMode);
    CsrUint16Ser(ptr, len, (u16) primitive->hostConfig.applicationDataPeriodMs);
    return(ptr);
}


void* CsrWifiSmeHostConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeHostConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeHostConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->hostConfig.powerMode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->hostConfig.applicationDataPeriodMs, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeHostConfigSetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmeHostConfigSetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeHostConfigSetCfm *primitive = (CsrWifiSmeHostConfigSetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmeHostConfigSetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeHostConfigSetCfm *primitive = kmalloc(sizeof(CsrWifiSmeHostConfigSetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeIbssStationIndSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 10) */
    bufferSize += 6; /* u8 primitive->address.a[6] */
    bufferSize += 1; /* u8 primitive->isconnected */
    return bufferSize;
}


u8* CsrWifiSmeIbssStationIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeIbssStationInd *primitive = (CsrWifiSmeIbssStationInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrMemCpySer(ptr, len, (const void *) primitive->address.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->isconnected);
    return(ptr);
}


void* CsrWifiSmeIbssStationIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeIbssStationInd *primitive = kmalloc(sizeof(CsrWifiSmeIbssStationInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrMemCpyDes(primitive->address.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->isconnected, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeKeyCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 15) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* CsrWifiSmeKeyType primitive->keyType */
    bufferSize += 6; /* u8 primitive->peerMacAddress.a[6] */
    return bufferSize;
}


u8* CsrWifiSmeKeyCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeKeyCfm *primitive = (CsrWifiSmeKeyCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->keyType);
    CsrMemCpySer(ptr, len, (const void *) primitive->peerMacAddress.a, ((u16) (6)));
    return(ptr);
}


void* CsrWifiSmeKeyCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeKeyCfm *primitive = kmalloc(sizeof(CsrWifiSmeKeyCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->keyType, buffer, &offset);
    CsrMemCpyDes(primitive->peerMacAddress.a, buffer, &offset, ((u16) (6)));

    return primitive;
}


size_t CsrWifiSmeLinkQualityGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 11) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 2; /* s16 primitive->linkQuality.unifiRssi */
    bufferSize += 2; /* s16 primitive->linkQuality.unifiSnr */
    return bufferSize;
}


u8* CsrWifiSmeLinkQualityGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeLinkQualityGetCfm *primitive = (CsrWifiSmeLinkQualityGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint16Ser(ptr, len, (u16) primitive->linkQuality.unifiRssi);
    CsrUint16Ser(ptr, len, (u16) primitive->linkQuality.unifiSnr);
    return(ptr);
}


void* CsrWifiSmeLinkQualityGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeLinkQualityGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeLinkQualityGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->linkQuality.unifiRssi, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->linkQuality.unifiSnr, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeMediaStatusIndSizeof(void *msg)
{
    CsrWifiSmeMediaStatusInd *primitive = (CsrWifiSmeMediaStatusInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 99) */
    bufferSize += 2;                                                     /* u16 primitive->interfaceTag */
    bufferSize += 1;                                                     /* CsrWifiSmeMediaStatus primitive->mediaStatus */
    bufferSize += 32;                                                    /* u8 primitive->connectionInfo.ssid.ssid[32] */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.ssid.length */
    bufferSize += 6;                                                     /* u8 primitive->connectionInfo.bssid.a[6] */
    bufferSize += 1;                                                     /* CsrWifiSme80211NetworkType primitive->connectionInfo.networkType80211 */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.channelNumber */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.channelFrequency */
    bufferSize += 2;                                                     /* CsrWifiSmeAuthMode primitive->connectionInfo.authMode */
    bufferSize += 2;                                                     /* CsrWifiSmeEncryption primitive->connectionInfo.pairwiseCipher */
    bufferSize += 2;                                                     /* CsrWifiSmeEncryption primitive->connectionInfo.groupCipher */
    bufferSize += 1;                                                     /* CsrWifiSmeRadioIF primitive->connectionInfo.ifIndex */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.atimWindowTu */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.beaconPeriodTu */
    bufferSize += 1;                                                     /* u8 primitive->connectionInfo.reassociation */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.beaconFrameLength */
    bufferSize += primitive->connectionInfo.beaconFrameLength;           /* u8 primitive->connectionInfo.beaconFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.associationReqFrameLength */
    bufferSize += primitive->connectionInfo.associationReqFrameLength;   /* u8 primitive->connectionInfo.associationReqFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.associationRspFrameLength */
    bufferSize += primitive->connectionInfo.associationRspFrameLength;   /* u8 primitive->connectionInfo.associationRspFrame */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocScanInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocScanInfoElementsLength; /* u8 primitive->connectionInfo.assocScanInfoElements */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqCapabilities */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqListenIntervalTu */
    bufferSize += 6;                                                     /* u8 primitive->connectionInfo.assocReqApAddress.a[6] */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocReqInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocReqInfoElementsLength;  /* u8 primitive->connectionInfo.assocReqInfoElements */
    bufferSize += 2;                                                     /* CsrWifiSmeIEEE80211Result primitive->connectionInfo.assocRspResult */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspCapabilityInfo */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspAssociationId */
    bufferSize += 2;                                                     /* u16 primitive->connectionInfo.assocRspInfoElementsLength */
    bufferSize += primitive->connectionInfo.assocRspInfoElementsLength;  /* u8 primitive->connectionInfo.assocRspInfoElements */
    bufferSize += 2;                                                     /* CsrWifiSmeIEEE80211Reason primitive->disassocReason */
    bufferSize += 2;                                                     /* CsrWifiSmeIEEE80211Reason primitive->deauthReason */
    return bufferSize;
}


u8* CsrWifiSmeMediaStatusIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMediaStatusInd *primitive = (CsrWifiSmeMediaStatusInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->mediaStatus);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.ssid.ssid, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.ssid.length);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.bssid.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.networkType80211);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.channelNumber);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.channelFrequency);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.authMode);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.pairwiseCipher);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.groupCipher);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.ifIndex);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.atimWindowTu);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.beaconPeriodTu);
    CsrUint8Ser(ptr, len, (u8) primitive->connectionInfo.reassociation);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.beaconFrameLength);
    if (primitive->connectionInfo.beaconFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.beaconFrame, ((u16) (primitive->connectionInfo.beaconFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.associationReqFrameLength);
    if (primitive->connectionInfo.associationReqFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.associationReqFrame, ((u16) (primitive->connectionInfo.associationReqFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.associationRspFrameLength);
    if (primitive->connectionInfo.associationRspFrameLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.associationRspFrame, ((u16) (primitive->connectionInfo.associationRspFrameLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocScanInfoElementsLength);
    if (primitive->connectionInfo.assocScanInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocScanInfoElements, ((u16) (primitive->connectionInfo.assocScanInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqCapabilities);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqListenIntervalTu);
    CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocReqApAddress.a, ((u16) (6)));
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocReqInfoElementsLength);
    if (primitive->connectionInfo.assocReqInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocReqInfoElements, ((u16) (primitive->connectionInfo.assocReqInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspResult);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspCapabilityInfo);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspAssociationId);
    CsrUint16Ser(ptr, len, (u16) primitive->connectionInfo.assocRspInfoElementsLength);
    if (primitive->connectionInfo.assocRspInfoElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->connectionInfo.assocRspInfoElements, ((u16) (primitive->connectionInfo.assocRspInfoElementsLength)));
    }
    CsrUint16Ser(ptr, len, (u16) primitive->disassocReason);
    CsrUint16Ser(ptr, len, (u16) primitive->deauthReason);
    return(ptr);
}


void* CsrWifiSmeMediaStatusIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMediaStatusInd *primitive = kmalloc(sizeof(CsrWifiSmeMediaStatusInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->mediaStatus, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.ssid.ssid, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->connectionInfo.ssid.length, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.bssid.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->connectionInfo.networkType80211, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.channelNumber, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.channelFrequency, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.authMode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.pairwiseCipher, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.groupCipher, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.ifIndex, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.atimWindowTu, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.beaconPeriodTu, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->connectionInfo.reassociation, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.beaconFrameLength, buffer, &offset);
    if (primitive->connectionInfo.beaconFrameLength)
    {
        primitive->connectionInfo.beaconFrame = kmalloc(primitive->connectionInfo.beaconFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.beaconFrame, buffer, &offset, ((u16) (primitive->connectionInfo.beaconFrameLength)));
    }
    else
    {
        primitive->connectionInfo.beaconFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.associationReqFrameLength, buffer, &offset);
    if (primitive->connectionInfo.associationReqFrameLength)
    {
        primitive->connectionInfo.associationReqFrame = kmalloc(primitive->connectionInfo.associationReqFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.associationReqFrame, buffer, &offset, ((u16) (primitive->connectionInfo.associationReqFrameLength)));
    }
    else
    {
        primitive->connectionInfo.associationReqFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.associationRspFrameLength, buffer, &offset);
    if (primitive->connectionInfo.associationRspFrameLength)
    {
        primitive->connectionInfo.associationRspFrame = kmalloc(primitive->connectionInfo.associationRspFrameLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.associationRspFrame, buffer, &offset, ((u16) (primitive->connectionInfo.associationRspFrameLength)));
    }
    else
    {
        primitive->connectionInfo.associationRspFrame = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocScanInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocScanInfoElementsLength)
    {
        primitive->connectionInfo.assocScanInfoElements = kmalloc(primitive->connectionInfo.assocScanInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocScanInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocScanInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocScanInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqCapabilities, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqListenIntervalTu, buffer, &offset);
    CsrMemCpyDes(primitive->connectionInfo.assocReqApAddress.a, buffer, &offset, ((u16) (6)));
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocReqInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocReqInfoElementsLength)
    {
        primitive->connectionInfo.assocReqInfoElements = kmalloc(primitive->connectionInfo.assocReqInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocReqInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocReqInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocReqInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspResult, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspCapabilityInfo, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspAssociationId, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->connectionInfo.assocRspInfoElementsLength, buffer, &offset);
    if (primitive->connectionInfo.assocRspInfoElementsLength)
    {
        primitive->connectionInfo.assocRspInfoElements = kmalloc(primitive->connectionInfo.assocRspInfoElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->connectionInfo.assocRspInfoElements, buffer, &offset, ((u16) (primitive->connectionInfo.assocRspInfoElementsLength)));
    }
    else
    {
        primitive->connectionInfo.assocRspInfoElements = NULL;
    }
    CsrUint16Des((u16 *) &primitive->disassocReason, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->deauthReason, buffer, &offset);

    return primitive;
}


void CsrWifiSmeMediaStatusIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMediaStatusInd *primitive = (CsrWifiSmeMediaStatusInd *) voidPrimitivePointer;
    kfree(primitive->connectionInfo.beaconFrame);
    kfree(primitive->connectionInfo.associationReqFrame);
    kfree(primitive->connectionInfo.associationRspFrame);
    kfree(primitive->connectionInfo.assocScanInfoElements);
    kfree(primitive->connectionInfo.assocReqInfoElements);
    kfree(primitive->connectionInfo.assocRspInfoElements);
    kfree(primitive);
}


size_t CsrWifiSmeMibConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->mibConfig.unifiFixMaxTxDataRate */
    bufferSize += 1; /* u8 primitive->mibConfig.unifiFixTxDataRate */
    bufferSize += 2; /* u16 primitive->mibConfig.dot11RtsThreshold */
    bufferSize += 2; /* u16 primitive->mibConfig.dot11FragmentationThreshold */
    bufferSize += 2; /* u16 primitive->mibConfig.dot11CurrentTxPowerLevel */
    return bufferSize;
}


u8* CsrWifiSmeMibConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMibConfigGetCfm *primitive = (CsrWifiSmeMibConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->mibConfig.unifiFixMaxTxDataRate);
    CsrUint8Ser(ptr, len, (u8) primitive->mibConfig.unifiFixTxDataRate);
    CsrUint16Ser(ptr, len, (u16) primitive->mibConfig.dot11RtsThreshold);
    CsrUint16Ser(ptr, len, (u16) primitive->mibConfig.dot11FragmentationThreshold);
    CsrUint16Ser(ptr, len, (u16) primitive->mibConfig.dot11CurrentTxPowerLevel);
    return(ptr);
}


void* CsrWifiSmeMibConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMibConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeMibConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->mibConfig.unifiFixMaxTxDataRate, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->mibConfig.unifiFixTxDataRate, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibConfig.dot11RtsThreshold, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibConfig.dot11FragmentationThreshold, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibConfig.dot11CurrentTxPowerLevel, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeMibGetCfmSizeof(void *msg)
{
    CsrWifiSmeMibGetCfm *primitive = (CsrWifiSmeMibGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 2;                             /* CsrResult primitive->status */
    bufferSize += 2;                             /* u16 primitive->mibAttributeLength */
    bufferSize += primitive->mibAttributeLength; /* u8 primitive->mibAttribute */
    return bufferSize;
}


u8* CsrWifiSmeMibGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMibGetCfm *primitive = (CsrWifiSmeMibGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint16Ser(ptr, len, (u16) primitive->mibAttributeLength);
    if (primitive->mibAttributeLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->mibAttribute, ((u16) (primitive->mibAttributeLength)));
    }
    return(ptr);
}


void* CsrWifiSmeMibGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMibGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeMibGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibAttributeLength, buffer, &offset);
    if (primitive->mibAttributeLength)
    {
        primitive->mibAttribute = kmalloc(primitive->mibAttributeLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->mibAttribute, buffer, &offset, ((u16) (primitive->mibAttributeLength)));
    }
    else
    {
        primitive->mibAttribute = NULL;
    }

    return primitive;
}


void CsrWifiSmeMibGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMibGetCfm *primitive = (CsrWifiSmeMibGetCfm *) voidPrimitivePointer;
    kfree(primitive->mibAttribute);
    kfree(primitive);
}


size_t CsrWifiSmeMibGetNextCfmSizeof(void *msg)
{
    CsrWifiSmeMibGetNextCfm *primitive = (CsrWifiSmeMibGetNextCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 2;                             /* CsrResult primitive->status */
    bufferSize += 2;                             /* u16 primitive->mibAttributeLength */
    bufferSize += primitive->mibAttributeLength; /* u8 primitive->mibAttribute */
    return bufferSize;
}


u8* CsrWifiSmeMibGetNextCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMibGetNextCfm *primitive = (CsrWifiSmeMibGetNextCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint16Ser(ptr, len, (u16) primitive->mibAttributeLength);
    if (primitive->mibAttributeLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->mibAttribute, ((u16) (primitive->mibAttributeLength)));
    }
    return(ptr);
}


void* CsrWifiSmeMibGetNextCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMibGetNextCfm *primitive = kmalloc(sizeof(CsrWifiSmeMibGetNextCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->mibAttributeLength, buffer, &offset);
    if (primitive->mibAttributeLength)
    {
        primitive->mibAttribute = kmalloc(primitive->mibAttributeLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->mibAttribute, buffer, &offset, ((u16) (primitive->mibAttributeLength)));
    }
    else
    {
        primitive->mibAttribute = NULL;
    }

    return primitive;
}


void CsrWifiSmeMibGetNextCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMibGetNextCfm *primitive = (CsrWifiSmeMibGetNextCfm *) voidPrimitivePointer;
    kfree(primitive->mibAttribute);
    kfree(primitive);
}


size_t CsrWifiSmeMicFailureIndSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 15) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* u8 primitive->secondFailure */
    bufferSize += 2; /* u16 primitive->count */
    bufferSize += 6; /* u8 primitive->address.a[6] */
    bufferSize += 1; /* CsrWifiSmeKeyType primitive->keyType */
    return bufferSize;
}


u8* CsrWifiSmeMicFailureIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMicFailureInd *primitive = (CsrWifiSmeMicFailureInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->secondFailure);
    CsrUint16Ser(ptr, len, (u16) primitive->count);
    CsrMemCpySer(ptr, len, (const void *) primitive->address.a, ((u16) (6)));
    CsrUint8Ser(ptr, len, (u8) primitive->keyType);
    return(ptr);
}


void* CsrWifiSmeMicFailureIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMicFailureInd *primitive = kmalloc(sizeof(CsrWifiSmeMicFailureInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->secondFailure, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->count, buffer, &offset);
    CsrMemCpyDes(primitive->address.a, buffer, &offset, ((u16) (6)));
    CsrUint8Des((u8 *) &primitive->keyType, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeMulticastAddressCfmSizeof(void *msg)
{
    CsrWifiSmeMulticastAddressCfm *primitive = (CsrWifiSmeMulticastAddressCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 15) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* u8 primitive->getAddressesCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getAddressesCount; i1++)
        {
            bufferSize += 6; /* u8 primitive->getAddresses[i1].a[6] */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeMulticastAddressCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeMulticastAddressCfm *primitive = (CsrWifiSmeMulticastAddressCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->getAddressesCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getAddressesCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->getAddresses[i1].a, ((u16) (6)));
        }
    }
    return(ptr);
}


void* CsrWifiSmeMulticastAddressCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeMulticastAddressCfm *primitive = kmalloc(sizeof(CsrWifiSmeMulticastAddressCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->getAddressesCount, buffer, &offset);
    primitive->getAddresses = NULL;
    if (primitive->getAddressesCount)
    {
        primitive->getAddresses = kmalloc(sizeof(CsrWifiMacAddress) * primitive->getAddressesCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getAddressesCount; i1++)
        {
            CsrMemCpyDes(primitive->getAddresses[i1].a, buffer, &offset, ((u16) (6)));
        }
    }

    return primitive;
}


void CsrWifiSmeMulticastAddressCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeMulticastAddressCfm *primitive = (CsrWifiSmeMulticastAddressCfm *) voidPrimitivePointer;
    kfree(primitive->getAddresses);
    kfree(primitive);
}


size_t CsrWifiSmePacketFilterSetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmePacketFilterSetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePacketFilterSetCfm *primitive = (CsrWifiSmePacketFilterSetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmePacketFilterSetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmePacketFilterSetCfm *primitive = kmalloc(sizeof(CsrWifiSmePacketFilterSetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmePermanentMacAddressGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 11) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 6; /* u8 primitive->permanentMacAddress.a[6] */
    return bufferSize;
}


u8* CsrWifiSmePermanentMacAddressGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePermanentMacAddressGetCfm *primitive = (CsrWifiSmePermanentMacAddressGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrMemCpySer(ptr, len, (const void *) primitive->permanentMacAddress.a, ((u16) (6)));
    return(ptr);
}


void* CsrWifiSmePermanentMacAddressGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmePermanentMacAddressGetCfm *primitive = kmalloc(sizeof(CsrWifiSmePermanentMacAddressGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrMemCpyDes(primitive->permanentMacAddress.a, buffer, &offset, ((u16) (6)));

    return primitive;
}


size_t CsrWifiSmePmkidCandidateListIndSizeof(void *msg)
{
    CsrWifiSmePmkidCandidateListInd *primitive = (CsrWifiSmePmkidCandidateListInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* u8 primitive->pmkidCandidatesCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->pmkidCandidatesCount; i1++)
        {
            bufferSize += 6; /* u8 primitive->pmkidCandidates[i1].bssid.a[6] */
            bufferSize += 1; /* u8 primitive->pmkidCandidates[i1].preAuthAllowed */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmePmkidCandidateListIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePmkidCandidateListInd *primitive = (CsrWifiSmePmkidCandidateListInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->pmkidCandidatesCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->pmkidCandidatesCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->pmkidCandidates[i1].bssid.a, ((u16) (6)));
            CsrUint8Ser(ptr, len, (u8) primitive->pmkidCandidates[i1].preAuthAllowed);
        }
    }
    return(ptr);
}


void* CsrWifiSmePmkidCandidateListIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmePmkidCandidateListInd *primitive = kmalloc(sizeof(CsrWifiSmePmkidCandidateListInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->pmkidCandidatesCount, buffer, &offset);
    primitive->pmkidCandidates = NULL;
    if (primitive->pmkidCandidatesCount)
    {
        primitive->pmkidCandidates = kmalloc(sizeof(CsrWifiSmePmkidCandidate) * primitive->pmkidCandidatesCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->pmkidCandidatesCount; i1++)
        {
            CsrMemCpyDes(primitive->pmkidCandidates[i1].bssid.a, buffer, &offset, ((u16) (6)));
            CsrUint8Des((u8 *) &primitive->pmkidCandidates[i1].preAuthAllowed, buffer, &offset);
        }
    }

    return primitive;
}


void CsrWifiSmePmkidCandidateListIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmePmkidCandidateListInd *primitive = (CsrWifiSmePmkidCandidateListInd *) voidPrimitivePointer;
    kfree(primitive->pmkidCandidates);
    kfree(primitive);
}


size_t CsrWifiSmePmkidCfmSizeof(void *msg)
{
    CsrWifiSmePmkidCfm *primitive = (CsrWifiSmePmkidCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 31) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* CsrWifiSmeListAction primitive->action */
    bufferSize += 1; /* u8 primitive->getPmkidsCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getPmkidsCount; i1++)
        {
            bufferSize += 6;  /* u8 primitive->getPmkids[i1].bssid.a[6] */
            bufferSize += 16; /* u8 primitive->getPmkids[i1].pmkid[16] */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmePmkidCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePmkidCfm *primitive = (CsrWifiSmePmkidCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->action);
    CsrUint8Ser(ptr, len, (u8) primitive->getPmkidsCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getPmkidsCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->getPmkids[i1].bssid.a, ((u16) (6)));
            CsrMemCpySer(ptr, len, (const void *) primitive->getPmkids[i1].pmkid, ((u16) (16)));
        }
    }
    return(ptr);
}


void* CsrWifiSmePmkidCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmePmkidCfm *primitive = kmalloc(sizeof(CsrWifiSmePmkidCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->action, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->getPmkidsCount, buffer, &offset);
    primitive->getPmkids = NULL;
    if (primitive->getPmkidsCount)
    {
        primitive->getPmkids = kmalloc(sizeof(CsrWifiSmePmkid) * primitive->getPmkidsCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->getPmkidsCount; i1++)
        {
            CsrMemCpyDes(primitive->getPmkids[i1].bssid.a, buffer, &offset, ((u16) (6)));
            CsrMemCpyDes(primitive->getPmkids[i1].pmkid, buffer, &offset, ((u16) (16)));
        }
    }

    return primitive;
}


void CsrWifiSmePmkidCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmePmkidCfm *primitive = (CsrWifiSmePmkidCfm *) voidPrimitivePointer;
    kfree(primitive->getPmkids);
    kfree(primitive);
}


size_t CsrWifiSmePowerConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* CsrWifiSmePowerSaveLevel primitive->powerConfig.powerSaveLevel */
    bufferSize += 2; /* u16 primitive->powerConfig.listenIntervalTu */
    bufferSize += 1; /* u8 primitive->powerConfig.rxDtims */
    bufferSize += 1; /* CsrWifiSmeD3AutoScanMode primitive->powerConfig.d3AutoScanMode */
    bufferSize += 1; /* u8 primitive->powerConfig.clientTrafficWindow */
    bufferSize += 1; /* u8 primitive->powerConfig.opportunisticPowerSave */
    bufferSize += 1; /* u8 primitive->powerConfig.noticeOfAbsence */
    return bufferSize;
}


u8* CsrWifiSmePowerConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmePowerConfigGetCfm *primitive = (CsrWifiSmePowerConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.powerSaveLevel);
    CsrUint16Ser(ptr, len, (u16) primitive->powerConfig.listenIntervalTu);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.rxDtims);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.d3AutoScanMode);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.clientTrafficWindow);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.opportunisticPowerSave);
    CsrUint8Ser(ptr, len, (u8) primitive->powerConfig.noticeOfAbsence);
    return(ptr);
}


void* CsrWifiSmePowerConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmePowerConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmePowerConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.powerSaveLevel, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->powerConfig.listenIntervalTu, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.rxDtims, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.d3AutoScanMode, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.clientTrafficWindow, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.opportunisticPowerSave, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->powerConfig.noticeOfAbsence, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeRegulatoryDomainInfoGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 10) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->regDomInfo.dot11MultiDomainCapabilityImplemented */
    bufferSize += 1; /* u8 primitive->regDomInfo.dot11MultiDomainCapabilityEnabled */
    bufferSize += 1; /* CsrWifiSmeRegulatoryDomain primitive->regDomInfo.currentRegulatoryDomain */
    bufferSize += 2; /* u8 primitive->regDomInfo.currentCountryCode[2] */
    return bufferSize;
}


u8* CsrWifiSmeRegulatoryDomainInfoGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeRegulatoryDomainInfoGetCfm *primitive = (CsrWifiSmeRegulatoryDomainInfoGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->regDomInfo.dot11MultiDomainCapabilityImplemented);
    CsrUint8Ser(ptr, len, (u8) primitive->regDomInfo.dot11MultiDomainCapabilityEnabled);
    CsrUint8Ser(ptr, len, (u8) primitive->regDomInfo.currentRegulatoryDomain);
    CsrMemCpySer(ptr, len, (const void *) primitive->regDomInfo.currentCountryCode, ((u16) (2)));
    return(ptr);
}


void* CsrWifiSmeRegulatoryDomainInfoGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeRegulatoryDomainInfoGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeRegulatoryDomainInfoGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->regDomInfo.dot11MultiDomainCapabilityImplemented, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->regDomInfo.dot11MultiDomainCapabilityEnabled, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->regDomInfo.currentRegulatoryDomain, buffer, &offset);
    CsrMemCpyDes(primitive->regDomInfo.currentCountryCode, buffer, &offset, ((u16) (2)));

    return primitive;
}


size_t CsrWifiSmeRoamCompleteIndSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmeRoamCompleteIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeRoamCompleteInd *primitive = (CsrWifiSmeRoamCompleteInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmeRoamCompleteIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeRoamCompleteInd *primitive = kmalloc(sizeof(CsrWifiSmeRoamCompleteInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeRoamStartIndSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 1; /* CsrWifiSmeRoamReason primitive->roamReason */
    bufferSize += 2; /* CsrWifiSmeIEEE80211Reason primitive->reason80211 */
    return bufferSize;
}


u8* CsrWifiSmeRoamStartIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeRoamStartInd *primitive = (CsrWifiSmeRoamStartInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint8Ser(ptr, len, (u8) primitive->roamReason);
    CsrUint16Ser(ptr, len, (u16) primitive->reason80211);
    return(ptr);
}


void* CsrWifiSmeRoamStartIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeRoamStartInd *primitive = kmalloc(sizeof(CsrWifiSmeRoamStartInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->roamReason, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->reason80211, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeRoamingConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 72) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].rssiHighThreshold */
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].rssiLowThreshold */
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].snrHighThreshold */
            bufferSize += 2; /* s16 primitive->roamingConfig.roamingBands[i2].snrLowThreshold */
        }
    }
    bufferSize += 1;         /* u8 primitive->roamingConfig.disableSmoothRoaming */
    bufferSize += 1;         /* u8 primitive->roamingConfig.disableRoamScans */
    bufferSize += 1;         /* u8 primitive->roamingConfig.reconnectLimit */
    bufferSize += 2;         /* u16 primitive->roamingConfig.reconnectLimitIntervalMs */
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].intervalSeconds */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].validitySeconds */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].minActiveChannelTimeTu */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].maxActiveChannelTimeTu */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].minPassiveChannelTimeTu */
            bufferSize += 2; /* u16 primitive->roamingConfig.roamScanCfg[i2].maxPassiveChannelTimeTu */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeRoamingConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeRoamingConfigGetCfm *primitive = (CsrWifiSmeRoamingConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].rssiHighThreshold);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].rssiLowThreshold);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].snrHighThreshold);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamingBands[i2].snrLowThreshold);
        }
    }
    CsrUint8Ser(ptr, len, (u8) primitive->roamingConfig.disableSmoothRoaming);
    CsrUint8Ser(ptr, len, (u8) primitive->roamingConfig.disableRoamScans);
    CsrUint8Ser(ptr, len, (u8) primitive->roamingConfig.reconnectLimit);
    CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.reconnectLimitIntervalMs);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].intervalSeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].validitySeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].minActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].maxActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].minPassiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->roamingConfig.roamScanCfg[i2].maxPassiveChannelTimeTu);
        }
    }
    return(ptr);
}


void* CsrWifiSmeRoamingConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeRoamingConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeRoamingConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].rssiHighThreshold, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].rssiLowThreshold, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].snrHighThreshold, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamingBands[i2].snrLowThreshold, buffer, &offset);
        }
    }
    CsrUint8Des((u8 *) &primitive->roamingConfig.disableSmoothRoaming, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->roamingConfig.disableRoamScans, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->roamingConfig.reconnectLimit, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->roamingConfig.reconnectLimitIntervalMs, buffer, &offset);
    {
        u16 i2;
        for (i2 = 0; i2 < 3; i2++)
        {
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].intervalSeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].validitySeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].minActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].maxActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].minPassiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->roamingConfig.roamScanCfg[i2].maxPassiveChannelTimeTu, buffer, &offset);
        }
    }

    return primitive;
}


size_t CsrWifiSmeRoamingConfigSetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmeRoamingConfigSetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeRoamingConfigSetCfm *primitive = (CsrWifiSmeRoamingConfigSetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmeRoamingConfigSetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeRoamingConfigSetCfm *primitive = kmalloc(sizeof(CsrWifiSmeRoamingConfigSetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeScanConfigGetCfmSizeof(void *msg)
{
    CsrWifiSmeScanConfigGetCfm *primitive = (CsrWifiSmeScanConfigGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 65) */
    bufferSize += 2; /* CsrResult primitive->status */
    {
        u16 i2;
        for (i2 = 0; i2 < 4; i2++)
        {
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].intervalSeconds */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].validitySeconds */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].minActiveChannelTimeTu */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].maxActiveChannelTimeTu */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].minPassiveChannelTimeTu */
            bufferSize += 2;                                     /* u16 primitive->scanConfig.scanCfg[i2].maxPassiveChannelTimeTu */
        }
    }
    bufferSize += 1;                                             /* u8 primitive->scanConfig.disableAutonomousScans */
    bufferSize += 2;                                             /* u16 primitive->scanConfig.maxResults */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.highRssiThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.lowRssiThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.deltaRssiThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.highSnrThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.lowSnrThreshold */
    bufferSize += 1;                                             /* s8 primitive->scanConfig.deltaSnrThreshold */
    bufferSize += 2;                                             /* u16 primitive->scanConfig.passiveChannelListCount */
    bufferSize += primitive->scanConfig.passiveChannelListCount; /* u8 primitive->scanConfig.passiveChannelList */
    return bufferSize;
}


u8* CsrWifiSmeScanConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeScanConfigGetCfm *primitive = (CsrWifiSmeScanConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    {
        u16 i2;
        for (i2 = 0; i2 < 4; i2++)
        {
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].intervalSeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].validitySeconds);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].minActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].maxActiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].minPassiveChannelTimeTu);
            CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.scanCfg[i2].maxPassiveChannelTimeTu);
        }
    }
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.disableAutonomousScans);
    CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.maxResults);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.highRssiThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.lowRssiThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.deltaRssiThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.highSnrThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.lowSnrThreshold);
    CsrUint8Ser(ptr, len, (u8) primitive->scanConfig.deltaSnrThreshold);
    CsrUint16Ser(ptr, len, (u16) primitive->scanConfig.passiveChannelListCount);
    if (primitive->scanConfig.passiveChannelListCount)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->scanConfig.passiveChannelList, ((u16) (primitive->scanConfig.passiveChannelListCount)));
    }
    return(ptr);
}


void* CsrWifiSmeScanConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeScanConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeScanConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    {
        u16 i2;
        for (i2 = 0; i2 < 4; i2++)
        {
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].intervalSeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].validitySeconds, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].minActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].maxActiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].minPassiveChannelTimeTu, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanConfig.scanCfg[i2].maxPassiveChannelTimeTu, buffer, &offset);
        }
    }
    CsrUint8Des((u8 *) &primitive->scanConfig.disableAutonomousScans, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->scanConfig.maxResults, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.highRssiThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.lowRssiThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.deltaRssiThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.highSnrThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.lowSnrThreshold, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->scanConfig.deltaSnrThreshold, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->scanConfig.passiveChannelListCount, buffer, &offset);
    if (primitive->scanConfig.passiveChannelListCount)
    {
        primitive->scanConfig.passiveChannelList = kmalloc(primitive->scanConfig.passiveChannelListCount, GFP_KERNEL);
        CsrMemCpyDes(primitive->scanConfig.passiveChannelList, buffer, &offset, ((u16) (primitive->scanConfig.passiveChannelListCount)));
    }
    else
    {
        primitive->scanConfig.passiveChannelList = NULL;
    }

    return primitive;
}


void CsrWifiSmeScanConfigGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeScanConfigGetCfm *primitive = (CsrWifiSmeScanConfigGetCfm *) voidPrimitivePointer;
    kfree(primitive->scanConfig.passiveChannelList);
    kfree(primitive);
}


size_t CsrWifiSmeScanResultIndSizeof(void *msg)
{
    CsrWifiSmeScanResultInd *primitive = (CsrWifiSmeScanResultInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 149) */
    bufferSize += 32;                                          /* u8 primitive->result.ssid.ssid[32] */
    bufferSize += 1;                                           /* u8 primitive->result.ssid.length */
    bufferSize += 6;                                           /* u8 primitive->result.bssid.a[6] */
    bufferSize += 2;                                           /* s16 primitive->result.rssi */
    bufferSize += 2;                                           /* s16 primitive->result.snr */
    bufferSize += 1;                                           /* CsrWifiSmeRadioIF primitive->result.ifIndex */
    bufferSize += 2;                                           /* u16 primitive->result.beaconPeriodTu */
    bufferSize += 8;                                           /* u8 primitive->result.timeStamp.data[8] */
    bufferSize += 8;                                           /* u8 primitive->result.localTime.data[8] */
    bufferSize += 2;                                           /* u16 primitive->result.channelFrequency */
    bufferSize += 2;                                           /* u16 primitive->result.capabilityInformation */
    bufferSize += 1;                                           /* u8 primitive->result.channelNumber */
    bufferSize += 1;                                           /* CsrWifiSmeBasicUsability primitive->result.usability */
    bufferSize += 1;                                           /* CsrWifiSmeBssType primitive->result.bssType */
    bufferSize += 2;                                           /* u16 primitive->result.informationElementsLength */
    bufferSize += primitive->result.informationElementsLength; /* u8 primitive->result.informationElements */
    bufferSize += 1;                                           /* CsrWifiSmeP2pRole primitive->result.p2pDeviceRole */
    switch (primitive->result.p2pDeviceRole)
    {
        case CSR_WIFI_SME_P2P_ROLE_CLI:
            bufferSize += 1; /* u8 primitive->result.deviceInfo.reservedCli.empty */
            break;
        case CSR_WIFI_SME_P2P_ROLE_GO:
            bufferSize += 1; /* CsrWifiSmeP2pGroupCapabilityMask primitive->result.deviceInfo.groupInfo.groupCapability */
            bufferSize += 6; /* u8 primitive->result.deviceInfo.groupInfo.p2pDeviceAddress.a[6] */
            bufferSize += 1; /* u8 primitive->result.deviceInfo.groupInfo.p2pClientInfoCount */
            {
                u16 i4;
                for (i4 = 0; i4 < primitive->result.deviceInfo.groupInfo.p2pClientInfoCount; i4++)
                {
                    bufferSize += 6; /* u8 primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].p2PClientInterfaceAddress.a[6] */
                    bufferSize += 6; /* u8 primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceAddress.a[6] */
                    bufferSize += 2; /* CsrWifiSmeWpsConfigTypeMask primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.configMethods */
                    bufferSize += 1; /* CsrWifiSmeP2pCapabilityMask primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.p2PDeviceCap */
                    bufferSize += 8; /* u8 primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.primDeviceType.deviceDetails[8] */
                    bufferSize += 1; /* u8 primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount */
                    {
                        u16 i6;
                        for (i6 = 0; i6 < primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount; i6++)
                        {
                            bufferSize += 8; /* u8 primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType[i6].deviceDetails[8] */
                        }
                    }
                    bufferSize += 32;        /* u8 primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceName[32] */
                    bufferSize += 1;         /* u8 primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceNameLength */
                }
            }
            break;
        case CSR_WIFI_SME_P2P_ROLE_NONE:
            bufferSize += 1; /* u8 primitive->result.deviceInfo.reservedNone.empty */
            break;
        case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
            bufferSize += 6; /* u8 primitive->result.deviceInfo.standalonedevInfo.deviceAddress.a[6] */
            bufferSize += 2; /* CsrWifiSmeWpsConfigTypeMask primitive->result.deviceInfo.standalonedevInfo.configMethods */
            bufferSize += 1; /* CsrWifiSmeP2pCapabilityMask primitive->result.deviceInfo.standalonedevInfo.p2PDeviceCap */
            bufferSize += 8; /* u8 primitive->result.deviceInfo.standalonedevInfo.primDeviceType.deviceDetails[8] */
            bufferSize += 1; /* u8 primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount */
            {
                u16 i4;
                for (i4 = 0; i4 < primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount; i4++)
                {
                    bufferSize += 8; /* u8 primitive->result.deviceInfo.standalonedevInfo.secDeviceType[i4].deviceDetails[8] */
                }
            }
            bufferSize += 32;        /* u8 primitive->result.deviceInfo.standalonedevInfo.deviceName[32] */
            bufferSize += 1;         /* u8 primitive->result.deviceInfo.standalonedevInfo.deviceNameLength */
            break;
        default:
            break;
    }
    return bufferSize;
}


u8* CsrWifiSmeScanResultIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeScanResultInd *primitive = (CsrWifiSmeScanResultInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrMemCpySer(ptr, len, (const void *) primitive->result.ssid.ssid, ((u16) (32)));
    CsrUint8Ser(ptr, len, (u8) primitive->result.ssid.length);
    CsrMemCpySer(ptr, len, (const void *) primitive->result.bssid.a, ((u16) (6)));
    CsrUint16Ser(ptr, len, (u16) primitive->result.rssi);
    CsrUint16Ser(ptr, len, (u16) primitive->result.snr);
    CsrUint8Ser(ptr, len, (u8) primitive->result.ifIndex);
    CsrUint16Ser(ptr, len, (u16) primitive->result.beaconPeriodTu);
    CsrMemCpySer(ptr, len, (const void *) primitive->result.timeStamp.data, ((u16) (8)));
    CsrMemCpySer(ptr, len, (const void *) primitive->result.localTime.data, ((u16) (8)));
    CsrUint16Ser(ptr, len, (u16) primitive->result.channelFrequency);
    CsrUint16Ser(ptr, len, (u16) primitive->result.capabilityInformation);
    CsrUint8Ser(ptr, len, (u8) primitive->result.channelNumber);
    CsrUint8Ser(ptr, len, (u8) primitive->result.usability);
    CsrUint8Ser(ptr, len, (u8) primitive->result.bssType);
    CsrUint16Ser(ptr, len, (u16) primitive->result.informationElementsLength);
    if (primitive->result.informationElementsLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->result.informationElements, ((u16) (primitive->result.informationElementsLength)));
    }
    CsrUint8Ser(ptr, len, (u8) primitive->result.p2pDeviceRole);
    switch (primitive->result.p2pDeviceRole)
    {
        case CSR_WIFI_SME_P2P_ROLE_CLI:
            CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.reservedCli.empty);
            break;
        case CSR_WIFI_SME_P2P_ROLE_GO:
            CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.groupInfo.groupCapability);
            CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.groupInfo.p2pDeviceAddress.a, ((u16) (6)));
            CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.groupInfo.p2pClientInfoCount);
            {
                u16 i4;
                for (i4 = 0; i4 < primitive->result.deviceInfo.groupInfo.p2pClientInfoCount; i4++)
                {
                    CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].p2PClientInterfaceAddress.a, ((u16) (6)));
                    CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceAddress.a, ((u16) (6)));
                    CsrUint16Ser(ptr, len, (u16) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.configMethods);
                    CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.p2PDeviceCap);
                    CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.primDeviceType.deviceDetails, ((u16) (8)));
                    CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount);
                    {
                        u16 i6;
                        for (i6 = 0; i6 < primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount; i6++)
                        {
                            CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType[i6].deviceDetails, ((u16) (8)));
                        }
                    }
                    CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceName, ((u16) (32)));
                    CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceNameLength);
                }
            }
            break;
        case CSR_WIFI_SME_P2P_ROLE_NONE:
            CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.reservedNone.empty);
            break;
        case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
            CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.standalonedevInfo.deviceAddress.a, ((u16) (6)));
            CsrUint16Ser(ptr, len, (u16) primitive->result.deviceInfo.standalonedevInfo.configMethods);
            CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.standalonedevInfo.p2PDeviceCap);
            CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.standalonedevInfo.primDeviceType.deviceDetails, ((u16) (8)));
            CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount);
            {
                u16 i4;
                for (i4 = 0; i4 < primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount; i4++)
                {
                    CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.standalonedevInfo.secDeviceType[i4].deviceDetails, ((u16) (8)));
                }
            }
            CsrMemCpySer(ptr, len, (const void *) primitive->result.deviceInfo.standalonedevInfo.deviceName, ((u16) (32)));
            CsrUint8Ser(ptr, len, (u8) primitive->result.deviceInfo.standalonedevInfo.deviceNameLength);
            break;
        default:
            break;
    }
    return(ptr);
}


void* CsrWifiSmeScanResultIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeScanResultInd *primitive = kmalloc(sizeof(CsrWifiSmeScanResultInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrMemCpyDes(primitive->result.ssid.ssid, buffer, &offset, ((u16) (32)));
    CsrUint8Des((u8 *) &primitive->result.ssid.length, buffer, &offset);
    CsrMemCpyDes(primitive->result.bssid.a, buffer, &offset, ((u16) (6)));
    CsrUint16Des((u16 *) &primitive->result.rssi, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->result.snr, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->result.ifIndex, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->result.beaconPeriodTu, buffer, &offset);
    CsrMemCpyDes(primitive->result.timeStamp.data, buffer, &offset, ((u16) (8)));
    CsrMemCpyDes(primitive->result.localTime.data, buffer, &offset, ((u16) (8)));
    CsrUint16Des((u16 *) &primitive->result.channelFrequency, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->result.capabilityInformation, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->result.channelNumber, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->result.usability, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->result.bssType, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->result.informationElementsLength, buffer, &offset);
    if (primitive->result.informationElementsLength)
    {
        primitive->result.informationElements = kmalloc(primitive->result.informationElementsLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->result.informationElements, buffer, &offset, ((u16) (primitive->result.informationElementsLength)));
    }
    else
    {
        primitive->result.informationElements = NULL;
    }
    CsrUint8Des((u8 *) &primitive->result.p2pDeviceRole, buffer, &offset);
    switch (primitive->result.p2pDeviceRole)
    {
        case CSR_WIFI_SME_P2P_ROLE_CLI:
            CsrUint8Des((u8 *) &primitive->result.deviceInfo.reservedCli.empty, buffer, &offset);
            break;
        case CSR_WIFI_SME_P2P_ROLE_GO:
            CsrUint8Des((u8 *) &primitive->result.deviceInfo.groupInfo.groupCapability, buffer, &offset);
            CsrMemCpyDes(primitive->result.deviceInfo.groupInfo.p2pDeviceAddress.a, buffer, &offset, ((u16) (6)));
            CsrUint8Des((u8 *) &primitive->result.deviceInfo.groupInfo.p2pClientInfoCount, buffer, &offset);
            primitive->result.deviceInfo.groupInfo.p2PClientInfo = NULL;
            if (primitive->result.deviceInfo.groupInfo.p2pClientInfoCount)
            {
                primitive->result.deviceInfo.groupInfo.p2PClientInfo = kmalloc(sizeof(CsrWifiSmeP2pClientInfoType) * primitive->result.deviceInfo.groupInfo.p2pClientInfoCount, GFP_KERNEL);
            }
            {
                u16 i4;
                for (i4 = 0; i4 < primitive->result.deviceInfo.groupInfo.p2pClientInfoCount; i4++)
                {
                    CsrMemCpyDes(primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].p2PClientInterfaceAddress.a, buffer, &offset, ((u16) (6)));
                    CsrMemCpyDes(primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceAddress.a, buffer, &offset, ((u16) (6)));
                    CsrUint16Des((u16 *) &primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.configMethods, buffer, &offset);
                    CsrUint8Des((u8 *) &primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.p2PDeviceCap, buffer, &offset);
                    CsrMemCpyDes(primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.primDeviceType.deviceDetails, buffer, &offset, ((u16) (8)));
                    CsrUint8Des((u8 *) &primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount, buffer, &offset);
                    primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType = NULL;
                    if (primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount)
                    {
                        primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType = kmalloc(sizeof(CsrWifiSmeWpsDeviceType) * primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount, GFP_KERNEL);
                    }
                    {
                        u16 i6;
                        for (i6 = 0; i6 < primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount; i6++)
                        {
                            CsrMemCpyDes(primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType[i6].deviceDetails, buffer, &offset, ((u16) (8)));
                        }
                    }
                    CsrMemCpyDes(primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceName, buffer, &offset, ((u16) (32)));
                    CsrUint8Des((u8 *) &primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceNameLength, buffer, &offset);
                }
            }
            break;
        case CSR_WIFI_SME_P2P_ROLE_NONE:
            CsrUint8Des((u8 *) &primitive->result.deviceInfo.reservedNone.empty, buffer, &offset);
            break;
        case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
            CsrMemCpyDes(primitive->result.deviceInfo.standalonedevInfo.deviceAddress.a, buffer, &offset, ((u16) (6)));
            CsrUint16Des((u16 *) &primitive->result.deviceInfo.standalonedevInfo.configMethods, buffer, &offset);
            CsrUint8Des((u8 *) &primitive->result.deviceInfo.standalonedevInfo.p2PDeviceCap, buffer, &offset);
            CsrMemCpyDes(primitive->result.deviceInfo.standalonedevInfo.primDeviceType.deviceDetails, buffer, &offset, ((u16) (8)));
            CsrUint8Des((u8 *) &primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount, buffer, &offset);
            primitive->result.deviceInfo.standalonedevInfo.secDeviceType = NULL;
            if (primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount)
            {
                primitive->result.deviceInfo.standalonedevInfo.secDeviceType = kmalloc(sizeof(CsrWifiSmeWpsDeviceType) * primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount, GFP_KERNEL);
            }
            {
                u16 i4;
                for (i4 = 0; i4 < primitive->result.deviceInfo.standalonedevInfo.secondaryDeviceTypeCount; i4++)
                {
                    CsrMemCpyDes(primitive->result.deviceInfo.standalonedevInfo.secDeviceType[i4].deviceDetails, buffer, &offset, ((u16) (8)));
                }
            }
            CsrMemCpyDes(primitive->result.deviceInfo.standalonedevInfo.deviceName, buffer, &offset, ((u16) (32)));
            CsrUint8Des((u8 *) &primitive->result.deviceInfo.standalonedevInfo.deviceNameLength, buffer, &offset);
            break;
        default:
            break;
    }

    return primitive;
}


void CsrWifiSmeScanResultIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeScanResultInd *primitive = (CsrWifiSmeScanResultInd *) voidPrimitivePointer;
    kfree(primitive->result.informationElements);
    switch (primitive->result.p2pDeviceRole)
    {
        case CSR_WIFI_SME_P2P_ROLE_GO:
        {
            u16 i4;
            for (i4 = 0; i4 < primitive->result.deviceInfo.groupInfo.p2pClientInfoCount; i4++)
            {
                kfree(primitive->result.deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType);
            }
        }
            kfree(primitive->result.deviceInfo.groupInfo.p2PClientInfo);
            break;
        case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
            kfree(primitive->result.deviceInfo.standalonedevInfo.secDeviceType);
            break;
        default:
            break;
    }
    kfree(primitive);
}


size_t CsrWifiSmeScanResultsGetCfmSizeof(void *msg)
{
    CsrWifiSmeScanResultsGetCfm *primitive = (CsrWifiSmeScanResultsGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 153) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 2; /* u16 primitive->scanResultsCount */
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->scanResultsCount; i1++)
        {
            bufferSize += 32;                                                   /* u8 primitive->scanResults[i1].ssid.ssid[32] */
            bufferSize += 1;                                                    /* u8 primitive->scanResults[i1].ssid.length */
            bufferSize += 6;                                                    /* u8 primitive->scanResults[i1].bssid.a[6] */
            bufferSize += 2;                                                    /* s16 primitive->scanResults[i1].rssi */
            bufferSize += 2;                                                    /* s16 primitive->scanResults[i1].snr */
            bufferSize += 1;                                                    /* CsrWifiSmeRadioIF primitive->scanResults[i1].ifIndex */
            bufferSize += 2;                                                    /* u16 primitive->scanResults[i1].beaconPeriodTu */
            bufferSize += 8;                                                    /* u8 primitive->scanResults[i1].timeStamp.data[8] */
            bufferSize += 8;                                                    /* u8 primitive->scanResults[i1].localTime.data[8] */
            bufferSize += 2;                                                    /* u16 primitive->scanResults[i1].channelFrequency */
            bufferSize += 2;                                                    /* u16 primitive->scanResults[i1].capabilityInformation */
            bufferSize += 1;                                                    /* u8 primitive->scanResults[i1].channelNumber */
            bufferSize += 1;                                                    /* CsrWifiSmeBasicUsability primitive->scanResults[i1].usability */
            bufferSize += 1;                                                    /* CsrWifiSmeBssType primitive->scanResults[i1].bssType */
            bufferSize += 2;                                                    /* u16 primitive->scanResults[i1].informationElementsLength */
            bufferSize += primitive->scanResults[i1].informationElementsLength; /* u8 primitive->scanResults[i1].informationElements */
            bufferSize += 1;                                                    /* CsrWifiSmeP2pRole primitive->scanResults[i1].p2pDeviceRole */
            switch (primitive->scanResults[i1].p2pDeviceRole)
            {
                case CSR_WIFI_SME_P2P_ROLE_CLI:
                    bufferSize += 1; /* u8 primitive->scanResults[i1].deviceInfo.reservedCli.empty */
                    break;
                case CSR_WIFI_SME_P2P_ROLE_GO:
                    bufferSize += 1; /* CsrWifiSmeP2pGroupCapabilityMask primitive->scanResults[i1].deviceInfo.groupInfo.groupCapability */
                    bufferSize += 6; /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2pDeviceAddress.a[6] */
                    bufferSize += 1; /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount */
                    {
                        u16 i4;
                        for (i4 = 0; i4 < primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount; i4++)
                        {
                            bufferSize += 6; /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].p2PClientInterfaceAddress.a[6] */
                            bufferSize += 6; /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceAddress.a[6] */
                            bufferSize += 2; /* CsrWifiSmeWpsConfigTypeMask primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.configMethods */
                            bufferSize += 1; /* CsrWifiSmeP2pCapabilityMask primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.p2PDeviceCap */
                            bufferSize += 8; /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.primDeviceType.deviceDetails[8] */
                            bufferSize += 1; /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount */
                            {
                                u16 i6;
                                for (i6 = 0; i6 < primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount; i6++)
                                {
                                    bufferSize += 8; /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType[i6].deviceDetails[8] */
                                }
                            }
                            bufferSize += 32;        /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceName[32] */
                            bufferSize += 1;         /* u8 primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceNameLength */
                        }
                    }
                    break;
                case CSR_WIFI_SME_P2P_ROLE_NONE:
                    bufferSize += 1; /* u8 primitive->scanResults[i1].deviceInfo.reservedNone.empty */
                    break;
                case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
                    bufferSize += 6; /* u8 primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceAddress.a[6] */
                    bufferSize += 2; /* CsrWifiSmeWpsConfigTypeMask primitive->scanResults[i1].deviceInfo.standalonedevInfo.configMethods */
                    bufferSize += 1; /* CsrWifiSmeP2pCapabilityMask primitive->scanResults[i1].deviceInfo.standalonedevInfo.p2PDeviceCap */
                    bufferSize += 8; /* u8 primitive->scanResults[i1].deviceInfo.standalonedevInfo.primDeviceType.deviceDetails[8] */
                    bufferSize += 1; /* u8 primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount */
                    {
                        u16 i4;
                        for (i4 = 0; i4 < primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount; i4++)
                        {
                            bufferSize += 8; /* u8 primitive->scanResults[i1].deviceInfo.standalonedevInfo.secDeviceType[i4].deviceDetails[8] */
                        }
                    }
                    bufferSize += 32;        /* u8 primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceName[32] */
                    bufferSize += 1;         /* u8 primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceNameLength */
                    break;
                default:
                    break;
            }
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeScanResultsGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeScanResultsGetCfm *primitive = (CsrWifiSmeScanResultsGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint16Ser(ptr, len, (u16) primitive->scanResultsCount);
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->scanResultsCount; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].ssid.ssid, ((u16) (32)));
            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].ssid.length);
            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].bssid.a, ((u16) (6)));
            CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].rssi);
            CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].snr);
            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].ifIndex);
            CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].beaconPeriodTu);
            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].timeStamp.data, ((u16) (8)));
            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].localTime.data, ((u16) (8)));
            CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].channelFrequency);
            CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].capabilityInformation);
            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].channelNumber);
            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].usability);
            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].bssType);
            CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].informationElementsLength);
            if (primitive->scanResults[i1].informationElementsLength)
            {
                CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].informationElements, ((u16) (primitive->scanResults[i1].informationElementsLength)));
            }
            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].p2pDeviceRole);
            switch (primitive->scanResults[i1].p2pDeviceRole)
            {
                case CSR_WIFI_SME_P2P_ROLE_CLI:
                    CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.reservedCli.empty);
                    break;
                case CSR_WIFI_SME_P2P_ROLE_GO:
                    CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.groupInfo.groupCapability);
                    CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.groupInfo.p2pDeviceAddress.a, ((u16) (6)));
                    CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount);
                    {
                        u16 i4;
                        for (i4 = 0; i4 < primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount; i4++)
                        {
                            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].p2PClientInterfaceAddress.a, ((u16) (6)));
                            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceAddress.a, ((u16) (6)));
                            CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.configMethods);
                            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.p2PDeviceCap);
                            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.primDeviceType.deviceDetails, ((u16) (8)));
                            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount);
                            {
                                u16 i6;
                                for (i6 = 0; i6 < primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount; i6++)
                                {
                                    CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType[i6].deviceDetails, ((u16) (8)));
                                }
                            }
                            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceName, ((u16) (32)));
                            CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceNameLength);
                        }
                    }
                    break;
                case CSR_WIFI_SME_P2P_ROLE_NONE:
                    CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.reservedNone.empty);
                    break;
                case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
                    CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceAddress.a, ((u16) (6)));
                    CsrUint16Ser(ptr, len, (u16) primitive->scanResults[i1].deviceInfo.standalonedevInfo.configMethods);
                    CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.standalonedevInfo.p2PDeviceCap);
                    CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.standalonedevInfo.primDeviceType.deviceDetails, ((u16) (8)));
                    CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount);
                    {
                        u16 i4;
                        for (i4 = 0; i4 < primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount; i4++)
                        {
                            CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.standalonedevInfo.secDeviceType[i4].deviceDetails, ((u16) (8)));
                        }
                    }
                    CsrMemCpySer(ptr, len, (const void *) primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceName, ((u16) (32)));
                    CsrUint8Ser(ptr, len, (u8) primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceNameLength);
                    break;
                default:
                    break;
            }
        }
    }
    return(ptr);
}


void* CsrWifiSmeScanResultsGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeScanResultsGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeScanResultsGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->scanResultsCount, buffer, &offset);
    primitive->scanResults = NULL;
    if (primitive->scanResultsCount)
    {
        primitive->scanResults = kmalloc(sizeof(CsrWifiSmeScanResult) * primitive->scanResultsCount, GFP_KERNEL);
    }
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->scanResultsCount; i1++)
        {
            CsrMemCpyDes(primitive->scanResults[i1].ssid.ssid, buffer, &offset, ((u16) (32)));
            CsrUint8Des((u8 *) &primitive->scanResults[i1].ssid.length, buffer, &offset);
            CsrMemCpyDes(primitive->scanResults[i1].bssid.a, buffer, &offset, ((u16) (6)));
            CsrUint16Des((u16 *) &primitive->scanResults[i1].rssi, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanResults[i1].snr, buffer, &offset);
            CsrUint8Des((u8 *) &primitive->scanResults[i1].ifIndex, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanResults[i1].beaconPeriodTu, buffer, &offset);
            CsrMemCpyDes(primitive->scanResults[i1].timeStamp.data, buffer, &offset, ((u16) (8)));
            CsrMemCpyDes(primitive->scanResults[i1].localTime.data, buffer, &offset, ((u16) (8)));
            CsrUint16Des((u16 *) &primitive->scanResults[i1].channelFrequency, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanResults[i1].capabilityInformation, buffer, &offset);
            CsrUint8Des((u8 *) &primitive->scanResults[i1].channelNumber, buffer, &offset);
            CsrUint8Des((u8 *) &primitive->scanResults[i1].usability, buffer, &offset);
            CsrUint8Des((u8 *) &primitive->scanResults[i1].bssType, buffer, &offset);
            CsrUint16Des((u16 *) &primitive->scanResults[i1].informationElementsLength, buffer, &offset);
            if (primitive->scanResults[i1].informationElementsLength)
            {
                primitive->scanResults[i1].informationElements = kmalloc(primitive->scanResults[i1].informationElementsLength, GFP_KERNEL);
                CsrMemCpyDes(primitive->scanResults[i1].informationElements, buffer, &offset, ((u16) (primitive->scanResults[i1].informationElementsLength)));
            }
            else
            {
                primitive->scanResults[i1].informationElements = NULL;
            }
            CsrUint8Des((u8 *) &primitive->scanResults[i1].p2pDeviceRole, buffer, &offset);
            switch (primitive->scanResults[i1].p2pDeviceRole)
            {
                case CSR_WIFI_SME_P2P_ROLE_CLI:
                    CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.reservedCli.empty, buffer, &offset);
                    break;
                case CSR_WIFI_SME_P2P_ROLE_GO:
                    CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.groupInfo.groupCapability, buffer, &offset);
                    CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.groupInfo.p2pDeviceAddress.a, buffer, &offset, ((u16) (6)));
                    CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount, buffer, &offset);
                    primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo = NULL;
                    if (primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount)
                    {
                        primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo = kmalloc(sizeof(CsrWifiSmeP2pClientInfoType) * primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount, GFP_KERNEL);
                    }
                    {
                        u16 i4;
                        for (i4 = 0; i4 < primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount; i4++)
                        {
                            CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].p2PClientInterfaceAddress.a, buffer, &offset, ((u16) (6)));
                            CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceAddress.a, buffer, &offset, ((u16) (6)));
                            CsrUint16Des((u16 *) &primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.configMethods, buffer, &offset);
                            CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.p2PDeviceCap, buffer, &offset);
                            CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.primDeviceType.deviceDetails, buffer, &offset, ((u16) (8)));
                            CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount, buffer, &offset);
                            primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType = NULL;
                            if (primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount)
                            {
                                primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType = kmalloc(sizeof(CsrWifiSmeWpsDeviceType) * primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount, GFP_KERNEL);
                            }
                            {
                                u16 i6;
                                for (i6 = 0; i6 < primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secondaryDeviceTypeCount; i6++)
                                {
                                    CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType[i6].deviceDetails, buffer, &offset, ((u16) (8)));
                                }
                            }
                            CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceName, buffer, &offset, ((u16) (32)));
                            CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.deviceNameLength, buffer, &offset);
                        }
                    }
                    break;
                case CSR_WIFI_SME_P2P_ROLE_NONE:
                    CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.reservedNone.empty, buffer, &offset);
                    break;
                case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
                    CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceAddress.a, buffer, &offset, ((u16) (6)));
                    CsrUint16Des((u16 *) &primitive->scanResults[i1].deviceInfo.standalonedevInfo.configMethods, buffer, &offset);
                    CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.standalonedevInfo.p2PDeviceCap, buffer, &offset);
                    CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.standalonedevInfo.primDeviceType.deviceDetails, buffer, &offset, ((u16) (8)));
                    CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount, buffer, &offset);
                    primitive->scanResults[i1].deviceInfo.standalonedevInfo.secDeviceType = NULL;
                    if (primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount)
                    {
                        primitive->scanResults[i1].deviceInfo.standalonedevInfo.secDeviceType = kmalloc(sizeof(CsrWifiSmeWpsDeviceType) * primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount, GFP_KERNEL);
                    }
                    {
                        u16 i4;
                        for (i4 = 0; i4 < primitive->scanResults[i1].deviceInfo.standalonedevInfo.secondaryDeviceTypeCount; i4++)
                        {
                            CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.standalonedevInfo.secDeviceType[i4].deviceDetails, buffer, &offset, ((u16) (8)));
                        }
                    }
                    CsrMemCpyDes(primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceName, buffer, &offset, ((u16) (32)));
                    CsrUint8Des((u8 *) &primitive->scanResults[i1].deviceInfo.standalonedevInfo.deviceNameLength, buffer, &offset);
                    break;
                default:
                    break;
            }
        }
    }

    return primitive;
}


void CsrWifiSmeScanResultsGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeScanResultsGetCfm *primitive = (CsrWifiSmeScanResultsGetCfm *) voidPrimitivePointer;
    {
        u16 i1;
        for (i1 = 0; i1 < primitive->scanResultsCount; i1++)
        {
            kfree(primitive->scanResults[i1].informationElements);
            switch (primitive->scanResults[i1].p2pDeviceRole)
            {
                case CSR_WIFI_SME_P2P_ROLE_GO:
                {
                    u16 i4;
                    for (i4 = 0; i4 < primitive->scanResults[i1].deviceInfo.groupInfo.p2pClientInfoCount; i4++)
                    {
                        kfree(primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo[i4].clientDeviceInfo.secDeviceType);
                    }
                }
                    kfree(primitive->scanResults[i1].deviceInfo.groupInfo.p2PClientInfo);
                    break;
                case CSR_WIFI_SME_P2P_ROLE_STANDALONE:
                    kfree(primitive->scanResults[i1].deviceInfo.standalonedevInfo.secDeviceType);
                    break;
                default:
                    break;
            }
        }
    }
    kfree(primitive->scanResults);
    kfree(primitive);
}


size_t CsrWifiSmeSmeStaConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->smeConfig.connectionQualityRssiChangeTrigger */
    bufferSize += 1; /* u8 primitive->smeConfig.connectionQualitySnrChangeTrigger */
    bufferSize += 1; /* CsrWifiSmeWmmModeMask primitive->smeConfig.wmmModeMask */
    bufferSize += 1; /* CsrWifiSmeRadioIF primitive->smeConfig.ifIndex */
    bufferSize += 1; /* u8 primitive->smeConfig.allowUnicastUseGroupCipher */
    bufferSize += 1; /* u8 primitive->smeConfig.enableOpportunisticKeyCaching */
    return bufferSize;
}


u8* CsrWifiSmeSmeStaConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeSmeStaConfigGetCfm *primitive = (CsrWifiSmeSmeStaConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.connectionQualityRssiChangeTrigger);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.connectionQualitySnrChangeTrigger);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.wmmModeMask);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.ifIndex);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.allowUnicastUseGroupCipher);
    CsrUint8Ser(ptr, len, (u8) primitive->smeConfig.enableOpportunisticKeyCaching);
    return(ptr);
}


void* CsrWifiSmeSmeStaConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeSmeStaConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeSmeStaConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.connectionQualityRssiChangeTrigger, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.connectionQualitySnrChangeTrigger, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.wmmModeMask, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.ifIndex, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.allowUnicastUseGroupCipher, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->smeConfig.enableOpportunisticKeyCaching, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeSmeStaConfigSetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 7) */
    bufferSize += 2; /* u16 primitive->interfaceTag */
    bufferSize += 2; /* CsrResult primitive->status */
    return bufferSize;
}


u8* CsrWifiSmeSmeStaConfigSetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeSmeStaConfigSetCfm *primitive = (CsrWifiSmeSmeStaConfigSetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    return(ptr);
}


void* CsrWifiSmeSmeStaConfigSetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeSmeStaConfigSetCfm *primitive = kmalloc(sizeof(CsrWifiSmeSmeStaConfigSetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeStationMacAddressGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 17) */
    bufferSize += 2; /* CsrResult primitive->status */
    {
        u16 i1;
        for (i1 = 0; i1 < 2; i1++)
        {
            bufferSize += 6; /* u8 primitive->stationMacAddress[i1].a[6] */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeStationMacAddressGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeStationMacAddressGetCfm *primitive = (CsrWifiSmeStationMacAddressGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    {
        u16 i1;
        for (i1 = 0; i1 < 2; i1++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->stationMacAddress[i1].a, ((u16) (6)));
        }
    }
    return(ptr);
}


void* CsrWifiSmeStationMacAddressGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeStationMacAddressGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeStationMacAddressGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    {
        u16 i1;
        for (i1 = 0; i1 < 2; i1++)
        {
            CsrMemCpyDes(primitive->stationMacAddress[i1].a, buffer, &offset, ((u16) (6)));
        }
    }

    return primitive;
}


size_t CsrWifiSmeTspecIndSizeof(void *msg)
{
    CsrWifiSmeTspecInd *primitive = (CsrWifiSmeTspecInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 13) */
    bufferSize += 2;                      /* u16 primitive->interfaceTag */
    bufferSize += 4;                      /* u32 primitive->transactionId */
    bufferSize += 1;                      /* CsrWifiSmeTspecResultCode primitive->tspecResultCode */
    bufferSize += 2;                      /* u16 primitive->tspecLength */
    bufferSize += primitive->tspecLength; /* u8 primitive->tspec */
    return bufferSize;
}


u8* CsrWifiSmeTspecIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeTspecInd *primitive = (CsrWifiSmeTspecInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint32Ser(ptr, len, (u32) primitive->transactionId);
    CsrUint8Ser(ptr, len, (u8) primitive->tspecResultCode);
    CsrUint16Ser(ptr, len, (u16) primitive->tspecLength);
    if (primitive->tspecLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->tspec, ((u16) (primitive->tspecLength)));
    }
    return(ptr);
}


void* CsrWifiSmeTspecIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeTspecInd *primitive = kmalloc(sizeof(CsrWifiSmeTspecInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->transactionId, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->tspecResultCode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->tspecLength, buffer, &offset);
    if (primitive->tspecLength)
    {
        primitive->tspec = kmalloc(primitive->tspecLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->tspec, buffer, &offset, ((u16) (primitive->tspecLength)));
    }
    else
    {
        primitive->tspec = NULL;
    }

    return primitive;
}


void CsrWifiSmeTspecIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeTspecInd *primitive = (CsrWifiSmeTspecInd *) voidPrimitivePointer;
    kfree(primitive->tspec);
    kfree(primitive);
}


size_t CsrWifiSmeTspecCfmSizeof(void *msg)
{
    CsrWifiSmeTspecCfm *primitive = (CsrWifiSmeTspecCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 15) */
    bufferSize += 2;                      /* u16 primitive->interfaceTag */
    bufferSize += 2;                      /* CsrResult primitive->status */
    bufferSize += 4;                      /* u32 primitive->transactionId */
    bufferSize += 1;                      /* CsrWifiSmeTspecResultCode primitive->tspecResultCode */
    bufferSize += 2;                      /* u16 primitive->tspecLength */
    bufferSize += primitive->tspecLength; /* u8 primitive->tspec */
    return bufferSize;
}


u8* CsrWifiSmeTspecCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeTspecCfm *primitive = (CsrWifiSmeTspecCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->interfaceTag);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint32Ser(ptr, len, (u32) primitive->transactionId);
    CsrUint8Ser(ptr, len, (u8) primitive->tspecResultCode);
    CsrUint16Ser(ptr, len, (u16) primitive->tspecLength);
    if (primitive->tspecLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->tspec, ((u16) (primitive->tspecLength)));
    }
    return(ptr);
}


void* CsrWifiSmeTspecCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeTspecCfm *primitive = kmalloc(sizeof(CsrWifiSmeTspecCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->interfaceTag, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->transactionId, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->tspecResultCode, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->tspecLength, buffer, &offset);
    if (primitive->tspecLength)
    {
        primitive->tspec = kmalloc(primitive->tspecLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->tspec, buffer, &offset, ((u16) (primitive->tspecLength)));
    }
    else
    {
        primitive->tspec = NULL;
    }

    return primitive;
}


void CsrWifiSmeTspecCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeTspecCfm *primitive = (CsrWifiSmeTspecCfm *) voidPrimitivePointer;
    kfree(primitive->tspec);
    kfree(primitive);
}


size_t CsrWifiSmeVersionsGetCfmSizeof(void *msg)
{
    CsrWifiSmeVersionsGetCfm *primitive = (CsrWifiSmeVersionsGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 33) */
    bufferSize += 2;                                                                                    /* CsrResult primitive->status */
    bufferSize += 4;                                                                                    /* u32 primitive->versions.chipId */
    bufferSize += 4;                                                                                    /* u32 primitive->versions.chipVersion */
    bufferSize += 4;                                                                                    /* u32 primitive->versions.firmwareBuild */
    bufferSize += 4;                                                                                    /* u32 primitive->versions.firmwarePatch */
    bufferSize += 4;                                                                                    /* u32 primitive->versions.firmwareHip */
    bufferSize += (primitive->versions.routerBuild ? strlen(primitive->versions.routerBuild) : 0) + 1;  /* char* primitive->versions.routerBuild (0 byte len + 1 for NULL Term) */
    bufferSize += 4;                                                                                    /* u32 primitive->versions.routerHip */
    bufferSize += (primitive->versions.smeBuild ? strlen(primitive->versions.smeBuild) : 0) + 1;        /* char* primitive->versions.smeBuild (0 byte len + 1 for NULL Term) */
    bufferSize += 4;                                                                                    /* u32 primitive->versions.smeHip */
    return bufferSize;
}


u8* CsrWifiSmeVersionsGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeVersionsGetCfm *primitive = (CsrWifiSmeVersionsGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint32Ser(ptr, len, (u32) primitive->versions.chipId);
    CsrUint32Ser(ptr, len, (u32) primitive->versions.chipVersion);
    CsrUint32Ser(ptr, len, (u32) primitive->versions.firmwareBuild);
    CsrUint32Ser(ptr, len, (u32) primitive->versions.firmwarePatch);
    CsrUint32Ser(ptr, len, (u32) primitive->versions.firmwareHip);
    CsrCharStringSer(ptr, len, primitive->versions.routerBuild);
    CsrUint32Ser(ptr, len, (u32) primitive->versions.routerHip);
    CsrCharStringSer(ptr, len, primitive->versions.smeBuild);
    CsrUint32Ser(ptr, len, (u32) primitive->versions.smeHip);
    return(ptr);
}


void* CsrWifiSmeVersionsGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeVersionsGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeVersionsGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->versions.chipId, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->versions.chipVersion, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->versions.firmwareBuild, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->versions.firmwarePatch, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->versions.firmwareHip, buffer, &offset);
    CsrCharStringDes(&primitive->versions.routerBuild, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->versions.routerHip, buffer, &offset);
    CsrCharStringDes(&primitive->versions.smeBuild, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->versions.smeHip, buffer, &offset);

    return primitive;
}


void CsrWifiSmeVersionsGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeVersionsGetCfm *primitive = (CsrWifiSmeVersionsGetCfm *) voidPrimitivePointer;
    kfree(primitive->versions.routerBuild);
    kfree(primitive->versions.smeBuild);
    kfree(primitive);
}


size_t CsrWifiSmeCloakedSsidsGetCfmSizeof(void *msg)
{
    CsrWifiSmeCloakedSsidsGetCfm *primitive = (CsrWifiSmeCloakedSsidsGetCfm *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 39) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* u8 primitive->cloakedSsids.cloakedSsidsCount */
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->cloakedSsids.cloakedSsidsCount; i2++)
        {
            bufferSize += 32; /* u8 primitive->cloakedSsids.cloakedSsids[i2].ssid[32] */
            bufferSize += 1;  /* u8 primitive->cloakedSsids.cloakedSsids[i2].length */
        }
    }
    return bufferSize;
}


u8* CsrWifiSmeCloakedSsidsGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCloakedSsidsGetCfm *primitive = (CsrWifiSmeCloakedSsidsGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->cloakedSsids.cloakedSsidsCount);
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->cloakedSsids.cloakedSsidsCount; i2++)
        {
            CsrMemCpySer(ptr, len, (const void *) primitive->cloakedSsids.cloakedSsids[i2].ssid, ((u16) (32)));
            CsrUint8Ser(ptr, len, (u8) primitive->cloakedSsids.cloakedSsids[i2].length);
        }
    }
    return(ptr);
}


void* CsrWifiSmeCloakedSsidsGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCloakedSsidsGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeCloakedSsidsGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->cloakedSsids.cloakedSsidsCount, buffer, &offset);
    primitive->cloakedSsids.cloakedSsids = NULL;
    if (primitive->cloakedSsids.cloakedSsidsCount)
    {
        primitive->cloakedSsids.cloakedSsids = kmalloc(sizeof(CsrWifiSsid) * primitive->cloakedSsids.cloakedSsidsCount, GFP_KERNEL);
    }
    {
        u16 i2;
        for (i2 = 0; i2 < primitive->cloakedSsids.cloakedSsidsCount; i2++)
        {
            CsrMemCpyDes(primitive->cloakedSsids.cloakedSsids[i2].ssid, buffer, &offset, ((u16) (32)));
            CsrUint8Des((u8 *) &primitive->cloakedSsids.cloakedSsids[i2].length, buffer, &offset);
        }
    }

    return primitive;
}


void CsrWifiSmeCloakedSsidsGetCfmSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeCloakedSsidsGetCfm *primitive = (CsrWifiSmeCloakedSsidsGetCfm *) voidPrimitivePointer;
    kfree(primitive->cloakedSsids.cloakedSsids);
    kfree(primitive);
}


size_t CsrWifiSmeWifiOnIndSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 9) */
    bufferSize += 6; /* u8 primitive->address.a[6] */
    return bufferSize;
}


u8* CsrWifiSmeWifiOnIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeWifiOnInd *primitive = (CsrWifiSmeWifiOnInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrMemCpySer(ptr, len, (const void *) primitive->address.a, ((u16) (6)));
    return(ptr);
}


void* CsrWifiSmeWifiOnIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeWifiOnInd *primitive = kmalloc(sizeof(CsrWifiSmeWifiOnInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrMemCpyDes(primitive->address.a, buffer, &offset, ((u16) (6)));

    return primitive;
}


size_t CsrWifiSmeSmeCommonConfigGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 10) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 1; /* CsrWifiSme80211dTrustLevel primitive->deviceConfig.trustLevel */
    bufferSize += 2; /* u8 primitive->deviceConfig.countryCode[2] */
    bufferSize += 1; /* CsrWifiSmeFirmwareDriverInterface primitive->deviceConfig.firmwareDriverInterface */
    bufferSize += 1; /* u8 primitive->deviceConfig.enableStrictDraftN */
    return bufferSize;
}


u8* CsrWifiSmeSmeCommonConfigGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeSmeCommonConfigGetCfm *primitive = (CsrWifiSmeSmeCommonConfigGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint8Ser(ptr, len, (u8) primitive->deviceConfig.trustLevel);
    CsrMemCpySer(ptr, len, (const void *) primitive->deviceConfig.countryCode, ((u16) (2)));
    CsrUint8Ser(ptr, len, (u8) primitive->deviceConfig.firmwareDriverInterface);
    CsrUint8Ser(ptr, len, (u8) primitive->deviceConfig.enableStrictDraftN);
    return(ptr);
}


void* CsrWifiSmeSmeCommonConfigGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeSmeCommonConfigGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeSmeCommonConfigGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->deviceConfig.trustLevel, buffer, &offset);
    CsrMemCpyDes(primitive->deviceConfig.countryCode, buffer, &offset, ((u16) (2)));
    CsrUint8Des((u8 *) &primitive->deviceConfig.firmwareDriverInterface, buffer, &offset);
    CsrUint8Des((u8 *) &primitive->deviceConfig.enableStrictDraftN, buffer, &offset);

    return primitive;
}


size_t CsrWifiSmeInterfaceCapabilityGetCfmSizeof(void *msg)
{
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 9) */
    bufferSize += 2; /* CsrResult primitive->status */
    bufferSize += 2; /* u16 primitive->numInterfaces */
    bufferSize += 2; /* u8 primitive->capBitmap[2] */
    return bufferSize;
}


u8* CsrWifiSmeInterfaceCapabilityGetCfmSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeInterfaceCapabilityGetCfm *primitive = (CsrWifiSmeInterfaceCapabilityGetCfm *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint16Ser(ptr, len, (u16) primitive->status);
    CsrUint16Ser(ptr, len, (u16) primitive->numInterfaces);
    CsrMemCpySer(ptr, len, (const void *) primitive->capBitmap, ((u16) (2)));
    return(ptr);
}


void* CsrWifiSmeInterfaceCapabilityGetCfmDes(u8 *buffer, size_t length)
{
    CsrWifiSmeInterfaceCapabilityGetCfm *primitive = kmalloc(sizeof(CsrWifiSmeInterfaceCapabilityGetCfm), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->status, buffer, &offset);
    CsrUint16Des((u16 *) &primitive->numInterfaces, buffer, &offset);
    CsrMemCpyDes(primitive->capBitmap, buffer, &offset, ((u16) (2)));

    return primitive;
}


size_t CsrWifiSmeErrorIndSizeof(void *msg)
{
    CsrWifiSmeErrorInd *primitive = (CsrWifiSmeErrorInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 3) */
    bufferSize += (primitive->errorMessage ? strlen(primitive->errorMessage) : 0) + 1; /* char* primitive->errorMessage (0 byte len + 1 for NULL Term) */
    return bufferSize;
}


u8* CsrWifiSmeErrorIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeErrorInd *primitive = (CsrWifiSmeErrorInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrCharStringSer(ptr, len, primitive->errorMessage);
    return(ptr);
}


void* CsrWifiSmeErrorIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeErrorInd *primitive = kmalloc(sizeof(CsrWifiSmeErrorInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrCharStringDes(&primitive->errorMessage, buffer, &offset);

    return primitive;
}


void CsrWifiSmeErrorIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeErrorInd *primitive = (CsrWifiSmeErrorInd *) voidPrimitivePointer;
    kfree(primitive->errorMessage);
    kfree(primitive);
}


size_t CsrWifiSmeInfoIndSizeof(void *msg)
{
    CsrWifiSmeInfoInd *primitive = (CsrWifiSmeInfoInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 3) */
    bufferSize += (primitive->infoMessage ? strlen(primitive->infoMessage) : 0) + 1; /* char* primitive->infoMessage (0 byte len + 1 for NULL Term) */
    return bufferSize;
}


u8* CsrWifiSmeInfoIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeInfoInd *primitive = (CsrWifiSmeInfoInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrCharStringSer(ptr, len, primitive->infoMessage);
    return(ptr);
}


void* CsrWifiSmeInfoIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeInfoInd *primitive = kmalloc(sizeof(CsrWifiSmeInfoInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrCharStringDes(&primitive->infoMessage, buffer, &offset);

    return primitive;
}


void CsrWifiSmeInfoIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeInfoInd *primitive = (CsrWifiSmeInfoInd *) voidPrimitivePointer;
    kfree(primitive->infoMessage);
    kfree(primitive);
}


size_t CsrWifiSmeCoreDumpIndSizeof(void *msg)
{
    CsrWifiSmeCoreDumpInd *primitive = (CsrWifiSmeCoreDumpInd *) msg;
    size_t bufferSize = 2;

    /* Calculate the Size of the Serialised Data. Could be more efficient (Try 8) */
    bufferSize += 4;                     /* u32 primitive->dataLength */
    bufferSize += primitive->dataLength; /* u8 primitive->data */
    return bufferSize;
}


u8* CsrWifiSmeCoreDumpIndSer(u8 *ptr, size_t *len, void *msg)
{
    CsrWifiSmeCoreDumpInd *primitive = (CsrWifiSmeCoreDumpInd *)msg;
    *len = 0;
    CsrUint16Ser(ptr, len, primitive->common.type);
    CsrUint32Ser(ptr, len, (u32) primitive->dataLength);
    if (primitive->dataLength)
    {
        CsrMemCpySer(ptr, len, (const void *) primitive->data, ((u16) (primitive->dataLength)));
    }
    return(ptr);
}


void* CsrWifiSmeCoreDumpIndDes(u8 *buffer, size_t length)
{
    CsrWifiSmeCoreDumpInd *primitive = kmalloc(sizeof(CsrWifiSmeCoreDumpInd), GFP_KERNEL);
    size_t offset;
    offset = 0;

    CsrUint16Des(&primitive->common.type, buffer, &offset);
    CsrUint32Des((u32 *) &primitive->dataLength, buffer, &offset);
    if (primitive->dataLength)
    {
        primitive->data = kmalloc(primitive->dataLength, GFP_KERNEL);
        CsrMemCpyDes(primitive->data, buffer, &offset, ((u16) (primitive->dataLength)));
    }
    else
    {
        primitive->data = NULL;
    }

    return primitive;
}


void CsrWifiSmeCoreDumpIndSerFree(void *voidPrimitivePointer)
{
    CsrWifiSmeCoreDumpInd *primitive = (CsrWifiSmeCoreDumpInd *) voidPrimitivePointer;
    kfree(primitive->data);
    kfree(primitive);
}


