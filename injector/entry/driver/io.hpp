#pragma once
#include <Windows.h>
#include <iostream>
#include <winternl.h>

extern "C" __int64 direct_device_control(
    HANDLE FileHandle,
    HANDLE Event,
    PIO_APC_ROUTINE ApcRoutine,
    PVOID ApcContext,
    PIO_STATUS_BLOCK IoStatusBlock,
    uint32_t IoControlCode,
    PVOID InputBuffer,
    uint32_t InputBufferLength,
    PVOID OutputBuffer,
    uint32_t OutputBufferLength);