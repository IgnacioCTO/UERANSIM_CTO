#include "task.hpp"
//#include "types.hpp"

#include <gnb/nts.hpp>

namespace nr::gnb
{
// Ignacio
GnbXnTask::GnbXnTask(TaskBase *base) : m_base{base}, m_statusInfo{}
{
    m_logger = m_base->logBase->makeUniqueLogger("Xn");
}

void GnbXnTask::onStart()
{
    m_logger->info("Interface Xn Created.");
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
