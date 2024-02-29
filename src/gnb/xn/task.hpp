#pragma once

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include <gnb/nts.hpp>
#include <gnb/types.hpp>
#include <utils/logger.hpp>
#include <utils/nts.hpp>

namespace nr::gnb
{

class SctpTask;
class GnbRrcTask;
class GtpTask;
class GnbAppTask;

// Ignacio
class GnbXnTask : public NtsTask
{
  private:

    TaskBase *m_base;
    std::unique_ptr<Logger> m_logger;

    GnbXnStatusInfo m_statusInfo;

    friend class GnbCmdHandler;

  public:
    explicit GnbXnTask(TaskBase *base);
    ~GnbXnTask() override = default;

  protected:
    void onStart() override;
    void onLoop() override;
    void onQuit() override;

  private:
    /* Interface management */
    void sendXnRequest(int xnclientId, std::string xnlocalAddress, int64_t xnlocalPort, std::string xnremoteAddress, int64_t xnremotePort);
};

} // namespace nr::gnb
