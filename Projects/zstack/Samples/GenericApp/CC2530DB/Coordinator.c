#include "OSAL.h"
#include "AF.h"
#include "ZDApp.h"
#include "ZDObject.h"
#include "ZDProfile.h"
#include <string.h>
#include "Coordinator.h"
#include "DebugTrace.h"
#if !defined( WIN32 )
#include "OnBoard.h"
#endif
#include "hal_lcd.h"
#include "hal_led.h"
#include "hal_key.h"
#include "hal_uart.h"
//ʵ�ֵ�Ե�ͨ��  ����
const cId_t GenericApp_ClusterList[GENERICAPP_MAX_CLUSTERS] =
{
    GENERICAPP_CLUSTERID
};// GENERICAPP_MAX_CLUSTERS���� Coordinator.h �ļ��ж���ĺ�
const SimpleDescriptionFormat_t GenericApp_SimpleDesc =
{
    GENERICAPP_ENDPOINT,
    GENERICAPP_PROFID,
    GENERICAPP_DEVICEID,
    GENERICAPP_DEVICE_VERSION,
    GENERICAPP_FLAGS,
    GENERICAPP_MAX_CLUSTERS,
    (cId_t *)GenericApp_ClusterList,
    0,
    (cId_t *)NULL
};//��������ݽṹ������������һ�� ZigBee �豸�ڵ�
endPointDesc_t GenericApp_epDesc;  //�ڵ�������GenericApp_epDesc  ��ZigBeeЭ�����¶��������һ����"t"��β
byte GenericApp_TaskID;   //�������ȼ�GenericApp_TaskID
byte GenericApp_TransID;  //���ݷ������к�GenericApp_TransID
void GenericApp_MessageMSGCB ( afIncomingMSGPacket_t *pckt );//��Ϣ��������GenericApp_MessageMSGCB
void GenericApp_sendTheMessage ( void ) ;//���ݷ��ͺ���GenericApp_sendTheMessage
//�����ʼ������
void GenericApp_Init( byte task_id )
{
    GenericApp_TaskID              =task_id; //��ʼ�����������ȼ�,�������ȼ���Э��ջ�Ĳ���ϵͳOSAL����
    GenericApp_TransID             = 0;      //���������ݰ�����ų�ʼ��Ϊ 0���� ZigBee Э��ջ�У�ÿ����һ�����ݰ�,�÷�������Զ���1
    GenericApp_epDesc.endPoint     = GENERICAPP_ENDPOINT;//�Խڵ����������еĳ�ʼ��
    GenericApp_epDesc.task_id      = &GenericApp_TaskID;
    GenericApp_epDesc.simpleDesc   = 
        (SimpleDescriptionFormat_t *) &GenericApp_SimpleDesc;
    GenericApp_epDesc.latencyReq   = noLatencyReqs;
    afRegister( &GenericApp_epDesc );//ʹ�� afRegister �������ڵ�����������ע�ᣬֻ��ע���Ժ�,�ſ���ʹ��OSAL�ṩ��ϵͳ����
}
//��Ϣ��������
UINT16 GenericApp_ProcessEvent( byte task_id,UINT16 events )
{
    afIncomingMSGPacket_t *MSGpkt;//������һ��ָ�������Ϣ�ṹ���ָ�� MSGpkt
    if ( events & SYS_EVENT_MSG )
    {
        MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive (GenericApp_TaskID ) ;//ʹ��osal_msg_receive ��������Ϣ�����Ͻ�����Ϣ������Ϣ�а����˽��յ����������ݰ�
        while ( MSGpkt )
        {
            switch ( MSGpkt->hdr.event )
            {
            case AF_INCOMING_MSG_CMD://�Խ��յ�����Ϣ�����жϣ�����ǽ��յ����������ݣ��������һ�еĺ��������ݽ�����Ӧ�Ĵ�����
                GenericApp_MessageMSGCB ( MSGpkt ) ;//��ɶ����ݽ��յĴ���
                break;
            default:
                break;
            }
            osal_msg_deallocate( (uint8 *)MSGpkt );//���յ�����Ϣ�������,��Ҫ���� osal_msg_ deallocate ��������ռ�ݵĶ��ڴ��ͷţ�������������"�ڴ�й©"��
            MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive//������һ����Ϣ���ٴ���Ϣ�����������Ϣ��Ȼ����������Ӧ�Ĵ�����ֱ��������Ϣ��������Ϊֹ��
                ( GenericApp_TaskID ) ;
        }
        return (events ^ SYS_EVENT_MSG) ;
    }
    return 0;
}
void GenericApp_MessageMSGCB ( afIncomingMSGPacket_t *pkt )
{
    unsigned char buffer[4] = "    ";
    switch ( pkt->clusterId )
    {
    case GENERICAPP_CLUSTERID:
        osal_memcpy (buffer,pkt->cmd.Data,3);//���յ������ݿ����������� buffer ��
        if ((buffer[0] =='L') || (buffer[1] == 'E') || (buffer [2]=='D'))//�жϽ��յ��������ǲ���"LED"�����ַ�
        {
            HalLedBlink (HAL_LED_2,0,50,500);//���������ַ�����ִ�У�ʹLED2��˸(HalLedBlink������ʹĳ��LED��˸)
        }
        else
        {
            HalLedSet (HAL_LED_2,HAL_LED_MODE_ON);//������յ��Ĳ����������ַ������LED2(HalLedSet������;����ĳ��LED��״̬) 
        }
        break;
    }
}  
