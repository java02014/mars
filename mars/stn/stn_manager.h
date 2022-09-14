//
// Created by Cpan on 2022/5/27.
//

#ifndef MMNET_STN_MANAGER_H
#define MMNET_STN_MANAGER_H

#endif  // MMNET_STN_MANAGER_H

#include "base_stn_manager.h"
#include "mars/app/context.h"
#include "mars/comm/thread/mutex.h"
#include "stn.h"
#include "stn_logic.h"

using namespace mars::comm;
using namespace mars::app;

namespace mars {

namespace comm {
class ProxyInfo;
}

namespace stn {

class NetCore;
class StnCallbackBridge;

class StnManager : public BaseStnManager {
 public:
    explicit StnManager(Context* context);
    ~StnManager() override;
    void Init() override;

 public:
    void OnInitConfigBeforeOnCreate(int _packer_encoder_version) override;
    void OnCreate() override;
    void OnDestroy() override;
    void OnSingalCrash(int _sig) override;
    void OnExceptionCrash() override;
    void OnNetworkChange() override;
    void OnNetworkDataChange(const char* _tag, ssize_t _send, ssize_t _recv) override;
#ifdef ANDROID
    void OnAlarm(int64_t _id) override;
#endif

 public:
    //外部接口
    void SetCallback(Callback* const callback) override;

 public:
    // #################### stn.h Callback ####################
    bool MakesureAuthed(const std::string& _host, const std::string& _user_id) override;

    //流量统计
    void TrafficData(ssize_t _send, ssize_t _recv) override;

    //底层询问上层该host对应的ip列表
    std::vector<std::string> OnNewDns(const std::string& _host, bool _longlink_host) override;
    //网络层收到push消息回调
    void OnPush(const std::string& _channel_id, uint32_t _cmdid, uint32_t _taskid, const AutoBuffer& _body, const AutoBuffer& _extend) override;
    //底层获取task要发送的数据
    bool Req2Buf(uint32_t taskid, void* const user_context, const std::string& _user_id, AutoBuffer& outbuffer, AutoBuffer& extend, int& error_code, const int channel_select, const std::string& host) override;
    //底层回包返回给上层解析
    int Buf2Resp(uint32_t taskid, void* const user_context, const std::string& _user_id, const AutoBuffer& inbuffer, const AutoBuffer& extend, int& error_code, const int channel_select) override;
    //任务执行结束
    int OnTaskEnd(uint32_t taskid, void* const user_context, const std::string& _user_id, int error_type, int error_code, const ConnectProfile& _profile) override;

    //上报网络连接状态
    void ReportConnectStatus(int status, int longlink_status) override;
    void OnLongLinkNetworkError(::mars::stn::ErrCmdType _err_type, int _err_code, const std::string& _ip, uint16_t _port) override;
    void OnShortLinkNetworkError(::mars::stn::ErrCmdType _err_type, int _err_code, const std::string& _ip, const std::string& _host, uint16_t _port) override;

    void OnLongLinkStatusChange(int _status) override;
    //长连信令校验 ECHECK_NOW = 0, ECHECK_NEVER = 1, ECHECK_NEXT = 2
    int GetLonglinkIdentifyCheckBuffer(const std::string& _channel_id, AutoBuffer& identify_buffer, AutoBuffer& buffer_hash, int32_t& cmdid) override;
    //长连信令校验回包
    bool OnLonglinkIdentifyResponse(const std::string& _channel_id, const AutoBuffer& response_buffer, const AutoBuffer& identify_buffer_hash) override;

    void RequestSync() override;
    //验证是否已登录

    //底层询问上层http网络检查的域名列表
    void RequestNetCheckShortLinkHosts(std::vector<std::string>& _hostlist) override;
    //底层向上层上报cgi执行结果
    void ReportTaskProfile(const ::mars::stn::TaskProfile& _task_profile) override;
    //底层通知上层cgi命中限制
    void ReportTaskLimited(int _check_type, const ::mars::stn::Task& _task, unsigned int& _param) override;
    //底层上报域名dns结果
    void ReportDnsProfile(const ::mars::stn::DnsProfile& _dns_profile) override;
    //.生成taskid.
    uint32_t GenTaskID() override;

