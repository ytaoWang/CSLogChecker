#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <zmq.h>

int main(int argc,char *argv[])
{
    int rc;
    void *ctx,*s;
    zmq_msg_t query,resultset;
    const char *query_string,*resultset_string = "OK";
    
    ctx = zmq_init(1);
    assert(ctx);
    
    s = zmq_socket(ctx,ZMQ_REP);
    assert(s);
    
    rc = zmq_bind(s,"tcp://127.0.0.1:5555");
    assert(rc == 0);
    
    while(1) 
    {
        rc = zmq_msg_init(&query);
        assert(rc == 0);
        
        rc = zmq_recv(s,&query,0);
        assert(rc == 0);
        
        query_string = (const char *)zmq_msg_data(&query);
        printf("recv:%s\n",query_string);
        zmq_msg_close(&query);
        
        rc = zmq_msg_init_size(&resultset,strlen(resultset_string) + 1);
        assert(rc == 0);
        
        memcpy(zmq_msg_data(&resultset),resultset_string,strlen(resultset_string) + 1);
        
        rc = zmq_send(s,&resultset,0);
        assert(rc == 0);
        zmq_msg_close(&resultset);
    }
    return 0;
    
}
