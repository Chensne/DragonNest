#ifndef _FMOD_MMDEVICEAPI_H
#define _FMOD_MMDEVICEAPI_H

#include "fmod_settings.h"

#ifdef FMOD_SUPPORT_WASAPI

// Audio data flow direction
typedef enum 
{
    eRender,
	eCapture,
	eAll,
    EDataFlow_enum_count
} EDataFlow;

// Role assigned to an audio endpoint
typedef enum
{
    eConsole,
    eMultimedia,
    eCommunications,
    ERole_enum_count
} ERole;

// Identifier for a property
#ifndef PROPERTYKEY_DEFINED
#define PROPERTYKEY_DEFINED
typedef struct
{
    FMOD_GUID  category;
    DWORD      propertyID;
} PROPERTYKEY;
#endif

// GUIDs
static const GUID  CLSID_MMDeviceEnumerator  = { 0xbcde0395, 0xe52f, 0x467c, { 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e }};
static const GUID  IID_IMMDeviceEnumerator   = { 0xa95664d2, 0x9614, 0x4f35, { 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6 }};
static const GUID  IID_IMMEndpoint           = { 0x1be09788, 0x6894, 0x4089, { 0x85, 0x86, 0x9a, 0x2a, 0x6c, 0x26, 0x5a, 0xc5 }};

// Property keys
static const PROPERTYKEY  PKEY_Device_DeviceDesc             = { { 0xa45c254e, 0xdf1c, 0x4efd, { 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0 } }, 2 };
static const PROPERTYKEY  PKEY_DeviceInterface_FriendlyName  = { { 0x026e516e, 0xb814, 0x414b, { 0x83, 0xcd, 0x85, 0x6d, 0x6f, 0xef, 0x48, 0x22 } }, 2 };

// Helper defines
#define REFPROPERTYKEY const PROPERTYKEY &
#define REFPROPVARIANT const PROPVARIANT &

// Error defines
#define E_NOTFOUND          HRESULT_FROM_WIN32(ERROR_NOT_FOUND)
#define E_UNSUPPORTED_TYPE  HRESULT_FROM_WIN32(ERROR_UNSUPPORTED_TYPE)

// Device states
#define DEVICE_STATE_ACTIVE      0x00000001
#define DEVICE_STATE_DISABLED    0x00000002
#define DEVICE_STATE_NOTPRESENT  0x00000004
#define DEVICE_STATE_UNPLUGGED   0x00000008
#define DEVICE_STATEMASK_ALL     0x0000000f


// Property variant management
#define PropVariantInit(pvar) FMOD_memset((pvar), 0, sizeof(PROPVARIANT))
WINOLEAPI PropVariantClear ( PROPVARIANT * pvar );

// Provides methods for enumerating, getting, and setting property values
struct IPropertyStore : public IUnknown
{
  public:
    virtual HRESULT F_STDCALL GetCount  (DWORD *cProps) = 0;
    virtual HRESULT F_STDCALL GetAt     (DWORD iProp, PROPERTYKEY *pkey) = 0;
    virtual HRESULT F_STDCALL GetValue  (REFPROPERTYKEY key, PROPVARIANT *pv) = 0;
    virtual HRESULT F_STDCALL SetValue  (REFPROPERTYKEY key, REFPROPVARIANT propvar) = 0;
    virtual HRESULT F_STDCALL Commit    (void) = 0;
};

// Provices notification when an endpoint state has changed
struct IMMNotificationClient : public IUnknown
{
  public:
    virtual HRESULT F_STDCALL OnDeviceStateChanged    (LPCWSTR pwstrDeviceId, DWORD dwNewState) = 0;
    virtual HRESULT F_STDCALL OnDeviceAdded           (LPCWSTR pwstrDeviceId) = 0;
    virtual HRESULT F_STDCALL OnDeviceRemoved         (LPCWSTR pwstrDeviceId) = 0;
    virtual HRESULT F_STDCALL OnDefaultDeviceChanged  (EDataFlow flow, ERole role, LPCWSTR pwstrDefaultDeviceId) = 0;
    virtual HRESULT F_STDCALL OnPropertyValueChanged  (LPCWSTR pwstrDeviceId, const PROPERTYKEY key) = 0;
};

// Encapsulate the generic features of a multimedia device resource
struct IMMDevice : public IUnknown
{
  public:
    virtual HRESULT F_STDCALL Activate           (REFIID iid, DWORD dwClsCtx, PROPVARIANT *pActivationParams, void **ppInterface) = 0;
    virtual HRESULT F_STDCALL OpenPropertyStore  (DWORD stgmAccess, IPropertyStore **ppProperties) = 0;
    virtual HRESULT F_STDCALL GetId              (LPWSTR *ppstrId) = 0;
    virtual HRESULT F_STDCALL GetState           (DWORD *pdwState) = 0;
};

// An audio endpoint device
struct IMMEndpoint : public IUnknown
{
  public:
    virtual HRESULT F_STDCALL GetDataFlow  (EDataFlow *pDataFlow) = 0;
};

// A collection of multimedia device resources
struct IMMDeviceCollection : public IUnknown
{
  public:
    virtual HRESULT F_STDCALL GetCount  (UINT *pcDevices) = 0;
    virtual HRESULT F_STDCALL Item      (UINT nDevice, IMMDevice **ppDevice) = 0;
};

// Provides methods for enumerating multimedia device resources
struct IMMDeviceEnumerator : public IUnknown
{
  public:
    virtual HRESULT F_STDCALL EnumAudioEndpoints                      (EDataFlow dataFlow, DWORD dwStateMask, IMMDeviceCollection **ppDevices) = 0;
    virtual HRESULT F_STDCALL GetDefaultAudioEndpoint                 (EDataFlow dataFlow, ERole role, IMMDevice **ppEndpoint) = 0;
    virtual HRESULT F_STDCALL GetDevice                               (LPCWSTR pwstrId, IMMDevice **ppDevice) = 0;    
    virtual HRESULT F_STDCALL RegisterEndpointNotificationCallback    (IMMNotificationClient *pClient) = 0;
    virtual HRESULT F_STDCALL UnregisterEndpointNotificationCallback  (IMMNotificationClient *pClient) = 0;
};

#endif  /* FMOD_SUPPORT_WASAPI */

#endif  /* _FMOD_MMDEVICEAPI_H */