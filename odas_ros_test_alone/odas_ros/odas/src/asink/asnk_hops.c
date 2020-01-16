    
    #include "asnk_hops.h"

    asnk_hops_obj * asnk_hops_construct(const snk_hops_cfg * snk_hops_config, const msg_hops_cfg * msg_hops_config) {

        asnk_hops_obj * obj;

        obj = (asnk_hops_obj *) malloc(sizeof(asnk_hops_obj));

        obj->snk_hops = snk_hops_construct(snk_hops_config, msg_hops_config);
        obj->in = (amsg_hops_obj *) NULL;

        obj->thread = thread_construct(&asnk_hops_thread, (void *) obj);

        return obj;

    }

    void asnk_hops_destroy(asnk_hops_obj * obj) {

        snk_hops_destroy(obj->snk_hops);
        thread_destroy(obj->thread);

        free((void *) obj);

    }

    void asnk_hops_connect(asnk_hops_obj * obj, amsg_hops_obj * in) {

        obj->in = in;

    }

    void asnk_hops_disconnect(asnk_hops_obj * obj) {

        obj->in = (amsg_hops_obj *) NULL;

    }

    void * asnk_hops_thread(void * ptr) {

        asnk_hops_obj * obj;
        msg_hops_obj * msg_hops_in;
        int rtnValue;

        obj = (asnk_hops_obj *) ptr;

        // Open the sink
        snk_hops_open(obj->snk_hops);

        while(1) {

            // Pop a message, process, and push back
            msg_hops_in = amsg_hops_filled_pop(obj->in);
            snk_hops_connect(obj->snk_hops, msg_hops_in);
            rtnValue = snk_hops_process(obj->snk_hops);
            snk_hops_disconnect(obj->snk_hops);
            amsg_hops_empty_push(obj->in, msg_hops_in);

            // If this is the last frame, rtnValue = -1
            if (rtnValue == -1) {
                break;
            }

        }

        // Close the sink
        snk_hops_close(obj->snk_hops);

    }