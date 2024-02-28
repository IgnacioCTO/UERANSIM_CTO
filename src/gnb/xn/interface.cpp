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

// #include "encode.hpp"


namespace nr::gnb
{

void GnbXnTask::sendXnRequest(int xnclientId, std::string xnlocalAddress, int64_t xnlocalPort, std::string xnremoteAddress, int64_t xnremotePort){
    
    //PayloadProtocolId ppid = PayloadProtocolId::NGAP; //TODO

    //auto *client = new sctp::SctpClient(ppid);

}

} // namespace nr::gnb