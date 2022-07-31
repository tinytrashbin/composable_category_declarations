#include "tracing.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using std::string;

namespace perfetto {

Trace g_packets;

Session StartSession() {
  g_packets.clear();
  for (size_t i = 0; i <= internal::kMaxCategorySlotNumber; i++) {
    const auto& slot = internal::category_declaration_slots[i];
    std::cout << "slot[" << i << "].length = " << slot.length << std::endl;
    for (size_t j = 0; j < slot.length; j++) {
      size_t category_id = (j << internal::kNumBitsRequiredForCategorySlotNumber) | i;
      AddPacket(TracePacket{TracePacket::MappingIID, category_id, "",
                            slot.categories[j].name});
    }
  }
  return Session{};
}

void Print(const std::vector<TracePacket>& packets) {
  for (size_t i = 0; i < packets.size(); i++) {
    const auto& packet = packets[i];
    std::cout << "packet[" << i << "]: {";
    if (packet.type ==TracePacket::MappingIID) {
      std::cout << "category_id " << packet.category_id << " => "
                << packet.category_name;
    } else {
      std::cout << "category_id=" << packet.category_id << ", event_name="
        << packet.event_name;
      if (packet.category_name.size() > 0) {
        std::cout << ", category_name = " << packet.category_name;
      }
    }
    std::cout << "}\n";
  }
}

Trace TraceProcess(const Trace& packets) {
  Trace output;
  std::unordered_map<size_t, string> category_id_map;
  for (auto& packet : packets) {
    if (packet.type == TracePacket::MappingIID) {
      category_id_map[packet.category_id] = packet.category_name;
      output.push_back(packet);
    } else {
      auto new_packet = packet;
      new_packet.category_name = category_id_map[packet.category_id];
      output.push_back(std::move(new_packet));
    }
  }
  return output;
}

}  // namespace perfetto
