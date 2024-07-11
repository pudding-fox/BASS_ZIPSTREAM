using System.Collections.Concurrent;
using System.Runtime.InteropServices;

internal static class Handles
{
    private static ConcurrentDictionary<string, GCHandle> Pinned = new ConcurrentDictionary<string, GCHandle>();

    public static bool Pin(string name, object value)
    {
        var handle = default(GCHandle);
        if (Pinned.TryRemove(name, out handle))
        {
            if (handle.IsAllocated)
            {
                handle.Free();
            }
        }
        //We can't actually use GCHandleType.Pinned.
        //I'm sure we used to do this?
        handle = GCHandle.Alloc(value, GCHandleType.Normal);
        if (Pinned.TryAdd(name, handle))
        {
            return true;
        }
        handle.Free();
        return false;
    }
}
