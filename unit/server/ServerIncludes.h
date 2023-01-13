//
// Created by Kirill Zhukov on 08.11.2022.
//

#ifndef UNIT_SERVERINCLUDES_H
#define UNIT_SERVERINCLUDES_H

#include "sstream"
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include "db/DB.h"
#include "block/Block.h"

namespace beast = boost::beast;   // from <boost/beast.hpp>
namespace http = beast::http;     // from <boost/beast/http.hpp>
namespace net = boost::asio;      // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

inline void create_error_response(const std::string &message, http::response<http::dynamic_body> *response_) {
    response_->result(http::status::bad_request);
    response_->set(http::field::content_type, "application/json");
    response_->set(http::field::server, "Unit");
    beast::ostream(response_->body()) << message;
}

inline void create_success_response(const std::string &message, http::response<http::dynamic_body> *response_) {
    response_->result(http::status::ok);
    response_->set(http::field::content_type, "application/json");
    response_->set(http::field::server, "unit");
    beast::ostream(response_->body()) << message;
}

/// const errors
namespace rpcError {
    static constexpr char httpMethodError[106] = R"({"jsonrpc": "2.0", "error": {"code": -32003, "message": "HTTP request method not supported"}, "id": null})";
    static constexpr char parsingError[96] = R"({"jsonrpc": "2.0", "error": {"code": -32700, "message": "failed to parse request"}, "id": null})";
    static constexpr char emptyBalanceError[86] = R"({"jsonrpc": "2.0", "error": {"code": -32002, "message": "empty balance"}, "id": null})";
    static constexpr char defaultError[93] = R"({"jsonrpc": "2.0", "error": {"code": -32004, "message": "something went wrong"}, "id": null})";
    static constexpr char defaultAccountError[110] = R"({"jsonrpc": "2.0", "error": {"code": -32004, "message": "something happening with your account"}, "id": null})";
    static constexpr char badNonce[114] = R"({"jsonrpc": "2.0", "error": {"code": -32005, "message": "old nonce is used for current transaction"}, "id": null})";
    static constexpr char invalidSignature[90] = R"({"jsonrpc": "2.0", "error": {"code": -32006, "message": "invalid signature"}, "id": null})";
    static constexpr char lowBalance[84] = R"({"jsonrpc": "2.0", "error": {"code": -32000, "message": "low balance"}, "id": null})";
    static constexpr char invalidMethod[89] = R"({"jsonrpc": "2.0", "error": {"code": -32001, "message": "method not found"}, "id": null})";
    static constexpr char invalidParameter[90] = R"({"jsonrpc": "2.0", "error": {"code": -32001, "message": "invalid parameter"}, "id": null})";
}

namespace messageError {
    static constexpr char httpMethodError[34] = R"(HTTP request method not supported)";
    static constexpr char parsingError[24] = R"(failed to parse request)";
    static constexpr char emptyBalanceError[14] = R"(empty balance)";
    static constexpr char defaultError[21] = R"(something went wrong)";
    static constexpr char defaultAccountError[38] = R"(something happening with your account)";
    static constexpr char badNonce[42] = R"(old nonce is used for current transaction)";
    static constexpr char invalidSignature[18] = R"(invalid signature)";
    static constexpr char lowBalance[12] = R"(low balance)";
    static constexpr char invalidMethod[17] = R"(method not found)";
    static constexpr char invalidParameter[18] = R"(invalid parameter)";
}

namespace rpcResponse {
    std::string processSimpleResponse(const std::string &result, int id) {
        std::stringstream ss;
        ss << R"({"jsonrpc": "2.0", "result":)" << result << R"(, "id":")" << id << R"("})";
        return ss.str();
    }
    std::string processSimpleResponse(const std::string &result, const std::string &id) {
        std::stringstream ss;
        ss << R"({"jsonrpc": "2.0", "result":)" << result << R"(, "id":")" << id << R"("})";
        return ss.str();
    }
    std::string processSimpleStringResponse(const std::string &result, int id) {
        std::stringstream ss;
        ss << R"({"jsonrpc": "2.0", "result":")" << result << R"(", "id":")" << id << R"("})";
        return ss.str();
    }
    std::string processSimpleStringResponse(const std::string &result, const std::string &id) {
        std::stringstream ss;
        ss << R"({"jsonrpc": "2.0", "result":")" << result << R"(", "id":")" << id << R"("})";
        return ss.str();
    }
}
#endif //UNIT_SERVERINCLUDES_H
