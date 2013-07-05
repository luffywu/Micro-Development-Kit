// TestServer.cpp: implementation of the TestServer class.
//
//////////////////////////////////////////////////////////////////////

#include "TestServer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TestServer::TestServer()
{
	SetIOThreadCount(4);//��������IO�߳�����
	SetWorkThreadCount(4);//���ù����߳���
	Listen(8888);
	Listen(6666);
	Listen(9999);
//	Connect("127.0.0.1",8888);//��������δ���ԣ���������ô��
}

TestServer::~TestServer()
{

}

/*
 *	���������߳�
 */
void* TestServer::Main(void* pParam)
{
	while ( IsOk() )
	{
		//ִ��ҵ��
#ifndef WIN32
		usleep( 1000 * 1000 );
#else
		Sleep( 1000 );
#endif
	}
	
	return 0;
}

/**
 * �������¼��ص�����
 * 
 * ������ʵ�־�������ҵ����
 * 
 */
void TestServer::OnConnect(mdk::NetHost* pClient)
{
	printf( "new client(%d) connect in\n", pClient->ID() );
	//�������ӵ�2�ַ�ʽ
	//��ʽ1������NetHost����ָ��
	//pClient->Hold();//���ֶ��󣬸��߷��������棬ҵ����ڽ���ĳЩʱ����ܻ����pClient��
	//����send close�����������ͷ�pClient����ֱ��pClient->Free()������
	//��pClient���뵽���Լ����б��У��Դ�������Ҫ����ʱʹ��

	//��ʽ2������NetHost ID
	//��pClient->ID()���뵽���Լ����б��У��Դ�������Ҫsend��close����ʱ��
	//ֱ�ӽ�ID���ݸ�SendMsg() CloseConnect()�ӿڼ��ɣ�����ҪpClient->Hold()���ֶ���
	//��send close����ʱ�����pClient�Ѿ����ײ��ͷţ��ӿڻ᷵��ʧ��
}

/**
 * ���ӹر��¼����ص�����
 * 
 * ������ʵ�־���Ͽ�����ҵ����
 * 
 */
void TestServer::OnCloseConnect(mdk::NetHost* pClient)
{
	printf( "client(%d) close connect\n", pClient->ID() );
	//��pClient���Լ����б���ɾ��
	//pClient->Free();//���߷��������棬ҵ��㽫������Ҫʹ��pClient������Ҫ�ͷ�ʱ�����ͷ�
}

void TestServer::OnMsg(mdk::NetHost* pClient)
{
	//���豨�ĽṹΪ��2byte��ʾ���ݳ���+��������
	unsigned char c[256];
	/*
		��ȡ���ݳ��ȣ����Ȳ���2byteֱ�ӷ��أ��ȴ��´����ݵ���ʱ�ٶ�ȡ
		ֻ��ȡ2byte��false��ʾ��������ȡ�������ݴӻ�����ɾ�����´λ��ǿ��Զ���
	*/
	if ( !pClient->Recv( c, 2, false ) ) return;
	unsigned short len = 0;
	memcpy( &len, c, 2 );//�õ����ݳ���
	len += 2;//���ĳ��� = ����ͷ����+���ݳ���
	if ( len > 256 ) 
	{
		printf( "close client:invaild fromat msg\n" );
		pClient->Close();
		return;
	}
	if ( pClient->GetLength() < len + 2 ) return;//���ݳ��Ȳ������ȴ��´��ٶ�ȡ
	pClient->Recv(c, len);//�����Ķ��������ӽ��ջ�����ɾ�������Բ����ܳ��Ȳ�������ʹ�����Ѿ��Ͽ���Ҳ���Զ�������
	pClient->Send( c, len );//�յ���Ϣԭ���ظ�
}
