using System.Runtime.InteropServices;

namespace ManagedBass.ZipStream
{
    public static class ArchiveError
    {
        const string DllName = "bass_zipstream";

        public const int E_FILE_NOT_FOUND = 0x2;

        public const int E_NOT_A_FILE = 0x9000100;
        public const int E_TIMEOUT = 0x9000200;
        public const int E_PASSWORD_REQUIRED = unchecked((int)0x80070005);

        [DllImport(DllName)]
        static extern int ARCHIVE_GetLastError();

        public static int GetLastError()
        {
            return ARCHIVE_GetLastError();
        }
    }
}
