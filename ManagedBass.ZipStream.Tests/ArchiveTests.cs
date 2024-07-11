using NUnit.Framework;
using System;
using System.IO;

namespace ManagedBass.ZipStream.Tests
{
    [TestFixture]
    public class ArchiveTests
    {
        private static readonly string Location = Path.GetDirectoryName(typeof(ArchiveTests).Assembly.Location);

        [SetUp]
        public void SetUp()
        {
            Assert.IsTrue(Loader.Load("bass"));
            Assert.IsTrue(BassZipStream.Load());
        }

        [TearDown]
        public void TearDown()
        {
            BassZipStream.Unload();
        }

        [TestCase("7z", "7z")]
        [TestCase("Iso", "iso,img")]
        [TestCase("Rar", "rar,r00")]
        [TestCase("Tar", "tar,ova")]
        [TestCase("Zip", "zip,z01,zipx,jar,xpi,odt,ods,docx,xlsx,epub,ipa,apk,appx")]
        public void Test001(string name, string extensions)
        {
            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            try
            {
                var count = default(int);
                if (!Archive.GetFormatCount(archive, out count))
                {
                    Assert.Fail("Failed to get format count.");
                }

                for (var a = 0; a < count; a++)
                {
                    var format = default(Archive.ArchiveFormat);
                    if (!Archive.GetFormat(archive, out format, a))
                    {
                        Assert.Fail("Failed to get format.");
                    }
                    if (!string.Equals(format.name, name, StringComparison.OrdinalIgnoreCase))
                    {
                        continue;
                    }
                    if (!string.Equals(format.extensions, extensions, StringComparison.OrdinalIgnoreCase))
                    {
                        continue;
                    }
                    return;
                }
                Assert.Fail("Expected format was not found.");
            }
            finally
            {
                Archive.Release(archive);
            }
        }

        [TestCase("Music.zip", "Gift\\01 Smile.flac")]
        [TestCase("Music.zip", "Gift\\02 Again & Again.flac")]
        [TestCase("Music.zip", "Gift\\03 Emotional Times.flac")]
        public void Test002(string archiveName, string entryPath)
        {
            var fileName = Path.Combine(Location, "Media", archiveName);

            var archive = default(IntPtr);
            if (!Archive.Create(out archive))
            {
                Assert.Fail("Failed to create archive.");
            }

            try
            {
                if (!Archive.Open(archive, fileName))
                {
                    Assert.Fail("Failed to open archive.");
                }

                Utils.GetEntryIndex(archive, entryPath);
            }
            finally
            {
                Archive.Release(archive);
            }
        }
    }
}
