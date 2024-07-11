using System;
using System.Runtime.InteropServices;

namespace ManagedBass.ZipStream
{
    public static class Archive
    {
        const string DllName = "bass_zipstream";

        [DllImport(DllName)]
        static extern bool ARCHIVE_Create(out IntPtr instance);

        public static bool Create(out IntPtr instance)
        {
            return ARCHIVE_Create(out instance);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_GetFormatCount(IntPtr instance, out int count);

        public static bool GetFormatCount(IntPtr instance, out int count)
        {
            return ARCHIVE_GetFormatCount(instance, out count);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_GetFormat(IntPtr instance, out ArchiveFormat format, int index);

        public static bool GetFormat(IntPtr instance, out ArchiveFormat format, int index)
        {
            return ARCHIVE_GetFormat(instance, out format, index);
        }

        [DllImport(DllName, CharSet = CharSet.Unicode)]
        static extern bool ARCHIVE_Open(IntPtr instance, string fileName);

        public static bool Open(IntPtr instance, string fileName)
        {
            return ARCHIVE_Open(instance, fileName);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_GetEntryCount(IntPtr instance, out int count);

        public static bool GetEntryCount(IntPtr instance, out int count)
        {
            return ARCHIVE_GetEntryCount(instance, out count);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_GetEntry(IntPtr instance, out ArchiveEntry entry, int index);

        public static bool GetEntry(IntPtr instance, out ArchiveEntry entry, int index)
        {
            return ARCHIVE_GetEntry(instance, out entry, index);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_Close(IntPtr instance);

        public static void Close(IntPtr instance)
        {
            ARCHIVE_Close(instance);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_Release(IntPtr instance);

        public static void Release(IntPtr instance)
        {
            ARCHIVE_Release(instance);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_Cleanup();

        public static bool Cleanup()
        {
            return ARCHIVE_Cleanup();
        }

        [DllImport(DllName)]
        static extern void ARCHIVE_GetPassword(GetPasswordHandler handler);

        public static void GetPassword(GetPasswordHandler handler)
        {
            Handles.Pin(nameof(ARCHIVE_GetPassword), handler);
            ARCHIVE_GetPassword(handler);
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct ArchiveFormat
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 50)]
            public string name;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            public string extensions;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct ArchiveEntry
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            public string path;
            public long size;
        }

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Unicode)]
        public struct ArchivePassword
        {
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 260)]
            public string path;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 50)]
            public string password;
        }

        [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
        public delegate bool GetPasswordHandler(ref ArchivePassword password);
    }
}
