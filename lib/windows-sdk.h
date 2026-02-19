#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// Core WinRT headers.
// https://learn.microsoft.com/en-us/windows/win32/api/_winrt/

#include <winrt/base.h>

#undef GetCurrentTime

#include <winrt/windows.foundation.h>
#include <winrt/windows.management.deployment.h>

using namespace winrt;

using namespace Windows::Foundation;
using namespace Windows::Management::Deployment;
