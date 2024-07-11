# BASS_ZIPSTREAM

A BASS plugin which creates a stream from an archive entry.

bass.dll is required for native projects.
ManagedBass is required for .NET projects.

A simple example;
```c#
var sourceChannel = BassZipStream.CreateStream("Music.zip", 0); //Create a stream for the first file in Music.zip.

Bass.ChannelPlay(sourceChannel);

while (Bass.ChannelIsActive(sourceChannel) == PlaybackState.Playing)
{
    Thread.Sleep(1000);
}

Bass.StreamFree(sourceChannel);
```

Some archive reading framework is provided;
```c#
var archive = default(IntPtr);
Archive.Create(out archive);
Archive.Open(archive, "Music.zip");

var count = default(int);
Archive.GetEntryCount(archive, out count)
for (var a = 0; a < count; a++)
{
    var entry = default(Archive.ArchiveEntry);
    Archive.GetEntry(archive, out entry, index)
    Console.WriteLine("Path: {0}, Size: {1}", entry.path, entry.size);
}

Archive.Release(archive);
```

Extract an entry;
```c#
var entry = default(IntPtr);
ArchiveEntry.OpenEntry("Music.zip", index, out entry);

var buffer = new byte[10240];
do
{
    var count = ArchiveEntry.ReadEntry(entry, buffer, buffer.Length);
    if (count <= 0)
    {
        break;
    }
    //Do something with the data in buffer.
} while (true);

ArchiveEntry.CloseEntry(entry);
```

Password protected archives can be used, register the handler;
```c#
Archive.GetPassword(GetPassword);

public static bool GetPassword(ref Archive.ArchivePassword password)
{
    password = PromptUserForPassword(password.path);
}
```