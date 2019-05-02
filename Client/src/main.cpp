#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>

#include <google/protobuf/util/json_util.h>

#include <iostream>

#include "protocol.pb.h"

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;


void on_connect(client* c, websocketpp::connection_hdl hdl) {
    // std::cout << "Print client name: ";
    // std::string name;
    // std::cin >> name;
    // 
    // c->send(hdl, name, websocketpp::frame::opcode::text);
}

// This message handler will be invoked once for each incoming message. It
// prints the message and then sends a copy of the message back to the server.
void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
    //std::cout << "on_message called with hdl: " << hdl.lock().get()
        //<< " and message: " << msg->get_payload()
        //<< std::endl;


    websocketpp::lib::error_code ec;

    GameState state;
    if (state.ParseFromString(msg->get_payload()))
    {
        std::string str;
        google::protobuf::util::MessageToJsonString(state, &str);
        std::cout << str;
    }

    //std::cout << "Specify action (0 - nothing, 1 - send hi): ";
    //int action;
    //std::cin >> action;
    //
    //if (action == 1) {
    //    c->send(hdl, "hi", msg->get_opcode(), ec);
    //}


    if (ec) {
        std::cout << "Echo failed because: " << ec.message() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Create a client endpoint
    client c;

    std::string uri = "ws://localhost:1234";

    if (argc == 2) {
        uri = argv[1];
    }

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));
        c.set_open_handler(bind(&on_connect, &c, ::_1));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    }
    catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }
}
