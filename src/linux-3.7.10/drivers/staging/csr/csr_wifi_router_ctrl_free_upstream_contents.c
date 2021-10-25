/*****************************************************************************

            (c) Cambridge Silicon Radio Limited 2012
            All rights reserved and confidential information of CSR

            Refer to LICENSE.txt included with this source for details
            on the license terms.

*****************************************************************************/

/* Note: this is an auto-generated file. */
#include <linux/slab.h>
#include "csr_wifi_router_ctrl_prim.h"
#include "csr_wifi_router_ctrl_lib.h"

/*----------------------------------------------------------------------------*
 *  NAME
 *      CsrWifiRouterCtrlFreeUpstreamMessageContents
 *
 *  DESCRIPTION
 *
 *
 *  PARAMETERS
 *      eventClass: only the value CSR_WIFI_ROUTER_CTRL_PRIM will be handled
 *      message:    the message to free
 *----------------------------------------------------------------------------*/
void CsrWifiRouterCtrlFreeUpstreamMessageContents(u16 eventClass, void *message)
{
    if (eventClass != CSR_WIFI_ROUTER_CTRL_PRIM)
    {
        return;
    }
    if (NULL == message)
    {
        return;
    }

    switch (*((CsrWifiRouterCtrlPrim *) message))
    {
        case CSR_WIFI_ROUTER_CTRL_HIP_IND:
        {
            CsrWifiRouterCtrlHipInd *p = (CsrWifiRouterCtrlHipInd *)message;
            kfree(p->mlmeCommand);
            p->mlmeCommand = NULL;
            kfree(p->dataRef1);
            p->dataRef1 = NULL;
            kfree(p->dataRef2);
            p->dataRef2 = NULL;
            break;
        }
        case CSR_WIFI_ROUTER_CTRL_MULTICAST_ADDRESS_IND:
        {
            CsrWifiRouterCtrlMulticastAddressInd *p = (CsrWifiRouterCtrlMulticastAddressInd *)message;
            kfree(p->setAddresses);
            p->setAddresses = NULL;
            break;
        }
        case CSR_WIFI_ROUTER_CTRL_WIFI_ON_IND:
        {
            CsrWifiRouterCtrlWifiOnInd *p = (CsrWifiRouterCtrlWifiOnInd *)message;
            kfree(p->versions.routerBuild);
            p->versions.routerBuild = NULL;
            break;
        }
        case CSR_WIFI_ROUTER_CTRL_WAPI_RX_MIC_CHECK_IND:
        {
            CsrWifiRouterCtrlWapiRxMicCheckInd *p = (CsrWifiRouterCtrlWapiRxMicCheckInd *)message;
            kfree(p->signal);
            p->signal = NULL;
            kfree(p->data);
            p->data = NULL;
            break;
        }
        case CSR_WIFI_ROUTER_CTRL_WAPI_UNICAST_TX_ENCRYPT_IND:
        {
            CsrWifiRouterCtrlWapiUnicastTxEncryptInd *p = (CsrWifiRouterCtrlWapiUnicastTxEncryptInd *)message;
            kfree(p->data);
            p->data = NULL;
            break;
        }

        default:
            break;
    }
}


