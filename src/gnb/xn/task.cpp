#include "task.hpp"
#include "cmd_handler.hpp"

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
}

void GnbXnTask::onLoop()
{
    auto msg = take();
    if (!msg)
        return;

    switch (msg->msgType)
    {
    case NtsMessageType::GNB_XN_TO_XN: {
        m_logger->debug("First Xn Message!");
        auto& w = dynamic_cast<NmGnbXnToXn &>(*msg);
        switch (w.what)
        {
        case NmGnbXnToXn::HANDOVER_REQUEST:
            m_logger->debug("First Xn HO REQUEST!");
            // m_statusInfo.isNgapUp = w.isNgapUp;
            break;
        }
        case NmGnbXnToXn::HANDOVER_REQUEST_ACK:
            m_logger->debug("First Xn HO REQUEST ACK!");
            // m_statusInfo.isNgapUp = w.isNgapUp;
            break;
        }
       
        break;
    }
    default:
        m_logger->unhandledNts(*msg);
        break;
    }
}

void GnbXnTask::onQuit()
{
}

} // namespace nr::gnb
