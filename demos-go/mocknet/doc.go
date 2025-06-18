/*
In cpp:
- An implementation of the data_transport_interface_t, called callback_data_transport_t, which gets a bunch of callbacks in the constructor

In cb-mpc-go/network:
- Create a bunch of callbacks as intefaces, data_transport_callback_interface

- Create a function that gets implementaion of data_transport_callback_interface and returns an instance of parallel_data_transport_t
 - Create another function that gets a parallel_data_transport_t pointer, and parallel_id, etc that call cpp to return job_parallel_2p_t pointer

In cb-mpc-go/mocknetwork:
- A mock implementation of the data_transport_callback_interface, called demo_data_transpot
*/
package mocknet
