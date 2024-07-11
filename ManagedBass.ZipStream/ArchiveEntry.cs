using System;
using System.Runtime.InteropServices;

namespace ManagedBass.ZipStream
{
    public static class ArchiveEntry
    {
        const string DllName = "bass_zipstream";

        public const long RESULT_OK = 0;

        public const long RESULT_CANCELLED = 676;

        public const long RESULT_PASSWORD_REQUIRED = 0x80070005L;

        [DllImport(DllName, CharSet = CharSet.Unicode)]
        static extern bool ARCHIVE_OpenEntry(string fileName, int index, out IntPtr instance);

        public static bool OpenEntry(string fileName, int index, out IntPtr instance)
        {
            return ARCHIVE_OpenEntry(fileName, index, out instance);
        }

        [DllImport(DllName)]
        static extern long ARCHIVE_GetEntryPosition(IntPtr instance);

        public static long GetEntryPosition(IntPtr instance)
        {
            return ARCHIVE_GetEntryPosition(instance);
        }

        [DllImport(DllName)]
        static extern long ARCHIVE_GetEntryLength(IntPtr instance);

        public static long GetEntryLength(IntPtr instance)
        {
            return ARCHIVE_GetEntryLength(instance);
        }

        [DllImport(DllName)]
        static extern long ARCHIVE_GetEntryAvailable(IntPtr instance);

        public static long GetEntryAvailable(IntPtr instance)
        {
            return ARCHIVE_GetEntryAvailable(instance);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_GetEntryResult(out long result, IntPtr instance);

        public static bool GetEntryResult(IntPtr instance, out long result)
        {
            return ARCHIVE_GetEntryResult(out result, instance);
        }

        [DllImport(DllName)]
        static extern int ARCHIVE_ReadEntry([In, Out] byte[] buffer, int length, IntPtr instance);

        public static int ReadEntry(IntPtr instance, byte[] buffer, int length)
        {
            return ARCHIVE_ReadEntry(buffer, length, instance);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_SeekEntry(long offset, IntPtr instance);

        public static bool SeekEntry(IntPtr instance, long offset)
        {
            return ARCHIVE_SeekEntry(offset, instance);
        }

        [DllImport(DllName)]
        static extern bool ARCHIVE_IsEOF(IntPtr instance);

        public static bool IsEOF(IntPtr instance)
        {
            return ARCHIVE_IsEOF(instance);
        }

        [DllImport(DllName)]
        static extern void ARCHIVE_CloseEntry(IntPtr instance);

        public static void CloseEntry(IntPtr instance)
        {
            ARCHIVE_CloseEntry(instance);
        }
    }
}
