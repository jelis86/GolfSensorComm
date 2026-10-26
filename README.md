# GolfSensorComm

A Windows C++ DLL for retrieving golf vision sensor frames over Ethernet, with native exports for C++ and C# / Unity integration.

## Source map

- `EthernetConnectDll.sln`: Visual Studio solution.
- `EthernetConnectDll/header.h`: exported declarations and acquisition states.
- `EthernetConnectDll/dll_main.cpp`: acquisition, frame buffers, and cleanup.
- `EthernetConnectDll/Ethernet.cpp`: Winsock TCP transport.
- `EthernetConnectDll/Dib.*` and `DIBSPRITE.*`: bitmap utilities.

## Build preparation

Use Windows, Visual Studio 2022 with Desktop development with C++, MSVC v143, and a Windows 10 SDK. Compatible sensor hardware is needed for acquisition; no emulator is included.

Open the solution and start with **Release / x64**. Review these checked-in settings before building:

1. Add `CREATEDLL_EXPORTS` to the DLL project's preprocessor definitions. The header uses this macro for exports, while the project defines `ETHERNETCONNECTDLL_EXPORTS` instead.
2. Change the x64 output directory from the developer-specific Unity path to a local path such as `$(SolutionDir)bin\$(Platform)\$(Configuration)\`.
3. Match the DLL architecture to the consuming application.

Debug / x64 references FFmpeg libraries under `C:\ffmpeg`; Release / x64 does not list these libraries. Win32 configurations reference a `pch.h` file that is not included. Those configurations require additional setup. Build and hardware execution were not verified during this documentation review.

## Native API

The transport uses **TCP port 4000**. Functions are declared with C linkage in `header.h`.

| Function | Behavior |
| --- | --- |
| `bool ConnectToIP(char* szIp)` | Allocates render targets and attempts a connection to the sensor IPv4 address; returns the connection state checked by the implementation. |
| `int Getstatus()` | Returns the acquisition state. |
| `int Setstatus()` | Resets state to `E_WAIT`; takes no arguments. |
| `void requestFrame(unsigned __int32 frameNo)` | Starts recorded-frame acquisition; use frame indices 0–63. |
| `int GetImg(byte* buffer, int bufferSize)` | Copies 64 frames from camera 0; returns the byte count, or `-1` if the buffer is too small. |
| `void requestAllStop()` | Sends the sensor disable command. |
| `void FreeEthernet()` | Releases render targets and network resources, removes callbacks, and resets state. |
| `void SetName(char* szpath, char* szfolderName)` | Stores output path settings; does not itself save frames. |
| `char* GetFolderName()` | Returns an internal buffer; automatic folder-name assignment is commented out in the current acquisition path. |

States: `E_SUCCESS = 0`, `E_FAILED = 1`, `E_WAIT = 2`, `E_COMPLETE = 3`.

## C# interop declarations

The native C++ `bool` is one byte and needs explicit return marshalling. Deploy the built DLL where the application can load it.

```csharp
using System.Runtime.InteropServices;

internal static class GolfSensorNative
{
    private const string Dll = "EthernetConnectDll.dll";

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    internal static extern bool ConnectToIP(string sensorIp);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int Getstatus();

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void requestFrame(uint frameNo);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int GetImg([Out] byte[] buffer, int bufferSize);

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void requestAllStop();

    [DllImport(Dll, CallingConvention = CallingConvention.Cdecl)]
    internal static extern void FreeEthernet();
}
```

## Acquisition lifecycle and constraints

Connect, request frame 0, and wait for `E_COMPLETE` with an application-defined timeout before copying frames. Allocate `640 * 480 * 64` bytes (**19,660,800 bytes**) for `GetImg`. Stop acquisition before cleanup, and arrange cleanup on failure as well as normal shutdown.

`GetImg` assumes 64 monochrome 640 × 480 frames from camera 0, even though other code can resize render targets. It does not validate a null pointer or acquisition completion. Global state and buffers mean callers should serialize access and avoid freeing resources during acquisition. This is not a general-purpose multi-sensor client.
