
#include "RTML_monitor.h"

RTML_monitor::RTML_monitor(const useconds_t period) :
    m_state(Monitor_state(SCHED_FIFO, 1, period))
{

}

RTML_monitor::RTML_monitor(const useconds_t period, unsigned int schedule_policy, unsigned int priority) :
    m_state(Monitor_state(schedule_policy, priority, period))
{
    
}

void* RTML_monitor::loop(void* ptr) {
    RTML_monitor *monitor = (RTML_monitor *) ptr;
    struct timespec now = {}, next = {}, tmp = {};

    // Mutex and conditional variables for pthread_cond_timedwait
    pcheck_print( pthread_mutex_init ( &monitor->m_state.fmtx, NULL ), P_OK, return NULL; );
    pcheck_print( pthread_cond_init ( &monitor->m_state.cond, NULL ), P_OK, return NULL; );

    clock_gettime(CLOCK_REALTIME, &next);

    for (;;) {

        ::printf("loop+...\n");

        clock_gettime(CLOCK_REALTIME, &now);

        // convert useconds_t to struct timespec
        struct timespec p;

        ::printf("useconds: %lu\n", monitor->m_state.period);

        useconds_t2timespec( &monitor->m_state.period, &p );

        ::printf("timespecp: %lu,%lu\n", p.tv_sec, p.tv_nsec);

        ::printf("timespec: %lu,%lu\n", next.tv_sec, next.tv_nsec);

        timespecadd( &next, &p, &next );

        ::printf("timespec2: %lu,%lu\n", next.tv_sec, next.tv_nsec);

        ::printf("timespecnow: %lu,%lu\n", now.tv_sec, now.tv_nsec);
        
        

        if ( timespeccmp( &now, &next, > ) ) {

            timespecsub(&next, &now, &tmp);
            ::printf("RTML_monitor is missing their deadline for %lu s.%lu ns\n", tmp.tv_sec, tmp.tv_nsec);

        }

        pthread_mutex_lock( &monitor->m_state.fmtx );
        pcheck_print( pthread_cond_timedwait( &monitor->m_state.cond, &monitor->m_state.fmtx, &next ), ETIMEDOUT, break; );
        pthread_mutex_unlock( &monitor->m_state.fmtx );

        monitor->run();

        if ( monitor->m_state.status == ABORT )
        {
            monitor->m_state.status = ABORTED;
            return NULL;
        }
    }

    return NULL;
}

int RTML_monitor::enable()
{
    pthread_attr_t attribute = {};
    struct sched_param parameter;

    ::printf( "RTML_monitor started!\n" );

    if ( !isRunning() ) {

        pcheck( pthread_attr_init( &attribute ) );

        pcheck_attr( pthread_attr_setschedpolicy( &attribute, m_state.sched_policy ), &attribute );

        int stack_size = 5000;
        pcheck_attr( pthread_attr_setstacksize(&attribute, stack_size),  &attribute );
        DEBUGV("Stack:%d\n", stack_size);

        parameter.sched_priority = m_state.priority;
        DEBUGV("Priority:%d\n", m_state.priority);

        pcheck_attr( pthread_attr_setschedparam( &attribute, &parameter ), &attribute );

        pcheck_attr( pthread_create( &thread, &attribute, &loop, (void *) this ), &attribute );

        pcheck( pthread_attr_destroy( &attribute ) );

        m_state.status = RUNNING;

    } else {
        //errno = EALREADY;
        return -1;
    }

    return P_OK;
}

int RTML_monitor::disable()
{
    m_state.status = ABORT;
    return P_OK;
}

bool RTML_monitor::isRunning() const
{
    return m_state.status == RUNNING;
}

const useconds_t & RTML_monitor::getPeriod() const
{
    return m_state.period;
}

void RTML_monitor::setPeriod(const useconds_t & p)
{
    m_state.period = p;
}
