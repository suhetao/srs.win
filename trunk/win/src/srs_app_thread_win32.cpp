#include <srs_app_thread_win32.hpp>
#include <srs_kernel_error.hpp>
#include <srs_kernel_log.hpp>

ISrsThreadHandler::ISrsThreadHandler()
{
}

ISrsThreadHandler::~ISrsThreadHandler()
{
}

void ISrsThreadHandler::on_thread_start()
{
}

int ISrsThreadHandler::on_before_cycle()
{
    int ret = ERROR_SUCCESS;
    return ret;
}

int ISrsThreadHandler::on_end_cycle()
{
    int ret = ERROR_SUCCESS;
    return ret;
}

void ISrsThreadHandler::on_thread_stop()
{
}

SrsThread::SrsThread(ISrsThreadHandler* thread_handler, int64_t interval_us)
{
    handler = thread_handler;
    cycle_interval_us = interval_us;
    
    hThread = NULL;
    loop = false;
    _cid = -1;
}

SrsThread::~SrsThread()
{
    stop();
}

int SrsThread::cid()
{
    return _cid;
}

int SrsThread::start()
{
    int ret = ERROR_SUCCESS;
    
    if(hThread) {
        srs_info("thread already running.");
        return ret;
    }
    
	hThread = (HANDLE)_beginthreadex(NULL,0,thread_fun,this,0,NULL);
    if(hThread == NULL){
        ret = ERROR_ST_CREATE_CYCLE_THREAD;
        srs_error("CreateThread failed. ret=%d", ret);
        return ret;
    }
    
    // we set to loop to true for thread to run.
    loop = true;
    
    // wait for cid to ready, for parent thread to get the cid.
    while (_cid < 0) {
        usleep(10 * SRS_TIME_MILLISECONDS);
    }
    
    return ret;
}

void SrsThread::stop()
{
    if (hThread) {
        loop = false;
		_endthreadex((uint32_t)hThread);
        
        hThread = NULL;
    }
}

bool SrsThread::can_loop()
{
    return loop;
}

void SrsThread::stop_loop()
{
    loop = false;
}

void SrsThread::thread_cycle()
{
    int ret = ERROR_SUCCESS;
    
    _srs_context->generate_id();
    srs_info("thread cycle start");
    
    _cid = _srs_context->get_id();
    
    srs_assert(handler);
    handler->on_thread_start();
    
    while (loop) {
        if ((ret = handler->on_before_cycle()) != ERROR_SUCCESS) {
            srs_warn("thread on before cycle failed, ignored and retry, ret=%d", ret);
            goto failed;
        }
        srs_info("thread on before cycle success");
        
        if ((ret = handler->cycle()) != ERROR_SUCCESS) {
            srs_warn("thread cycle failed, ignored and retry, ret=%d", ret);
            goto failed;
        }
        srs_info("thread cycle success");
        
        if ((ret = handler->on_end_cycle()) != ERROR_SUCCESS) {
            srs_warn("thread on end cycle failed, ignored and retry, ret=%d", ret);
            goto failed;
        }
        srs_info("thread on end cycle success");

failed:
        if (!loop) {
            break;
        }
        
        usleep(cycle_interval_us);
    }
    
    handler->on_thread_stop();
    srs_info("thread cycle finished");
}

uint32_t SrsThread::thread_fun(void* arg)
{
    SrsThread* obj = (SrsThread*)arg;
    srs_assert(obj);
    
    obj->thread_cycle();
    
    return NULL;
}