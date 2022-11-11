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

namespace rpcResponse {
    std::string processSimpleResponse(const std::string &result, const std::string &id) {
        std::stringstream ss;
        ss << R"({"jsonrpc": "2.0", "result":)" << result << R"(, "id":")" << id << R"("})";
    }
}
#endif //UNIT_SERVERINCLUDES_H
