#ifndef IRODS_USER_ADMINISTRATION_HPP
#define IRODS_USER_ADMINISTRATION_HPP

#undef NAMESPACE_IMPL
#undef rxComm

#ifdef IRODS_USER_ADMINISTRATION_ENABLE_SERVER_SIDE_API
    #define NAMESPACE_IMPL      server
    #define rxComm              rsComm_t
#else 
    #define NAMESPACE_IMPL      client
    #define rxComm              rcComm_t
#endif // IRODS_USER_ADMINISTRATION_ENABLE_SERVER_SIDE_API

#include "rcConnect.h"

#include <vector>
#include <string>
#include <optional>
#include <ostream>
#include <system_error>
#include <stdexcept>

namespace irods::experimental::administration
{
    // Types

    enum class user_type
    {
        rodsuser,
        groupadmin,
        rodsadmin
    }; // user_type

    enum class zone_type
    {
        local,
        remote
    }; // zone_type

    struct user
    {
        explicit user(std::string name, std::optional<std::string> zone = std::nullopt)
            : name{std::move(name)}
            , zone{zone ? *zone : ""}
        {
        }

        std::string name;
        std::string zone;
    }; // user

    struct group
    {
        explicit group(std::string name)
            : name{std::move(name)}
        {
        }

        std::string name;
    }; // group

    // Exceptions

    class user_management_error
        : std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

namespace irods::experimental::administration::NAMESPACE_IMPL
{
    // User Management

    auto make_user(rxComm& conn,
                   const user& user,
                   user_type user_type = user_type::rodsuser,
                   zone_type zone_type = zone_type::local) -> std::error_code;

    auto remove_user(rxComm& conn, const user& user) -> std::error_code;

    auto set_user_password(rxComm& conn, const user& user, std::string_view new_password) -> std::error_code;

    auto set_user_type(rxComm& conn, const user& user, user_type new_user_type) -> std::error_code;

    auto add_user_auth(rxComm& conn, const user& user, std::string_view auth) -> std::error_code;

    auto remove_user_auth(rxComm& conn, const user& user, std::string_view auth) -> std::error_code;

    // Group Management

    auto make_group(rxComm& conn, const group& group) -> std::error_code;

    auto remove_group(rxComm& conn, const group& group) -> std::error_code;

    auto add_user_to_group(rxComm& conn, const group& group, const user& user) -> std::error_code;

    auto remove_user_from_group(rxComm& conn, const group& group, const user& user) -> std::error_code;

    // Query

    auto users(rxComm& conn) -> std::vector<user>;
    auto users(rxComm& conn, const group& group) -> std::vector<user>;

    auto groups(rxComm& conn) -> std::vector<group>;
    auto groups(rxComm& conn, const user& user) -> std::vector<group>;

    auto exists(rxComm& conn, const user& user) -> bool;
    auto exists(rxComm& conn, const group& group) -> bool;

    auto id(rxComm& conn, const user& user) -> std::optional<std::string>;
    auto id(rxComm& conn, const group& group) -> std::optional<std::string>;

    auto type(rxComm& conn, const user& user) -> std::optional<user_type>;
    auto auth_names(rxComm& conn, const user& user) -> std::vector<std::string>;

    auto user_is_member_of_group(rxComm& conn, const group& group, const user& user) -> bool;

    // Utility

    auto local_unique_name(rxComm& conn, const user& user) -> std::string;

    auto operator<<(std::ostream& out, const user& user) -> std::ostream&;
    auto operator<<(std::ostream& out, const group& group) -> std::ostream&;
} // namespace irods::experimental::administration::NAMESPACE_IMPL

#endif // IRODS_USER_ADMINISTRATION_HPP

