#ifndef MOCK_MMST_TABLE_LIB_H
#define MOCK_MMST_TABLE_LIB_H

#include <Library/GoogleTestLib.h>
#include <Library/FunctionMockLib.h>
extern "C" {
#include <Uefi.h>
#include <Library/MmServicesTableLib.h>
}

struct MockMmStTableLib {
  MOCK_INTERFACE_DECLARATION (MockMmStTableLib);

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    gMmst_MmLocateProtocol,
    (IN  EFI_GUID  *Protocol,
     IN  VOID      *Registration  OPTIONAL,
     OUT VOID      **Interface)
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    gMmst_MmInstallProtocolInterface,
    (IN OUT EFI_HANDLE      *UserHandle,
     IN EFI_GUID            *Protocol,
     IN EFI_INTERFACE_TYPE  InterfaceType,
     IN VOID                *Interface)
    );

  MOCK_FUNCTION_DECLARATION (
    EFI_STATUS,
    gMmst_MmRegisterProtocolNotify,
    (IN  CONST EFI_GUID    *Protocol,
     IN  EFI_MM_NOTIFY_FN  Function,
     OUT VOID              **Registration)
    );
};

#endif // MOCK_MMST_TABLE_LIB_H
