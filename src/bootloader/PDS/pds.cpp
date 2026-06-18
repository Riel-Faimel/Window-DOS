#include "_pds.hpp"

void PDS::append(Info i, Storage::drive* d){
    device_space.s.append({i, d});
}
void PDS::append(Info i, Network::drive* d){
    device_space.n.append({i, d});
}
void PDS::append(Info i, Bridge::drive* d){
    device_space.b.append({i, d});
}
void PDS::remove(unsigned long long tpid){
    auto [s, n, b] = device_space;
    size_t i = 0;
    for (auto item : s) {
        if (item.info.type_id == tpid) {
            s.remove(i);
            return;
        }
        i++;
    }
    i = 0;
    for (auto item : n) {
        if (item.info.type_id == tpid) {
            n.remove(i);
            return;
        }
        i++;
    }
    i = 0;
    for (auto item : b) {
        if (item.info.type_id == tpid) {
            b.remove(i);
            return;
        }
        i++;
    }
    return;
}