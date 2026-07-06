## API Usage Guide

This C++ DLL provides a set of export functions (via `__declspec(dllexport)`) designed for Windows-based golf simulator applications to interface with the vision tracking sensor over Ethernet.

### 1. Prerequisites
To use this DLL in your application (e.g., C# / .NET / C++), ensure you target the Windows platform and have the necessary Winsock dependencies configured.

---

### 2. Exported Functions (API Reference)

#### `bool ConnectToIP(char* szIp)`
Establishes a TCP/IP or UDP connection to the golf sensor using the specified IP address.
* **Arguments:** `szIp` - The target IP address of the vision sensor (e.g., `"192.168.0.10"`).
* **Returns:** `true` if the connection setup is successfully initiated, `false` otherwise.

#### `int Getstatus()` / `int Setstatus()`
Retrieves or modifies the current tracking/streaming status of the sensor.

#### `void requestFrame(unsigned __int32 frameNo)`
Requests a specific image/video frame from the golf sensor.

#### `int GetImg(byte* buffer, ...)`
Copies the real-time frame buffer from the vision sensor into the allocated application memory buffer.

#### `void requestAllStop()`
Sends a command to the sensor to halt all current actions, image streams, or background tracking processes.

#### `void FreeEthernet()`
Safely terminates the active socket connections, clears internal buffers/render targets, and cleans up network resources (`WSACleanup`).

---

### 3. Integration Example (C# / .NET)

If you are developing a golf simulator wrapper application in C#, you can import and invoke the DLL functions using `DllImport` as shown below:

```csharp
using System;
using System.Runtime.InteropServices;

public class GolfSensorClient
{
    // Adjust the DLL name matching your compiled binary output
    private const string DllPath = "EthernetConnectDll.dll";

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern bool ConnectToIP(string szIp);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void requestFrame(uint frameNo);

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void requestAllStop();

    [DllImport(DllPath, CallingConvention = CallingConvention.Cdecl)]
    public static extern void FreeEthernet();

    public void StartSensorSession()
    {
        string sensorIp = "192.168.1.100"; // Example Sensor IP
        
        Console.WriteLine($"Connecting to Golf Sensor at {sensorIp}...");
        bool isConnected = ConnectToIP(sensorIp);

        if (isConnected)
        {
            Console.WriteLine("Connection initialized successfully.");
            
            // Request frame sequence for ball tracking
            requestFrame(0);
        }
        else
        {
            Console.WriteLine("Failed to establish connection.");
        }
    }

    public void CloseSession()
    {
        Console.WriteLine("Stopping sensor streams and releasing resources...");
        requestAllStop();
        FreeEthernet();
    }
}
