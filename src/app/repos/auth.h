#ifndef CONSTITUENT_REPOS_AUTH_H
#define CONSTITUENT_REPOS_AUTH_H

#include <memory>
#include "../db/connection_pool.h"
#include "../types/auth.h"

class AuthRepo {
private:
    std::shared_ptr<ConnectionPool> db;
public:
    explicit AuthRepo(std::shared_ptr<ConnectionPool> db);

    RegisterDeviceResult register_with_device(DeviceRegisterDto &&dto) const;
    Device get_device(const std::string& device_id) const;
    void store_device_challenge(const std::string& device_id, const std::string& challenge) const;
    void clear_device_challenge(const std::string& device_id) const;
    void remove_all_for_user() const;
};


#endif //CONSTITUENT_REPOS_AUTH_H