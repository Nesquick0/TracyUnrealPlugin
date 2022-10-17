// Copyright(c) 2019 Vadim Slyusarev

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "ProfilingDebugging/ExternalProfiler.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTracy, Log, All);

/**
* Tracy implementation of FExternalProfiler
*/
class FTracyExternalProfiler : public FExternalProfiler, public FOutputDevice
{
public:
	/** Constructor */
	FTracyExternalProfiler();
	/** Destructor */
	virtual ~FTracyExternalProfiler();
	virtual void FrameSync() override;
	/** Gets the name of this profiler as a string.  This is used to allow the user to select this profiler in a system configuration file or on the command-line */
	virtual const TCHAR* GetProfilerName() const override;
	/** Pauses profiling. */
	virtual void ProfilerPauseFunction() override;
	/** Resumes profiling. */
	virtual void ProfilerResumeFunction() override;
	virtual void StartScopedEvent(const TCHAR* Text) override;
	virtual void EndScopedEvent() override;
	virtual void SetThreadName(const TCHAR* Name) override;

	virtual bool CanBeUsedOnAnyThread() const override;
	virtual bool CanBeUsedOnMultipleThreads() const override;

	virtual void OutputBookmark(const FString& Text); // override is intentionally missing because function might not be in engine source

protected:
	virtual void Serialize( const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category ) override;
};

/**
 * The public interface to this module.  In most cases, this interface is only public to sibling modules 
 * within this plugin.
 */
class FTracyPluginModule : public IModuleInterface
{

public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Singleton-like access to this module's interface.  This is just for convenience!
	 * Beware of calling this during the shutdown phase, though.  Your module might have been unloaded already.
	 *
	 * @return Returns singleton instance, loading the module on demand if needed
	 */
	static inline FTracyPluginModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FTracyPluginModule>("Tracy");
	}

	/**
	 * Checks to see if this module is loaded and ready.  It is only valid to call Get() if IsAvailable() returns true.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("Tracy");
	}
};