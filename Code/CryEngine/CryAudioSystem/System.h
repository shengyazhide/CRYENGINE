// Copyright 2001-2018 Crytek GmbH / Crytek Group. All rights reserved.

#pragma once

#include "MainThread.h"
#include "Request.h"
#include <CryAudio/IAudioSystem.h>
#include <CrySystem/ISystem.h>
#include <CrySystem/TimeValue.h>
#include <CryInput/IInput.h>
#include <concqueue/concqueue.hpp>

namespace CryAudio
{
class CSystem final : public IAudioSystem, public ::ISystemEventListener, public IInputEventListener
{
public:

	CSystem() = default;
	virtual ~CSystem() override;

	CSystem(CSystem const&) = delete;
	CSystem(CSystem&&) = delete;
	CSystem& operator=(CSystem const&) = delete;
	CSystem& operator=(CSystem&&) = delete;

	// CryAudio::IAudioSystem
	virtual void        Release() override;
	virtual void        SetImpl(Impl::IImpl* const pIImpl, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        LoadTrigger(ControlId const triggerId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        UnloadTrigger(ControlId const triggerId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ExecuteTriggerEx(SExecuteTriggerData const& triggerData, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ExecuteTrigger(ControlId const triggerId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        StopTrigger(ControlId const triggerId = InvalidControlId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        SetParameter(ControlId const parameterId, float const value, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        SetGlobalParameter(ControlId const parameterId, float const value, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        SetSwitchState(ControlId const switchId, SwitchStateId const switchStateId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        SetGlobalSwitchState(ControlId const switchId, SwitchStateId const switchStateId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        PlayFile(SPlayFileInfo const& playFileInfo, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        StopFile(char const* const szName, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ReportStartedFile(CStandaloneFile& standaloneFile, bool const bSuccessfullyStarted, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ReportStoppedFile(CStandaloneFile& standaloneFile, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ReportFinishedTriggerConnectionInstance(TriggerInstanceId const triggerInstanceId, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ReportPhysicalizedObject(Impl::IObject* const pIObject, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ReportVirtualizedObject(Impl::IObject* const pIObject, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        StopAllSounds(SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        PreloadSingleRequest(PreloadRequestId const id, bool const bAutoLoadOnly, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        UnloadSingleRequest(PreloadRequestId const id, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        LoadSetting(ControlId const id, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        UnloadSetting(ControlId const id, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        ReloadControlsData(char const* const szFolderPath, char const* const szLevelName, SRequestUserData const& userData = SRequestUserData::GetEmptyObject()) override;
	virtual void        AddRequestListener(void (*func)(SRequestInfo const* const), void* const pObjectToListenTo, ESystemEvents const eventMask) override;
	virtual void        RemoveRequestListener(void (*func)(SRequestInfo const* const), void* const pObjectToListenTo) override;
	virtual void        ExternalUpdate() override;
	virtual char const* GetConfigPath() const override;
	virtual IListener*  CreateListener(CTransformation const& transformation, char const* const szName = nullptr) override;
	virtual void        ReleaseListener(IListener* const pIListener) override;
	virtual IObject*    CreateObject(SCreateObjectData const& objectData = SCreateObjectData::GetEmptyObject()) override;
	virtual void        ReleaseObject(IObject* const pIObject) override;
	virtual void        GetFileData(char const* const szName, SFileData& fileData) override;
	virtual void        GetTriggerData(ControlId const triggerId, STriggerData& triggerData) override;
	virtual void        GetImplInfo(SImplInfo& implInfo) override;
	virtual void        Log(ELogType const type, char const* const szFormat, ...) override;

	// Below data is only used when CRY_AUDIO_USE_PRODUCTION_CODE is defined!
	virtual void ExecutePreviewTrigger(ControlId const triggerId) override;
	virtual void ExecutePreviewTriggerEx(Impl::ITriggerInfo const& triggerInfo) override;
	virtual void StopPreviewTrigger() override;
	// ~CryAudio::IAudioSystem

	// ISystemEventListener
	virtual void OnSystemEvent(ESystemEvent event, UINT_PTR wparam, UINT_PTR lparam) override;
	// ~ISystemEventListener

	// IInputEventListener
	virtual bool OnInputEvent(SInputEvent const& event) override;
	// ~IInputEventListener

	bool Initialize();
	void PushRequest(CRequest const& request);
	void ParseControlsData(char const* const szFolderPath, EDataScope const dataScope, SRequestUserData const& userData = SRequestUserData::GetEmptyObject());
	void ParsePreloadsData(char const* const szFolderPath, EDataScope const dataScope, SRequestUserData const& userData = SRequestUserData::GetEmptyObject());
	void AutoLoadSetting(EDataScope const dataScope, SRequestUserData const& userData = SRequestUserData::GetEmptyObject());

	void InternalUpdate();

private:

	using Requests = ConcQueue<UnboundMPSC, CRequest>;
	using RequestsSyncCallbacks = ConcQueue<UnboundSPSC, CRequest>;

	void           ReleaseImpl();
	void           OnLanguageChanged();
	void           ProcessRequests(Requests& requestQueue);
	void           ProcessRequest(CRequest& request);
	ERequestStatus ProcessSystemRequest(CRequest const& request);
	ERequestStatus ProcessCallbackRequest(CRequest& request);
	ERequestStatus ProcessObjectRequest(CRequest const& request);
	ERequestStatus ProcessListenerRequest(SRequestData const* const pPassedRequestData);
	void           NotifyListener(CRequest const& request);
	ERequestStatus HandleSetImpl(Impl::IImpl* const pIImpl);
	void           SetImplLanguage();
	void           SetCurrentEnvironmentsOnObject(CObject* const pObject, EntityId const entityToIgnore);
#if defined(CRY_AUDIO_USE_OCCLUSION)
	void           SetOcclusionType(CObject& object, EOcclusionType const occlusionType) const;
#endif // CRY_AUDIO_USE_OCCLUSION
	void           ExecuteDefaultTrigger(EDefaultTriggerType const type, SRequestUserData const& userData = SRequestUserData::GetEmptyObject());

	static void    OnCallback(SRequestInfo const* const pRequestInfo);

	bool                  m_isInitialized = false;
	bool                  m_didThreadWait = false;
	volatile float        m_accumulatedFrameTime = 0.0f;
	std::atomic<uint32>   m_externalThreadFrameId{ 0 };
	uint32                m_lastExternalThreadFrameId = 0;
	uint16                m_objectPoolSize = 0;
	SImplInfo             m_implInfo;
	CMainThread           m_mainThread;

	Requests              m_requestQueue;
	RequestsSyncCallbacks m_syncCallbacks;
	CRequest              m_syncRequest;
	CryEvent              m_mainEvent;
	CryEvent              m_audioThreadWakeupEvent;

#if defined(CRY_AUDIO_USE_PRODUCTION_CODE)
public:

	void RetriggerControls(SRequestUserData const& userData = SRequestUserData::GetEmptyObject());
	void ResetRequestCount();
	void ScheduleIRenderAuxGeomForRendering(IRenderAuxGeom* pRenderAuxGeom);

private:

	void SubmitLastIRenderAuxGeomForRendering();
	void DrawDebug();
	void HandleDrawDebug();
	void HandleRetriggerControls();
	void HandleRefresh(char const* const szLevelName);

	std::atomic<IRenderAuxGeom*> m_currentRenderAuxGeom{ nullptr };
	std::atomic<IRenderAuxGeom*> m_lastRenderAuxGeom{ nullptr };
#endif // CRY_AUDIO_USE_PRODUCTION_CODE
};
} // namespace CryAudio
