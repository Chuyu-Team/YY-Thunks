#pragma once
#include <Windows.h>

#pragma pack(push, 1)

namespace
{
    constexpr uint32_t kSMBIOS32 = '_MS_';

    struct DMIEntryPoint
    {
        BYTE anchor[5];
        BYTE checksum;
        WORD tableLength;
        DWORD tableAddress;
        WORD structureCount;
        BYTE bcdRevision;
    };

    struct SMBEntryPoint32
    {
        DWORD anchor /* = kSMBIOS32*/;
        BYTE checksum;
        BYTE entryPointLength;
        BYTE majorVersion;
        BYTE minorVersion;
        WORD maxStructureSize;
        BYTE entryPointRevision;
        BYTE formattedArea[5];
        DMIEntryPoint dmi;
    };

    struct RawSMBIOSData
    {
        BYTE    Used20CallingMethod;
        BYTE    SMBIOSMajorVersion;
        BYTE    SMBIOSMinorVersion;
        BYTE    DmiRevision;
        DWORD   Length;
        BYTE    SMBIOSTableData[];
    };

    struct SMBIOSHEADER
    {
        BYTE Type;
        BYTE Length;
        WORD Handle;
    };

    ///
    /// Text strings associated with a given SMBIOS structure are returned in the dmiStrucBuffer, appended directly after
    /// the formatted portion of the structure. This method of returning string information eliminates the need for
    /// application software to deal with pointers embedded in the SMBIOS structure. Each string is terminated with a null
    /// (00h) BYTE and the set of strings is terminated with an additional null (00h) BYTE. When the formatted portion of
    /// a SMBIOS structure references a string, it does so by specifying a non-zero string number within the structure's
    /// string-set. For example, if a string field contains 02h, it references the second string following the formatted portion
    /// of the SMBIOS structure. If a string field references no string, a null (0) is placed in that string field. If the
    /// formatted portion of the structure contains string-reference fields and all the string fields are set to 0 (no string
    /// references), the formatted section of the structure is followed by two null (00h) BYTES.
    ///
    typedef UINT8 SMBIOS_TABLE_STRING;

    ///
    /// Memory Device - Type Detail
    ///
    struct MEMORY_DEVICE_TYPE_DETAIL
    {
        UINT16    Reserved : 1;
        UINT16    Other : 1;
        UINT16    Unknown : 1;
        UINT16    FastPaged : 1;
        UINT16    StaticColumn : 1;
        UINT16    PseudoStatic : 1;
        UINT16    Rambus : 1;
        UINT16    Synchronous : 1;
        UINT16    Cmos : 1;
        UINT16    Edo : 1;
        UINT16    WindowDram : 1;
        UINT16    CacheDram : 1;
        UINT16    Nonvolatile : 1;
        UINT16    Registered : 1;
        UINT16    Unbuffered : 1;
        UINT16    LrDimm : 1;
    };

    ///
    /// Memory Device - Memory Operating Mode Capability
    ///
    union MEMORY_DEVICE_OPERATING_MODE_CAPABILITY
    {
        ///
        /// Individual bit fields
        ///
        struct
        {
            UINT16 Reserved : 1;   ///< Set to 0.
            UINT16 Other : 1;
            UINT16 Unknown : 1;
            UINT16 VolatileMemory : 1;
            UINT16 ByteAccessiblePersistentMemory : 1;
            UINT16 BlockAccessiblePersistentMemory : 1;
            UINT16 Reserved2 : 10;  ///< Set to 0.
        } Bits;
        ///
        /// All bit fields as a 16-bit value
        ///
        UINT16 Uint16;
    };

    ///
    /// Physical Memory Array (Type 16).
    ///
    /// This structure describes a collection of memory devices that operate
    /// together to form a memory address space.
    ///
    typedef struct SMBIOS_TABLE_TYPE16 : public SMBIOSHEADER
    {
        UINT8                     Location;                       ///< The enumeration value from MEMORY_ARRAY_LOCATION.
        UINT8                     Use;                            ///< The enumeration value from MEMORY_ARRAY_USE.
        UINT8                     MemoryErrorCorrection;          ///< The enumeration value from MEMORY_ERROR_CORRECTION.
        UINT32                    MaximumCapacity;
        UINT16                    MemoryErrorInformationHandle;
        UINT16                    NumberOfMemoryDevices;
        //
        // Add for smbios 2.7
        //
        UINT64                    ExtendedMaximumCapacity;
    } PhysicalMemoryArray;

    ///
    /// Memory Device (Type 17).
    ///
    /// This structure describes a single memory device that is part of
    /// a larger Physical Memory Array (Type 16).
    /// Note:  If a system includes memory-device sockets, the SMBIOS implementation
    /// includes a Memory Device structure instance for each slot, whether or not the
    /// socket is currently populated.
    ///
    typedef struct SMBIOS_TABLE_TYPE17 : public SMBIOSHEADER
    {
        UINT16                                    MemoryArrayHandle;
        UINT16                                    MemoryErrorInformationHandle;
        UINT16                                    TotalWidth;
        UINT16                                    DataWidth;
        UINT16                                    Size;
        UINT8                                     FormFactor;         ///< The enumeration value from MEMORY_FORM_FACTOR.
        UINT8                                     DeviceSet;
        SMBIOS_TABLE_STRING                       DeviceLocator;
        SMBIOS_TABLE_STRING                       BankLocator;
        UINT8                                     MemoryType;         ///< The enumeration value from MEMORY_DEVICE_TYPE.
        MEMORY_DEVICE_TYPE_DETAIL                 TypeDetail;
        UINT16                                    Speed;
        SMBIOS_TABLE_STRING                       Manufacturer;
        SMBIOS_TABLE_STRING                       SerialNumber;
        SMBIOS_TABLE_STRING                       AssetTag;
        SMBIOS_TABLE_STRING                       PartNumber;
        //
        // Add for smbios 2.6
        //
        UINT8                                     Attributes;
        //
        // Add for smbios 2.7
        //
        UINT32                                    ExtendedSize;
        //
        // Keep using name "ConfiguredMemoryClockSpeed" for compatibility
        // although this field is renamed from "Configured Memory Clock Speed"
        // to "Configured Memory Speed" in smbios 3.2.0.
        //
        UINT16                                    ConfiguredMemoryClockSpeed;
        //
        // Add for smbios 2.8.0
        //
        UINT16                                    MinimumVoltage;
        UINT16                                    MaximumVoltage;
        UINT16                                    ConfiguredVoltage;
        //
        // Add for smbios 3.2.0
        //
        UINT8                                      MemoryTechnology;  ///< The enumeration value from MEMORY_DEVICE_TECHNOLOGY
        MEMORY_DEVICE_OPERATING_MODE_CAPABILITY    MemoryOperatingModeCapability;
        SMBIOS_TABLE_STRING                        FirmwareVersion;
        UINT16                                     ModuleManufacturerID;
        UINT16                                     ModuleProductID;
        UINT16                                     MemorySubsystemControllerManufacturerID;
        UINT16                                     MemorySubsystemControllerProductID;
        UINT64                                     NonVolatileSize;
        UINT64                                     VolatileSize;
        UINT64                                     CacheSize;
        UINT64                                     LogicalSize;
        //
        // Add for smbios 3.3.0
        //
        UINT32                                     ExtendedSpeed;
        UINT32                                     ExtendedConfiguredMemorySpeed;
    } MemoryDevice;
}

#pragma pack(pop)
