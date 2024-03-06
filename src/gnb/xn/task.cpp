#include "task.hpp"
#include <thread>
#include <iostream>
#include <string>
//#include "types.hpp"

#include <gnb/nts.hpp>
#include <gnb/app/task.hpp>
#include <gnb/sctp/task.hpp>
namespace nr::gnb
{
// Ignacio
GnbXnTask::GnbXnTask(TaskBase *base) : m_base{base}, m_statusInfo{}
{
    m_logger = m_base->logBase->makeUniqueLogger("xn");
}

void GnbXnTask::onStart()
{    
    std::string str = m_base->config->ngapIp;
    size_t last_index = str.find_last_not_of("0123456789");
    int server_port = std::stoi(str.substr(last_index + 1)) + 8080;

    m_logger->info("Interface Xn created");

    if (server_port == 8084){
        sctp::SctpServer xnserver("127.0.0.1", server_port);//TODO:xnIp in config m_base->config->ngapIp
        xnserver.start();
        m_logger->info("Xn server interface is active");
    } else {
        auto msg = std::make_unique<NmGnbSctp>(NmGnbSctp::XN_CONNECTION_REQUEST);
        msg->clientId = 7;
        msg->localAddress = "127.0.0.1";
        msg->localPort = 5555;
        msg->remoteAddress = "127.0.0.1";
        msg->remotePort = 8084;
        msg->ppid = sctp::PayloadProtocolId::NGAP;
        msg->associatedTask = this;
        m_base->sctpTask->push(std::move(msg));   
        m_logger->info("Xn connection request sent");
    }

    // here is where i should send the request to connect to xn
    // cause i need to include this instance as the assoc task
    // looking at onstart ngap task.cpp is where it launch the
    // petition
}

void GnbXnTask::onLoop()
{
    auto msg = take();
    if (!msg)
        return;

    if (msg->msgType == NtsMessageType::GNB_XN_TO_XN)
    {
   
        m_logger->debug("First Xn Message!");
        auto& w = dynamic_cast<NmGnbXnToXn &>(*msg);

        switch (w.present)
        {
        case NmGnbXnToXn::HANDOVER_REQUEST:
            m_logger->debug("First Xn HO REQUEST!");
            break;
        
        case NmGnbXnToXn::HANDOVER_REQUEST_ACK:
            m_logger->debug("First Xn HO REQUEST ACK!");
            break;
             
        default:
            m_logger->unhandledNts(*msg);
            break;
        }
    }
}


void GnbXnTask::onQuit()
{
} 

} // namespace nr::gnb
