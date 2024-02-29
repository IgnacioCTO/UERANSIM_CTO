//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "task.hpp"

#include <algorithm>

#include <gnb/app/task.hpp>
#include <gnb/rrc/task.hpp>
#include <gnb/sctp/task.hpp>

#include <lib/sctp/types.hpp>
// #include "encode.hpp"
#include <cstring>
#include <thread>
#include <utility>

namespace nr::gnb
{

// Desde aqui puedo enviar un mensaje SCTP, puedo crear un udp y mandarlo con sctp
void GnbXnTask::sendXnRequest(int xnclientId, std::string xnlocalAddress, int64_t xnlocalPort, std::string xnremoteAddress, int64_t xnremotePort){
    
    // aqui envio el mensaje sctp con la info, pero es en gnb/sctp donde creo el cliente para conectarlo
    auto msg = std::make_unique<NmGnbSctp>(NmGnbSctp::XN_CONNECTION_REQUEST);

    msg->clientId = xnclientId;
    msg->localAddress = xnlocalAddress;
    msg->localPort = xnlocalPort;
    msg->remoteAddress = xnremoteAddress;
    msg->remotePort = xnremotePort;
    
    m_base->sctpTask->push(std::move(msg));
}

} // namespace nr::gnb