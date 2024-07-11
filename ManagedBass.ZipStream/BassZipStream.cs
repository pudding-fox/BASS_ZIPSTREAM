using System.IO;
using System.Runtime.InteropServices;

namespace ManagedBass.ZipStream
{
    public static class BassZipStream
    {
        const string DllName = "bass_zipstream";

        /// <summary>
        /// //Buffer 20% before creating stream.
        /// </summary>
        public const int DEFAULT_BUFFER_MIN = 20;
        public const int DEFAULT_BUFFER_TIMEOUT = 1000;
        public const bool DEFAULT_DOUBLE_BUFFER = false;

        public static int BufferMin
        {
            get
            {
                var value = default(int);
                if (BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute.BufferMin, out value))
                {
                    return value;
                }
                return DEFAULT_BUFFER_MIN;
            }
            set
            {
                BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute.BufferMin, value);
            }
        }

        public static int BufferTimeout
        {
            get
            {
                var value = default(int);
                if (BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute.BufferTimeout, out value))
                {
                    return value;
                }
                return DEFAULT_BUFFER_TIMEOUT;
            }
            set
            {
                BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute.BufferTimeout, value);
            }
        }

        public static bool DoubleBuffer
        {
            get
            {
                var value = default(bool);
                if (BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute.DoubleBuffer, out value))
                {
                    return value;
                }
                return DEFAULT_DOUBLE_BUFFER;
            }
            set
            {
                BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute.DoubleBuffer, value);
            }
        }

        public static int Module = 0;

        public static bool Load(string folderName = null)
        {
            if (Module == 0)
            {
                var fileName = default(string);
                if (!string.IsNullOrEmpty(folderName))
                {
                    fileName = Path.Combine(folderName, DllName);
                }
                else
                {
                    fileName = Path.Combine(Loader.FolderName, DllName);
                }
                Module = Bass.PluginLoad(string.Format("{0}.{1}", fileName, Loader.Extension));
            }
            return Module != 0;
        }

        public static bool Unload()
        {
            if (Module != 0)
            {
                if (!Bass.PluginFree(Module))
                {
                    return false;
                }
                Module = 0;
            }
            return true;
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute Attrib, out bool Value);

        public static bool GetConfig(BassZipStreamAttribute Attrib, out bool Value)
        {
            return BASS_ZIPSTREAM_GetConfig(Attrib, out Value);
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_GetConfig(BassZipStreamAttribute Attrib, out int Value);

        public static bool GetConfig(BassZipStreamAttribute Attrib, out int Value)
        {
            return BASS_ZIPSTREAM_GetConfig(Attrib, out Value);
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute Attrib, bool Value);

        public static bool SetConfig(BassZipStreamAttribute Attrib, bool Value)
        {
            return BASS_ZIPSTREAM_SetConfig(Attrib, Value);
        }

        [DllImport(DllName)]
        static extern bool BASS_ZIPSTREAM_SetConfig(BassZipStreamAttribute Attrib, int Value);

        public static bool SetConfig(BassZipStreamAttribute Attrib, int Value)
        {
            return BASS_ZIPSTREAM_SetConfig(Attrib, Value);
        }

        [DllImport(DllName, CharSet = CharSet.Unicode)]
        static extern int BASS_ZIPSTREAM_StreamCreateFile(bool mem, string file, int index, long offset, long length, BassFlags flags);

        public static int CreateStream(string File, int Index, long Offset = 0, long Length = 0, BassFlags Flags = BassFlags.Default)
        {
            return BASS_ZIPSTREAM_StreamCreateFile(false, File, Index, Offset, Length, Flags | BassFlags.Unicode);
        }
    }

    public enum BassZipStreamAttribute : byte
    {
        None = 0,
        BufferMin = 1,
        BufferTimeout = 2,
        DoubleBuffer = 3
    }
}
