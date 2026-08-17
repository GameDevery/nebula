using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Util
{
    public unsafe struct String
    {
        public IntPtr heapBuffer;
        public fixed byte localBuffer[16];
        public UIntPtr strLen;
        public UIntPtr heapBufferSize;
    }

    public class StringMarshaler : ICustomMarshaler {

		private static StringMarshaler Instance = new StringMarshaler ();

		public static ICustomMarshaler GetInstance (string s)
		{
			return Instance;
		}

		public void CleanUpManagedData (object o)
		{
		}

		public void CleanUpNativeData (IntPtr pNativeData)
		{
            if (pNativeData == IntPtr.Zero)
                return;

            IntPtr heapBuffer = Marshal.ReadIntPtr(pNativeData);
            if (heapBuffer != IntPtr.Zero)
                Marshal.FreeHGlobal(heapBuffer);
            Marshal.FreeHGlobal(pNativeData);
		}

		public int GetNativeDataSize ()
		{
			return Marshal.SizeOf<String>();
		}

		public IntPtr MarshalManagedToNative (object obj)
		{
			string s = obj as string;
			if (s == null)
				return IntPtr.Zero;

            byte[] bytes = Encoding.UTF8.GetBytes(s);
            int nativeSize = Marshal.SizeOf<String>();
            int sizeOffset = IntPtr.Size + 16;
            IntPtr ret = Marshal.AllocHGlobal(nativeSize);
            for (int i = 0; i < nativeSize; i++)
                Marshal.WriteByte(ret, i, 0);

            if (bytes.Length < 16)
            {
                Marshal.Copy(bytes, 0, IntPtr.Add(ret, IntPtr.Size), bytes.Length);
            }
            else
            {
                IntPtr heapBuffer = Marshal.AllocHGlobal(bytes.Length + 1);
                Marshal.Copy(bytes, 0, heapBuffer, bytes.Length);
                Marshal.WriteByte(heapBuffer, bytes.Length, 0);
                Marshal.WriteIntPtr(ret, heapBuffer);
                WriteNativeSize(ret, sizeOffset + UIntPtr.Size, bytes.Length + 1);
            }

            WriteNativeSize(ret, sizeOffset, bytes.Length);
			return ret;
		}

		public object MarshalNativeToManaged (IntPtr pNativeData)
		{
            if (pNativeData == IntPtr.Zero)
                return null;

            IntPtr heapBuffer = Marshal.ReadIntPtr(pNativeData);
            int sizeOffset = IntPtr.Size + 16;
            int length = ReadNativeSize(pNativeData, sizeOffset);
            IntPtr stringBuffer = heapBuffer == IntPtr.Zero
                ? IntPtr.Add(pNativeData, IntPtr.Size)
                : heapBuffer;
            return Marshal.PtrToStringUTF8(stringBuffer, length);
		}

        private static void WriteNativeSize(IntPtr ptr, int offset, int value)
        {
            if (UIntPtr.Size == 4)
                Marshal.WriteInt32(ptr, offset, value);
            else
                Marshal.WriteInt64(ptr, offset, value);
        }

        private static int ReadNativeSize(IntPtr ptr, int offset)
        {
            return UIntPtr.Size == 4
                ? Marshal.ReadInt32(ptr, offset)
                : checked((int)Marshal.ReadInt64(ptr, offset));
        }
	}
}
