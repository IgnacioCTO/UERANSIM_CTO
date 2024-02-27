#pragma once

#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include <gnb/types.hpp>
#include <utils/logger.hpp>
#include <utils/nts.hpp>

namespace nr::gnb
{
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
};

} // namespace nr::gnb
