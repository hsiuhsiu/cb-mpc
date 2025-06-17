#pragma once
#include <cbmpc/crypto/base.h>
#include <vector>
#include <cstdint>

namespace coinbase::mpc {
using party_idx_t = int32_t; // forward declaration to avoid including mpc_job.h

class data_transport_interface_t {
 public:
  virtual error_t send(party_idx_t receiver, const mem_t& msg) = 0;
  virtual error_t receive(party_idx_t sender, mem_t& msg) = 0;
  virtual error_t receive_all(const std::vector<party_idx_t>& senders, std::vector<mem_t>& message) = 0;
  virtual ~data_transport_interface_t() = default;
};

} // namespace coinbase::mpc
