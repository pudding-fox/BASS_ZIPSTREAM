using NUnit.Framework;
using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace ManagedBass.ZipStream.Tests
{
    public static class Utils
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(Utils).Assembly.Location);

        public static void Iterations(Action<int> action, int count)
        {
            for (var a = 0; a < count; a++)
            {
                action(a);
            }
        }

        public static void ParallelIterations(Action<int> action, int count)
        {
            Parallel.For(0, count, action);
        }

        public static int GetEntryCount(string archiveName)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            if (!Archive.Open(archive, fileName))
            {
                Assert.Fail("Failed to open archive.");
            }

            try
            {
                var count = default(int);
                if (!Archive.GetEntryCount(archive, out count))
                {
                    Assert.Fail("Failed to get entry count.");
                }
                return count;
            }
            finally
            {
                Archive.Release(archive);
            }
        }

        public static void GetEntryType(string archiveName, int index, out bool isFile)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            if (!Archive.Open(archive, fileName))
            {
                Assert.Fail("Failed to open archive.");
            }

            try
            {
                var entry = default(Archive.ArchiveEntry);
                if (!Archive.GetEntry(archive, out entry, index))
                {
                    Assert.Fail("Failed to get entry.");
                }
                isFile = entry.size > 0;
            }
            finally
            {
                Archive.Release(archive);
            }
        }

        public static int GetEntryIndex(string archiveName, string entryPath)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            if (!Archive.Open(archive, fileName))
            {
                Assert.Fail("Failed to open archive.");
            }

            try
            {
                return GetEntryIndex(archive, entryPath);
            }
            finally
            {
                Archive.Release(archive);
            }
        }

        public static int GetEntryIndex(IntPtr archive, string entryPath)
        {
            var count = default(int);
            if (!Archive.GetEntryCount(archive, out count))
            {
                Assert.Fail("Failed to get entry count.");
            }

            for (var a = 0; a < count; a++)
            {
                var entry = default(Archive.ArchiveEntry);
                if (!Archive.GetEntry(archive, out entry, a))
                {
                    Assert.Fail("Failed to get entry.");
                }
                if (string.Equals(entry.path, entryPath, StringComparison.OrdinalIgnoreCase))
                {
                    return a;
                }
            }
            Assert.Fail("The expected entry was not found.");
            throw new NotImplementedException();
        }

        public static int GetEntryHashCode(IntPtr entry)
        {
            var hashCode = default(int);
            var buffer = new byte[10240];
            do
            {
                var count = ArchiveEntry.ReadEntry(entry, buffer, buffer.Length);
                if (count <= 0)
                {
                    break;
                }
                for (var a = 0; a < count; a++)
                {
                    unchecked
                    {
                        hashCode += buffer[a];
                    }
                }
            } while (true);
            return Math.Abs(hashCode);
        }

        public static long GetEntryResult(IntPtr entry)
        {
            do
            {
                var result = default(long);
                if (ArchiveEntry.GetEntryResult(entry, out result))
                {
                    return result;
                }
                Thread.Yield();
                Thread.Sleep(5000);
                Thread.Yield();
            } while (true);
        }

        public static class PasswordHandler
        {
            public static string FileName { get; private set; }

            public static string Password { get; private set; }

            public static int Delay { get; private set; }

            public static void Set(string fileName, string password, int delay)
            {
                FileName = fileName;
                Password = password;
                Delay = delay;
                Archive.GetPassword(Get);
            }

            public static bool Get(ref Archive.ArchivePassword password)
            {
                if (!string.Equals(password.path, FileName, StringComparison.OrdinalIgnoreCase))
                {
                    return false;
                }
                if (Delay > 0)
                {
                    Thread.Yield();
                    Thread.Sleep(Delay);
                    Thread.Yield();
                }
                password.password = Password;
                return !string.IsNullOrEmpty(password.password);
            }

            public static void Reset()
            {
                Archive.GetPassword(null);
                FileName = string.Empty;
                Password = string.Empty;
                Delay = 0;
            }
        }
    }
}
