# Tracy Profiler - Unreal Engine plugin
Unreal engine plugin for tracy profiler - https://github.com/wolfpld/tracy

Based on unreal external profiler api.

## Usage:
Run game/editor with **`-tracy`** cmd parameter.

## Currently supports:
* Frames.
* Start and stop scope events.
* Log messages.
* Bookmarks (tracy message with different color) - requires engine changes.

## How to install:
* Clone repository and copy Tracy folder in your unreal project's Plugins.
* In Tracy/Source/Tracy/Tracy.Build.cs you can modify tracy macros (TRACY_ON_DEMAND, TRACY_NO_CALLSTACK, TRACY_CALLSTACK)
* Copy files necessary from Tracy profiler into Tracy/Source/TracyLib
    * From your clone of Tracy profiler copy `public` folder into Tracy/Source/TracyLib:
    * There might be some fixes necessary in Tracy files:
```diff
# client/TracyProfiler.cpp
-        ptr += sprintf( ptr, "OS: Windows %i.%i.%i\n", ver.dwMajorVersion, ver.dwMinorVersion, ver.dwBuildNumber );
+        ptr += sprintf( ptr, "OS: Windows %i.%i.%i\n", (int)ver.dwMajorVersion, (int)ver.dwMinorVersion, (int)ver.dwBuildNumber );
```
* Compile your unreal project.

## How to enable bookmarks (requires engine changes):
Create function for handling bookmarks in class FExternalProfiler
```diff
# Source/Runtime/Core/Public/ProfilingDebugging/ExternalProfiler.h
class CORE_API FExternalProfiler : public IModularFeature
{
    ...
+	template <typename FmtType, typename... Types>
+	void OutputBookmarkEvent(const FmtType& Fmt, Types... Args)
+	{
+		OutputBookmark(FString::Printf(Fmt, Args...));
+	}
+	virtual void OutputBookmark(const FString& Text) {};
```

Modify TRACE_BOOKMARK macro in MiscTrace.h
```diff
# Source/Runtime/Core/Public/ProfilingDebugging/MiscTrace.h
+#include "ProfilingDebugging/ExternalProfiler.h"
...
-#define TRACE_BOOKMARK(Format, ...) \
-	static bool PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__); \
-	if (!PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__)) \
-	{ \
-		FMiscTrace::OutputBookmarkSpec(&PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__), __FILE__, __LINE__, Format); \
-		PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__) = true; \
-	} \
-	FMiscTrace::OutputBookmark(&PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__), ##__VA_ARGS__);
+#if UE_EXTERNAL_PROFILING_ENABLED
+#define TRACE_BOOKMARK(Format, ...) \
+	static bool PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__); \
+	if (!PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__)) \
+	{ \
+		FMiscTrace::OutputBookmarkSpec(&PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__), __FILE__, __LINE__, Format); \
+		PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__) = true; \
+	} \
+	FMiscTrace::OutputBookmark(&PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__), ##__VA_ARGS__); \
+	if (FActiveExternalProfilerBase::GetActiveProfiler()) FActiveExternalProfilerBase::GetActiveProfiler()->OutputBookmarkEvent+(Format, ##__VA_ARGS__);
+
+#else
+
+#define TRACE_BOOKMARK(Format, ...) \
+	static bool PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__); \
+	if (!PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__)) \
+	{ \
+		FMiscTrace::OutputBookmarkSpec(&PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__), __FILE__, __LINE__, Format); \
+		PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__) = true; \
+	} \
+	FMiscTrace::OutputBookmark(&PREPROCESSOR_JOIN(__BookmarkPoint, __LINE__), ##__VA_ARGS__);
+
+#endif
```