/**	
 *	Creates a pool of threads, which will service a queue of funtors.
 * 
 *	The pool of threads is determined by the MAX_THREADS define, or passed in through the constructor.   
 *  To have a thread  execute a request, the user submits a ThreadRequest derived command 
 *  object (functor) to the thread queue.  (I was going to call it threadJob but that's rude).
 *  The request handler will pull this out of the queue, and run the functors operator() method.
 *  If the queue is full, determined by the MAX_QUEUE_SIZE define, or the constructor parameter, 
 *  then the submitRequest method will block preventing new items being added to the queue.  
 *  These are implemented via a ManualEvent object.
 *
 *	@warning Exceptions WILL NOT propogate out of the accept handler.  You MUST catch 
 *           your exceptions in the ThreadRequest derived functor.
 *
 *	@author Peter Hancock
 */
#pragma once

//�߳�����
class CThreadRequest
{
public:
	//����ִ�к����������Լ������쳣
	virtual void ThreadProc(unsigned int nThreadId) = 0;	
};

class CThreadPool  
{
public:
	//�߳̿�ʼ�������ݽṹ
	struct ThreadData
	{
		CThreadPool *pThreadPool;	// this
		unsigned int nThreadId;		//�߳�id
	};
	class SuicideException
	{
	};
	class SuicidePill : public CThreadRequest
	{
	public:
		virtual void ThreadProc(unsigned int)
		{
			throw SuicideException();
		}
	};

public:
	enum { THREAD_COUNT = 5, MAX_QUEUE_SIZE = 100 };

	//�̳߳ع��캯��
	// threadSize - �߳���
	// queueSize  - ������д�С
	CThreadPool(int threadSize=THREAD_COUNT, int queueSize=MAX_QUEUE_SIZE);
	virtual ~CThreadPool();

	//�̳߳ؿ�ʼ����
	int						Begin(void);
	//���빤��Ҫ�󣬲��ÿ���CThreadRequest���ڴ��ͷ�
	//����false��ʾ�����������Ѿ��ر�
	BOOL					PushRequest(CThreadRequest* pRequest);
	//�ر�
	void					ShutDown(void);
	//
	int						GetThreadCount(void) const { return (int)m_vThreadPool.size(); }
	//ɱ���������̣߳�������һ���µ��߳̽������
	bool					tryKillThread(unsigned int threadID);

private:
	//�̳߳��Ƿ��ڼ����У�ֻ���ڼ���״̬���ܽ�������
	BOOL							m_bAlive;
	//�߳̾����
	std::vector< HANDLE >			m_vThreadPool;
	//�߳�ID��
	std::vector< unsigned int >		m_vThreadIDPool;
	//�������
	std::queue< CThreadRequest* >	m_dJobQueue;
	//����������ߴ�
	int								m_nQueueSize;
	//������������ź���������ȷ��ÿһ������ֻ��һ���߳̽��յ�
	HANDLE							m_hQueueAccessSemaphone;
	//����ȷ���������̰߳�ȫ��
	CRITICAL_SECTION				m_csQueue;
	//����ȷ�������п����λ��
	HANDLE							m_hQueueNotFullEvent;

	static unsigned int __stdcall _ThreadProc(void* lpParam);
	void ThreadProc(unsigned int nThreadId) throw();
};
