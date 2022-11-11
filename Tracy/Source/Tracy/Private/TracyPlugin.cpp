// Copyright Epic Games, Inc. All Rights Reserved.

#include "TracyPlugin.h"
#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"

#include "ProfilingDebugging/ExternalProfiler.h"
#include "Features/IModularFeatures.h"
#include "Templates/UniquePtr.h"
#include "Misc/CommandLine.h"

#if UE_EXTERNAL_PROFILING_ENABLED

#ifdef TRACY_ENABLE
/** Per thread TArray for all active scopes */
struct FTracyPerThreadHandleMap : public TThreadSingleton<FTracyPerThreadHandleMap>
{
	TArray<TracyCZoneCtx> TracyContextStack;
};
#endif

FTracyExternalProfiler::FTracyExternalProfiler()
{
	// Register as a modular feature
	IModularFeatures::Get().RegisterModularFeature(FExternalProfiler::GetFeatureName(), this);

	GLog->AddOutputDevice(this);
}

FTracyExternalProfiler::~FTracyExternalProfiler()
{
	IModularFeatures::Get().UnregisterModularFeature(FExternalProfiler::GetFeatureName(), this);

	// At shutdown, GLog may already be null
	if( GLog != NULL )
	{
		GLog->RemoveOutputDevice(this);
	}
}

void FTracyExternalProfiler::FrameSync()
{
#ifdef TRACY_ENABLE
	FrameMark;
#endif
}

const TCHAR* FTracyExternalProfiler::GetProfilerName() const
{
	return TEXT("Tracy");
}

void FTracyExternalProfiler::ProfilerPauseFunction()
{
}

void FTracyExternalProfiler::ProfilerResumeFunction()
{
}

void FTracyExternalProfiler::StartScopedEvent(const TCHAR* Text)
{
#ifdef TRACY_ENABLE
	FTracyPerThreadHandleMap& ThreadContext = FTracyPerThreadHandleMap::Get();

	// Activate new event.
	TracyCZoneCtx& TracyZoneCtx = ThreadContext.TracyContextStack.AddZeroed_GetRef();
#if defined TRACY_HAS_CALLSTACK && defined TRACY_CALLSTACK
	TracyZoneCtx = ___tracy_emit_zone_begin_alloc_callstack( ___tracy_alloc_srcloc_name(__LINE__, __FILE__, strlen( __FILE__ ), __FUNCTION__, strlen( __FUNCTION__ ), TCHAR_TO_ANSI(Text), strlen( TCHAR_TO_ANSI(Text) )), TRACY_CALLSTACK, true );
#else
	TracyZoneCtx = ___tracy_emit_zone_begin_alloc( ___tracy_alloc_srcloc_name(__LINE__, __FILE__, strlen( __FILE__ ), __FUNCTION__, strlen( __FUNCTION__ ), TCHAR_TO_ANSI(Text), strlen( TCHAR_TO_ANSI(Text) )), true );
#endif
#endif
}

void FTracyExternalProfiler::EndScopedEvent()
{
#ifdef TRACY_ENABLE
	// Deactivate last event.
	FTracyPerThreadHandleMap& ThreadContext = FTracyPerThreadHandleMap::Get();
	if (ThreadContext.TracyContextStack.Num() > 0)
	{
		TracyCZoneEnd(ThreadContext.TracyContextStack.Pop(false));
	}
#endif
}

void FTracyExternalProfiler::SetThreadName(const TCHAR* Name)
{
#ifdef TRACY_ENABLE
	tracy::SetThreadName(TCHAR_TO_ANSI(Name));
#endif
}

bool FTracyExternalProfiler::CanBeUsedOnAnyThread() const
{
	return true;
}

bool FTracyExternalProfiler::CanBeUsedOnMultipleThreads() const
{
	return true;
}

void FTracyExternalProfiler::OutputBookmark(const FString& Text)
{
#ifdef TRACY_ENABLE
	TracyMessageC(TCHAR_TO_ANSI(*Text), Text.Len(), FColor(64,192,64).DWColor());
#endif
}

void FTracyExternalProfiler::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category)
{
#ifdef TRACY_ENABLE
	switch (Verbosity)
	{
	case ELogVerbosity::Fatal:
	case ELogVerbosity::Error:
		TracyMessageC(TCHAR_TO_ANSI(V), strlen( TCHAR_TO_ANSI(V) ), FColor(192,64,64).DWColor());
		break;
	case ELogVerbosity::Warning:
		TracyMessageC(TCHAR_TO_ANSI(V), strlen( TCHAR_TO_ANSI(V) ), FColor(192,192,64).DWColor());
		break;
	default:
		TracyMessage(TCHAR_TO_ANSI(V), strlen(TCHAR_TO_ANSI(V)));
		break;
	}
#endif
}

#endif	// UE_EXTERNAL_PROFILING_ENABLED

void FTracyPluginModule::StartupModule()
{
#ifdef TRACY_ENABLE
	if (FParse::Param(FCommandLine::Get(), TEXT("Tracy"))) // Use -Tracy parameter to enable profiler.
	{
		static TUniquePtr<FTracyExternalProfiler> ProfilerTracy = MakeUnique<FTracyExternalProfiler>();
	}
#endif
}

void FTracyPluginModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FTracyPluginModule, Tracy)