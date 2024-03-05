//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "server.hpp"
#include "internal.hpp"

sctp::SctpServer::SctpServer(const std::string &address, uint16_t port) : sd(0)
{
    try
    {
        sd = CreateSocket(); //sd is the descriptor identifier 
        BindSocket(sd, address, port);
        SetInitOptions(sd, 10, 10, 10, 10 * 1000); //max instreams, outstreams,max attemps, timeout
        SetEventOptions(sd); //set subscribed events
        StartListening(sd); 
    }
    catch (const SctpError &e)
    {
        CloseSocket(sd);
        throw;
    }
}

sctp::SctpServer::~SctpServer()
{
    CloseSocket(sd);
}

void sctp::SctpServer::start()
{
    Accept(sd);  
}
