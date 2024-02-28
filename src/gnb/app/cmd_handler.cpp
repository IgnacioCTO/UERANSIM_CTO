//
// This file is a part of UERANSIM project.
// Copyright (c) 2023 ALİ GÜNGÖR.
//
// https://github.com/aligungr/UERANSIM/
// See README, LICENSE, and CONTRIBUTING files for licensing details.
//

#include "cmd_handler.hpp"

#include <gnb/app/task.hpp>
#include <gnb/gtp/task.hpp>
#include <gnb/ngap/task.hpp>
#include <gnb/rls/task.hpp>
#include <gnb/rrc/task.hpp>
#include <gnb/sctp/task.hpp>
#include <gnb/xn/task.hpp>
#include <utils/common.hpp>
#include <utils/printer.hpp>
#include <iostream>
#include <string>

#define PAUSE_CONFIRM_TIMEOUT 3000
#define PAUSE_POLLING 10

namespace nr::gnb
{

void GnbCmdHandler::sendResult(const InetAddress &address, const std::string &output)
{
    m_base->cliCallbackTask->push(std::make_unique<app::NwCliSendResponse>(address, output, false));
}

void GnbCmdHandler::sendError(const InetAddress &address, const std::string &output)
{
    m_base->cliCallbackTask->push(std::make_unique<app::NwCliSendResponse>(address, output, true));
}

void GnbCmdHandler::pauseTasks()
{
    m_base->gtpTask->requestPause();
    m_base->rlsTask->requestPause();
    m_base->ngapTask->requestPause();
    m_base->rrcTask->requestPause();
    m_base->sctpTask->requestPause();
    m_base->XnTask->requestPause();
}

void GnbCmdHandler::unpauseTasks()
{
    m_base->gtpTask->requestUnpause();
    m_base->rlsTask->requestUnpause();
    m_base->ngapTask->requestUnpause();
    m_base->rrcTask->requestUnpause();
    m_base->sctpTask->requestUnpause();
    m_base->XnTask->requestUnpause();
}

bool GnbCmdHandler::isAllPaused()
{
    if (!m_base->gtpTask->isPauseConfirmed())
        return false;
    if (!m_base->rlsTask->isPauseConfirmed())
        return false;
    if (!m_base->ngapTask->isPauseConfirmed())
        return false;
    if (!m_base->rrcTask->isPauseConfirmed())
        return false;
    if (!m_base->sctpTask->isPauseConfirmed())
        return false;
    if (!m_base->XnTask->isPauseConfirmed())
        return false;
    return true;
}

void GnbCmdHandler::handleCmd(NmGnbCliCommand &msg)
{
    pauseTasks();

    uint64_t currentTime = utils::CurrentTimeMillis();
    uint64_t endTime = currentTime + PAUSE_CONFIRM_TIMEOUT;

    bool isPaused = false;
    while (currentTime < endTime)
    {
        currentTime = utils::CurrentTimeMillis();
        if (isAllPaused())
        {
            isPaused = true;
            break;
        }
        utils::Sleep(PAUSE_POLLING);
    }

    if (!isPaused)
    {
        sendError(msg.address, "gNB is unable process command due to pausing timeout");
    }
    else
    {
        handleCmdImpl(msg);
    }

    unpauseTasks();
}

void GnbCmdHandler::handleCmdImpl(NmGnbCliCommand &msg)
{
    switch (msg.cmd->present)
    {
    case app::GnbCliCommand::STATUS: {
        sendResult(msg.address, ToJson(m_base->appTask->m_statusInfo).dumpYaml());
        break;
    }
    case app::GnbCliCommand::INFO: {
        sendResult(msg.address, ToJson(*m_base->config).dumpYaml());
        break;
    }
    case app::GnbCliCommand::AMF_LIST: {
        Json json = Json::Arr({});
        for (auto &amf : m_base->ngapTask->m_amfCtx)
            json.push(Json::Obj({{"id", amf.first}}));
        sendResult(msg.address, json.dumpYaml());
        break;
    }
    case app::GnbCliCommand::AMF_INFO: {
        if (m_base->ngapTask->m_amfCtx.count(msg.cmd->amfId) == 0)
            sendError(msg.address, "AMF not found with given ID");
        else
        {
            auto amf = m_base->ngapTask->m_amfCtx[msg.cmd->amfId];
            sendResult(msg.address, ToJson(*amf).dumpYaml());
        }
        break;
    }
    case app::GnbCliCommand::UE_LIST: {
        Json json = Json::Arr({});
        for (auto &ue : m_base->ngapTask->m_ueCtx)
        {
            json.push(Json::Obj({
                {"ue-id", ue.first},
                {"ran-ngap-id", ue.second->ranUeNgapId},
                {"amf-ngap-id", ue.second->amfUeNgapId},
            }));
        }
        sendResult(msg.address, json.dumpYaml());
        break;
    }
    case app::GnbCliCommand::UE_COUNT: {
        sendResult(msg.address, std::to_string(m_base->ngapTask->m_ueCtx.size()));
        break;
    }
    case app::GnbCliCommand::UE_RELEASE_REQ: {
        if (m_base->ngapTask->m_ueCtx.count(msg.cmd->ueId) == 0)
            sendError(msg.address, "UE not found with given ID");
        else
        {
            auto ue = m_base->ngapTask->m_ueCtx[msg.cmd->ueId];
            m_base->ngapTask->sendContextRelease(ue->ctxId, NgapCause::RadioNetwork_unspecified);
            sendResult(msg.address, "Requesting UE context release");
        }
        break;
    }
    case app::GnbCliCommand::HANDOVERPREPARE: {
        
        Json json = Json::Arr({});
        int ueid = msg.cmd->ueId;
        //std::cout << " ueid: "<< ueid << std::endl;
        if (m_base->ngapTask->m_ueCtx.count(msg.cmd->ueId) == 0)
            sendError(msg.address, "UE not found with given ID");
        else
        {
            auto *ue = m_base->ngapTask->findUeContext(ueid);
            auto *amf = m_base->ngapTask->findAmfContext(ue->associatedAmfId);
            json.push(Json::Obj({
                {"ue-id", ueid},
                {"AMF UE NGAP ID", std::to_string(ue->amfUeNgapId)},
                {"RAN UE NGAP ID", std::to_string(ue->ranUeNgapId)},
                {"UE uplink  stream ID", std::to_string(ue->uplinkStream)},
                {"AMF ID", std::to_string(ue->associatedAmfId)},
                {"AMF CTX ID", std::to_string(amf->ctxId)},
                {"AMF name",amf->amfName},
            }));
            sendResult(msg.address, json.dumpYaml());
        }
        break;
    }
    case app::GnbCliCommand::HANDOVER: {
        int asAmfId = msg.cmd->asAmfId; 
        int64_t amfUeNgapId = msg.cmd->amfUeNgapId;
        int64_t ranUeNgapId = msg.cmd->ranUeNgapId;
        int ctxtId = msg.cmd->ctxtId;
        int ulStr = msg.cmd->ulStr;
        std::string amf_name = msg.cmd->amf_name;
        std::cout << " asAmfId: "<< asAmfId << std::endl;
        std::cout << " amf_name: "<< amf_name << std::endl;
        m_base->ngapTask->handleXnHandover(asAmfId, amfUeNgapId, ranUeNgapId, ctxtId, ulStr, amf_name);
        
        break;
    } 
    case app::GnbCliCommand::BINDXN: {
        int xnclientId = msg.cmd->xnclientId; 
        std::string xnlocalAddress = msg.cmd->xnlocalAddress;
        int64_t xnlocalPort = msg.cmd->xnlocalPort;
        std::string xnremoteAddress = msg.cmd->xnremoteAddress;
        int64_t xnremotePort = msg.cmd->xnremotePort;
        m_base->XnTask->sendXnRequest(xnclientId, xnlocalAddress, xnlocalPort, xnremoteAddress, xnremotePort);
        break;
    } 
    }
}

} // namespace nr::gnb