    // #################### end stn.h ####################

    // #################### end stn_logci.h ####################

 public:
    // extern void SetLonglinkSvrAddr(const std::string& host, const std::vector<uint16_t> ports) override;
    // extern void SetShortlinkSvrAddr(const uint16_t port) override;

    // 'host' will be ignored when 'debugip' is not empty.
    void SetLonglinkSvrAddr(const std::string& host, const std::vector<uint16_t> ports, const std::string& debugip) override;

    // 'task.host' will be ignored when 'debugip' is not empty.
    void SetShortlinkSvrAddr(const uint16_t port, const std::string& debugip) override;

    // setting debug ip address for the corresponding host
    void SetDebugIP(const std::string& host, const std::string& ip) override;

    // setting backup iplist for the corresponding host
    // if debugip is not empty, iplist will be ignored.
    // iplist will be used when newdns/dns ip is not available.
    void SetBackupIPs(const std::string& host, const std::vector<std::string>& iplist) override;

    // async function.
    bool StartTask(const Task& task) override;

    // sync function
    void StopTask(uint32_t taskid) override;

    // check whether task's list has the task or not.
    bool HasTask(uint32_t taskid) override;

    // reconnect longlink and redo all task
    // when you change svr ip, you must call this function.
    void RedoTasks() override;

    // touch tasks loop. Generally, invoke it after autoauth successfully.
    void TouchTasks() override;

    //    need longlink channel
    void DisableLongLink() override;

    // stop and clear all task
    void ClearTasks() override;

    // the same as ClearTasks(), but also reinitialize network.
    void Reset() override;

    void ResetAndInitEncoderVersion(int _encoder_version) override;

    // setting signalling's parameters.
    // if you did not call this function, stn will use default value: period:  5s, keeptime: 20s
    void SetSignallingStrategy(long _period, long _keeptime) override;

    // used to keep longlink active
    // keep signnaling once 'period' and last 'keeptime'
    void KeepSignalling() override;

    void StopSignalling() override;

    // connect quickly if longlink is not connected.
    void MakesureLonglinkConnected() override;

    bool LongLinkIsConnected() override;

    bool ProxyIsAvailable(const mars::comm::ProxyInfo& _proxy_info, const std::string& _test_host, const std::vector<std::string>& _hardcode_ips) override;

    // noop is used to keep longlink conected
    // get noop taskid
    uint32_t getNoopTaskID() override;

    //===----------------------------------------------------------------------===//
    ///
    /// Support multi longlinks for mars
    /// these APIs are subject to change in developing
    ///
    //===----------------------------------------------------------------------===//
    void CreateLonglink_ext(LonglinkConfig& _config) override;
    void DestroyLonglink_ext(const std::string& name) override;
    std::vector<std::string> GetAllLonglink_ext() override;
    void MarkMainLonglink_ext(const std::string& name) override;

    bool LongLinkIsConnected_ext(const std::string& name) override;
    void MakesureLonglinkConnected_ext(const std::string& name) override;

    // #################### end stn_logci.h ####################

    // Add for Android
    //   ConnectProfile GetConnectProfile(uint32_t _taskid, int _channel_select) override;
    //   void AddServerBan(const std::string& _ip) override;
    //   void DisconnectLongLinkByTaskId(uint32_t _taskid, LongLink::TDisconnectInternalCode _code) override;
    // end Add for Android

 public:
    NetCore* GetNetCore();

 private:
    // Callback* callback_ = NULL;
    // StnCallbackBridge* stn_callback_bridge_ = NULL;
    // NetCore* net_core_;
    std::shared_ptr<Callback> callback_ = NULL;
//    std::shared_ptr<StnCallbackBridge> callback_bridge_ = NULL;
    std::shared_ptr<NetCore> net_core_ = NULL;

    comm::Mutex order_mutex_;
};  // StnManager

}  // namespace stn
}  // namespace mars
