#pragma once

#include "data_transport.h"
#include <cbmpc/protocol/mpc_job.h>

namespace coinbase::mpc {

typedef int32_t jsid_t;

class network_t : public data_transport_interface_t {
 public:
  network_t(std::shared_ptr<data_transport_interface_t> _data_transport_ptr, int _parallel_count = 1)
      : data_transport_ptr(_data_transport_ptr) {
    set_parallel(_parallel_count);
  }

  error_t send(const party_idx_t receiver, const jsid_t jsid, const mem_t msg);
  error_t receive(const party_idx_t sender, const jsid_t jsid, mem_t& msg);
  error_t receive_all(const std::vector<party_idx_t>& senders, jsid_t jsid, std::vector<mem_t>& msgs);

  // data_transport_interface_t overrides using jsid 0
  error_t send(const party_idx_t receiver, const mem_t& msg) override { return send(receiver, 0, msg); }
  error_t receive(const party_idx_t sender, mem_t& msg) override { return receive(sender, 0, msg); }
  error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<mem_t>& msgs) override {
    return receive_all(senders, 0, msgs);
  }

  void set_parallel(int _parallel_count);

 private:
  std::shared_ptr<data_transport_interface_t> data_transport_ptr;
  int parallel_count;

  // For parallel send
  int is_send_active = 0;
  std::mutex is_send_active_mtx;
  std::condition_variable send_active_cv;

  int send_ready = 0;
  std::mutex send_ready_mtx;
  std::condition_variable send_done_cv;
  std::condition_variable send_start_cv;

  std::vector<buf_t> send_msg;
  std::mutex send_msg_mutex;

  // For parallel receive
  int is_receive_active = 0;
  std::mutex is_receive_active_mtx;
  std::condition_variable receive_active_cv;

  int receive_ready = 0;
  std::mutex receive_ready_mtx;
  std::condition_variable receive_done_cv;
  std::condition_variable receive_start_cv;

  std::vector<buf_t> receive_msg;
  std::mutex receive_msg_mutex;

  // For parallel receive_all
  int is_receive_all_active = 0;
  std::mutex is_receive_all_mtx;
  std::condition_variable receive_all_active_cv;

  int receive_all_ready = 0;
  std::mutex receive_all_ready_mtx;
  std::condition_variable receive_all_done_cv;
  std::condition_variable receive_all_start_cv;

  std::unordered_map<party_idx_t, std::vector<buf_t>> receive_all_msgs;
  std::mutex receive_all_msgs_mutex;
};

class job_session_mp_t : public job_mp_t {
 protected:
  error_t send_impl(party_idx_t to, mem_t msg) override { return network_ptr->send(to, jsid, msg); }

  error_t receive_impl(party_idx_t from, mem_t& mem) override { return network_ptr->receive(from, jsid, mem); }

  error_t receive_many_impl(std::vector<party_idx_t> from_set, std::vector<mem_t>& outs) override {
    return network_ptr->receive_all(from_set, jsid, outs);
  }

 public:
  job_session_mp_t(party_idx_t index, std::vector<crypto::pname_t> pnames, std::shared_ptr<network_t> _network_ptr,
                   jsid_t _jsid)
      : job_mp_t(index, pnames, _network_ptr), network_ptr(_network_ptr), jsid(_jsid) {};

  void set_network(party_idx_t party_idx, std::shared_ptr<network_t> ptr) {
    party_index = party_idx;
    network_ptr = ptr;
    set_transport(party_idx, ptr);
  }

  job_session_mp_t get_parallel_job(int parallel_count, jsid_t jsid) {
    return job_session_mp_t(party_index, names, network_ptr, jsid);
  }

 protected:
  jsid_t jsid;
  std::shared_ptr<network_t> network_ptr;
};

class job_session_2p_t : public job_2p_t {
 public:
  job_session_2p_t(party_t party, crypto::pname_t pname1, crypto::pname_t pname2, std::shared_ptr<network_t> ptr,
                   jsid_t id = 0)
      : job_2p_t(party, pname1, pname2, ptr), network_ptr(ptr), jsid(id) {};

  void set_network(party_t party, std::shared_ptr<network_t> ptr) {
    party_index = party_idx_t(party);
    network_ptr = ptr;
    set_transport(party_idx_t(party), ptr);
  }

  job_session_2p_t get_parallel_job(int parallel_count, jsid_t jsid) {
    return job_session_2p_t(party_t(party_index), names[0], names[1], network_ptr, jsid);
  }

  void set_parallel_count(int parallel_count) { network_ptr->set_parallel(parallel_count); }

  error_t send_impl(party_idx_t to, mem_t msg) override { return network_ptr->send(to, jsid, msg); }
  error_t receive_impl(party_idx_t from, mem_t& mem) override { return network_ptr->receive(from, jsid, mem); }

 protected:
  jsid_t jsid;
  std::shared_ptr<network_t> network_ptr;
};

}  // namespace coinbase::mpc
