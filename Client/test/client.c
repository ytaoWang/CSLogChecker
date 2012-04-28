#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <zmq.h>

int main(int argc,char *argv[])
{
    int rc;
    void *ctx,*s;
    const char *query_string = "hello ZeroMQ";
    zmq_msg_t query,resultset;
    
    ctx = zmq_init(1);
    assert(ctx);
    
    s = zmq_socket(ctx,ZMQ_REQ);
    assert(s);
    
    rc = zmq_connect(s,"tcp://127.0.0.1:5555");
    assert(rc == 0);
    
    //send
    rc = zmq_msg_init_size(&query,strlen(query_string) + 1);
    assert(rc == 0);
    memcpy(zmq_msg_data(&query),query_string,strlen(query_string) + 1);
    
    rc = zmq_send(s,&query,0);
    assert(rc == 0);
    
    //recv
    rc = zmq_msg_init(&resultset);
    assert(rc == 0);
    
    rc = zmq_recv(s,&resultset,0);
    assert(rc == 0);
    
    printf("ack:%s\n",(const char *)zmq_msg_data(&resultset));
    zmq_msg_close(&resultset);
    
    zmq_close(s);
    zmq_term(ctx);
    
    return 0;
}
